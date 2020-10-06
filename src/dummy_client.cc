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
#include <string.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <strings.h>
#include <stdio.h>
#include <sys/types.h>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>


#define SVAROG_MAX_MESSAGE_FROM_SERVER 256

int main(int argc, char *argv[])
{
	char buffer[SVAROG_MAX_MESSAGE_FROM_SERVER];
	int sockfd;
	hostent * server;
	sockaddr_in serv_addr;
	
	
	if (argc != 5)
	{
		std::cerr << "usage: " << argv[0] << " <file> <server_name> <port> <password>\n";
		return -1;
	}
	
	std::cout << "svarog-dummy-client, copyright by Pawel Biernacki\n";
	std::cout << "Vantaa 2020\n";
	std::cout << PACKAGE_URL << "\n";
	
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   
    if (sockfd < 0) {
		std::cerr << "error opening socket\n";
	}
	
	server = gethostbyname(argv[2]);

	bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(atoi(argv[3]));


	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
	{
		std::cerr << "connection error\n";
		close(sockfd);
		return -1;
	}
	
	
	ssize_t n;
	std::ifstream file_stream(argv[1]);
	
	if (!file_stream.good())
	{
		std::cerr << "error reading file " << argv[1] << "\n";
		close(sockfd);
		return -1;
	}

	std::stringstream s;
	
	s << "#!svarog " << argv[4] << "\n";
	
	
	timeval tv;
    tv.tv_sec = 10000;  // secs timeout
    tv.tv_usec = 0;

	if (setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,(char *)&tv,sizeof(tv)) < 0)
    {
        std::cerr << "timeout\n";
		close(sockfd);
        return -1;
    }
    int a=65535;
	if (setsockopt( sockfd, SOL_SOCKET, SO_SNDBUF, &a, sizeof(int)) < 0)
	{
		std::cerr << "sndbuf\n";
		close(sockfd);
		return -1;
	}

	std::string line;
	
	n = write(sockfd, s.str().c_str(), s.str().size());
	if (n < 0) 
	{
		std::cerr << "error writing\n";
		close(sockfd);
		return -1;
	}
	
	char output_buffer[1024];
	memset(output_buffer, 0, 1024);
	
	while (file_stream.read(output_buffer, 1023))
	{
		n = write(sockfd, output_buffer, strlen(output_buffer));
		if (n < 0) 
		{
			std::cerr << "error writing\n";
			close(sockfd);
			return -1;
		}
		//std::cout << output_buffer;
		memset(output_buffer, 0, 1024);
	}

	if (file_stream.eof())
	{
		n = write(sockfd, output_buffer, strlen(output_buffer));
		if (n < 0) 
		{
			std::cerr << "error writing\n";
			close(sockfd);
			return -1;
		}
	}
	
	n = write(sockfd, "#END\n", 6);
		if (n < 0) 
		{
			std::cerr << "error writing\n";
			close(sockfd);
			return -1;
		}
	

	std::cout << "written message to server\n";
	
	bzero(buffer,SVAROG_MAX_MESSAGE_FROM_SERVER);
   
	
	tv.tv_sec = 100;  // secs timeout
    tv.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,(char *)&tv,sizeof(tv)) < 0)
    {
        std::cerr << "timeout\n";
		close(sockfd);
        return -1;
    }
	
	std::cout << "attempt to read from server\n";
	n = read(sockfd, buffer, SVAROG_MAX_MESSAGE_FROM_SERVER-1);
 
	if (n < 0) 
	{
		std::cerr << "error reading\n";
		close(sockfd);
		return -1;
	}
	
	std::cout << "got " << buffer << "\n";
	
	close(sockfd);
	
	return 0;
}
