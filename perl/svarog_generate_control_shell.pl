#!/usr/bin/perl
=pod
/***************************************************************************
 *   Copyright (C) 2021 by Pawel Biernacki                                 *
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
 
 
=item svarog_generate_control_shell.pl

The script receives the amount of svarog visible states as the argument.

It prints out to the standard output a shell script that uses svarog
to control the knowledge from the KNOWLEDGE directory.

When errors occur during the execution of the result script it probably means
that some of the tasks are still running.

=cut
use strict;
use warnings;


my $amount_of_visible_states = shift;

die "usage: $0 <amount of visible states>" unless $amount_of_visible_states;

my $knowledge_directory = "KNOWLEDGE";

print <<EOL;
#!/usr/bin/bash

# This shell script has been created by $0 for $amount_of_visible_states visible states.

EOL

for my $i (1..$amount_of_visible_states)
{
    print <<EOL;
echo "$knowledge_directory/knowledge_$i.svarog"
svarog $knowledge_directory/knowledge_$i.svarog
EOL
}
