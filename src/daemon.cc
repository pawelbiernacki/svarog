/***************************************************************************
 *   Copyright (C) 2020 by Pawel Biernacki                                 *
 *   pawel.f.biernacki@gmail.com                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "config.h"
#include "svarog.h"
#include "svarog_daemon.h"
#include <iostream>
#include <iomanip>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sstream>
#include <regex>
using namespace svarog;

int my_port = 5001;
bool my_port_known = false;

std::string my_password="1234";
bool my_password_known = false;


void svarog_server::send_message(const std::string & message)
{
	int n = write(sock, message.c_str(), message.size());
	if (n < 0)
	{
		syslog(LOG_PERROR, "svarog-daemon error writing");
	}
}
			
			
static void daemon()
{
	pid_t pid;
    
    pid = fork();
	
	if (pid < 0)
        exit(EXIT_FAILURE);
	
	if (pid > 0)
        exit(EXIT_SUCCESS);
	
	if (setsid() < 0)
        exit(EXIT_FAILURE);
	
	signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
	
	pid = fork();
	
	if (pid < 0)
        exit(EXIT_FAILURE);
    
    if (pid > 0)
        exit(EXIT_SUCCESS);
	
	umask(0);
    
    chdir("/root");
	
	int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close (x);
    }
    
    openlog ("svarog-daemon", LOG_PID, LOG_DAEMON);
}

static void doprocessing_client(int sock);


static void doprocessing()
{
    int portno, sockfd,pid;
    int newsockfd;
    unsigned clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int opt = 1;
	
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    if (sockfd < 0) {
        syslog (LOG_PERROR, "svarog-daemon failed to open socket.");
        exit(-1);
    }
    
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        syslog (LOG_PERROR, "svarog-daemon failed to set the socket options.");
        exit(EXIT_FAILURE); 
    } 
    
    int a=65535;
	if ( (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &a ,sizeof(int)) ) < 0 )
	{
		syslog(LOG_PERROR, "svarog-daemon options");
		exit(EXIT_FAILURE);
	}


    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = my_port;

	serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(portno);
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    {
        syslog (LOG_PERROR, "svarog-daemon error on binding.");
		close(sockfd);
        exit(-1);
    }

	listen(sockfd,5);
    clilen = sizeof(cli_addr);
    
    while (true) 
    {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		
        if (newsockfd < 0) 
        {
			syslog (LOG_PERROR, "svarog-daemon error on accept.");
			exit(1);
        }
      
        /* Create child process */
        pid = fork();
		
        if (pid < 0) {
			syslog (LOG_PERROR, "svarog-daemon error on fork.");
            exit(1);
        }
      
        if (pid == 0) {
         /* This is the client process */
            close(sockfd);
            doprocessing_client(newsockfd);
			syslog (LOG_NOTICE, "svarog-daemon client process terminated.");
            exit(0);
        }
        else 
        {
            close(newsockfd);
        }		
    }

}


static void doprocessing_client(int sock) 
{
	char buffer[1024];
	svarog_server server{sock};
	
	syslog (LOG_NOTICE, "svarog-daemon client processing.");

    bool reconnect = true;
    
	std::regex eof_regex("#END");
	
		std::stringstream input_buffer_stream;
		ssize_t n;
		do
		{
			bzero(buffer,1024);
			n = read(sock,buffer,1023);
   
			if (n < 0) 
			{
				syslog(LOG_PERROR, "svarog-daemon error reading");
				break;
			}
		
			if (n == 0)
				break;
			
			input_buffer_stream.write(buffer, strlen(buffer));
			
			if (std::regex_search(input_buffer_stream.str(), eof_regex))
				break;
		}
		while (true);
		
		
		//std::cout << input_buffer_stream.str();
		
		std::stringstream buffer_stream(input_buffer_stream.str());
		std::string line;
		std::getline(buffer_stream, line);
		
		std::stringstream line_stream(line);
		std::string program, password;
		if (line_stream.get() != '#')
		{
			syslog(LOG_PERROR, "svarog-daemon first line does not begin with #");
			server.send_message("error");
			return;
		}
		if (line_stream.get() != '!')
		{
			syslog(LOG_PERROR, "svarog-daemon first line does not begin with #!");
			server.send_message("error 0");
			return;
		}
		
		line_stream >> program >> password;
		if (line_stream.bad())
		{
			std::stringstream error_stream;
			error_stream << "svarog-daemon first line " << line;
			syslog(LOG_PERROR, error_stream.str().c_str());
			
			server.send_message("error 1");
			return;
		}
		
		if (program != "svarog")
		{
			syslog(LOG_PERROR, "svarog-daemon first line does not contain #!svarog");
			server.send_message("error 2");
			return;			
		}
		
		if (password != my_password)
		{
			syslog(LOG_PERROR, "svarog-daemon password mismatch");
			server.send_message("error 3");
			return;			
		}
		
		
		optimizer my_optimizer;
		
		if (my_optimizer.parse_buffer(input_buffer_stream.str().c_str()))
		{
			syslog(LOG_PERROR, "svarog-daemon syntax error");
			server.send_message(my_optimizer.get_error_message());
		}
		else
		{
			if (my_optimizer.get_contains_interactive_commands())
			{
				syslog(LOG_PERROR, "svarog-daemon interactive commands are not allowed");
				n = send(sock, "error 5", 8, 0);
				if (n < 0)
				{
					syslog(LOG_PERROR, "svarog-daemon error writing");
				}
			}
			else
			{
				try
				{
					my_optimizer.run();
					
					std::stringstream solution_stream;
					solution_stream << std::dec << std::showpoint << std::setw(10) 
					<< my_optimizer.get_last_solution();
					
					server.send_message(solution_stream.str());
					
				}
				catch (std::runtime_error & e)
				{
					std::stringstream error_stream;
					error_stream << "error 6 " << e.what();
					server.send_message(error_stream.str());
				}
			}
		}
}



int main(int argc, char * argv[])
{	
	std::cout << "svarog-daemon, copyright by Pawel Biernacki\n";
	std::cout << "Vantaa 2020\n";
	std::cout << PACKAGE_URL << "\n";
	
	for (int i=1; i<argc; i++)
	{
		std::string option(argv[i]);
		
		if (option=="-p" || option=="--port")
		{
			if (i == argc-1)
			{
				std::cerr << "option -p or --port require a port number\n";
				return -1;
			}
			std::string port(argv[i+1]);
			i++;
			my_port = std::stoi(port);
			my_port_known = true;
		}
		else
		if (option == "-s" || option == "--password")
		{
			if (i == argc-1)
			{
				std::cerr << "option -s or --password require a password\n";
				return -1;
			}
			std::string password(argv[i+1]);
			i++;
			my_password = password;
			my_password_known = true;
		}
		else
		if (option == "-h" || option == "--help")
		{
			std::cout << "usage: " << argv[0] 
			<< " [-p|--port <port>] [-s|--password <password>] [-h|--help]\n";
			return 0;
		}
	}
	
	if (!my_port_known)
	{
		std::cerr << "unknown port\n";
		return -1;
	}
	
	if (!my_password_known)
	{
		std::cerr << "unknown password\n";
		return -1;
	}
	
	daemon();
	while (1)
    {
        syslog (LOG_NOTICE, "svarog-daemon started.");
        doprocessing();
    }
   
    syslog (LOG_NOTICE, "svarog-daemon terminated.");
    closelog();
    return EXIT_SUCCESS;	
}
