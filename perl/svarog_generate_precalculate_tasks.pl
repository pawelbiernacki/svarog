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
 
=item svarog_generate_precalculate_tasks.pl

This script expects the following arguments:
- depth (a small positive integer denoting the depth of the game tree for precalculations)
- granularity (a small integer >=2, preferably 2)
- specification file - name of a file containing a Svarog specification to be used for the precalculations
- visible states file - name of a text file produced by the Svarog command "cout << visible states << eol;" 

In the directory TASKS it produces the Svarog specifications named task_<number>.svarog that precalculate
knowledge for the given depth and granularity and the corresponding visible state.

=cut

use strict;
use warnings;


my $depth = shift;
my $granularity = shift; 
my $specification_file = shift;
my $visible_states_file = shift;

die "$0 <depth> <granularity> <specification file> <visible states file>" if !$depth or !$granularity or !$specification_file or !$visible_states_file;

my $tasks_directory = "TASKS";
my $knowledge_directory = "KNOWLEDGE";

mkdir $tasks_directory unless -d $tasks_directory;
mkdir $knowledge_directory unless -d $knowledge_directory;

local *VISIBLE_STATES;
local *TASK;

sub get_specification
{
    local *SPECIFICATION;

    open SPECIFICATION, "<$specification_file" or die $!;
    
    local $/ = undef;
    
    my $specification=<SPECIFICATION>;
    
    close SPECIFICATION or die $!;

    return $specification;
}


my $s = get_specification();

open VISIBLE_STATES, "<$visible_states_file" or die $!;
my $x = 1;

while (my $visible_state = <VISIBLE_STATES>)
{
    chomp($visible_state);
    #print "$x\n";
    
    open TASK, ">$tasks_directory/task_$x.svarog" or die $!;
    print TASK $s;
    print TASK <<ENDL;
cout << precalculate($depth,$granularity,$visible_state) << eol;
ENDL
    close TASK or die $!;
    
    $x++;
}

close VISIBLE_STATES or die $!;

print "svarog tasks in $tasks_directory have been created\n";
