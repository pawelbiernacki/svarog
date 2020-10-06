/***************************************************************************
 *   Copyright (C) 2019 by Pawel Biernacki                                 *
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
#include "svarog.h"
using namespace svarog;

void command_cout_perkun::execute(optimizer & o) const
{
	std::cout << "model\n{\n";
	for (std::list<visible_state*>::const_iterator i(o.vs.get_list_of_visible_states().begin());
		 i != o.vs.get_list_of_visible_states().end(); i++)
	{
		for (std::list<state*>::const_iterator j((*i)->get_list_of_states().begin()); j!=(*i)->get_list_of_states().end(); j++)
		{
			for (std::list<action*>::const_iterator k(o.a.get_list_of_actions().begin()); k != o.a.get_list_of_actions().end(); k++)
			{
				for (std::list<visible_state*>::const_iterator i2(o.vs.get_list_of_visible_states().begin()); i2 != o.vs.get_list_of_visible_states().end(); i2++)
				{
					for (std::list<state*>::const_iterator j2((*i2)->get_list_of_states().begin()); j2!=(*i2)->get_list_of_states().end(); j2++)
					{
						std::cout << "set({";
					
												bool first = true;
				
						for (int l = 0; l < o.v.get_vector_of_variables().size(); l++)
						{
							if (o.v.get_vector_of_variables()[l]->get_is_input_variable())
							{
								if (!first) std::cout << ",";
								std::cout << o.v.get_vector_of_variables()[l]->get_name() << "=>"
									<< (*i)->get_input_variable_value(o.v.get_vector_of_variables()[l])->get_name()
									<< " ";
								first = false;
							}
						}

						for (int l = 0; l < o.v.get_vector_of_variables().size(); l++)
						{
							if (o.v.get_vector_of_variables()[l]->get_is_hidden_variable())
							{
								if (!first) std::cout << ",";
								std::cout << o.v.get_vector_of_variables()[l]->get_name() << "=>"
									<< (*j)->get_hidden_variable_value(o.v.get_vector_of_variables()[l])->get_name()
									<< " ";
								first = false;
							}
						}

						std::cout << "},{";
				
						first = true;
				
						for (int l = 0; l < o.v.get_vector_of_variables().size(); l++)
						{
							if (o.v.get_vector_of_variables()[l]->get_is_output_variable())
							{
								if (!first) std::cout << ",";
								std::cout << o.v.get_vector_of_variables()[l]->get_name() << "=>"
									<< (*k)->get_output_variable_value(o.v.get_vector_of_variables()[l])->get_name()
									<< " ";
								first = false;
							}
						}
				
						std::cout << "},{";
						first = true;
						
						// input variables + hidden variables
						for (int l = 0; l < o.v.get_vector_of_variables().size(); l++)
						{
							if (o.v.get_vector_of_variables()[l]->get_is_input_variable())
							{
								if (!first) std::cout << ",";
								std::cout << o.v.get_vector_of_variables()[l]->get_name() << "=>"
									<< (*i2)->get_input_variable_value(o.v.get_vector_of_variables()[l])->get_name()
									<< " ";
								first = false;
							}
						}

						for (int l = 0; l < o.v.get_vector_of_variables().size(); l++)
						{
							if (o.v.get_vector_of_variables()[l]->get_is_hidden_variable())
							{
								if (!first) std::cout << ",";
								std::cout << o.v.get_vector_of_variables()[l]->get_name() << "=>"
									<< (*j2)->get_hidden_variable_value(o.v.get_vector_of_variables()[l])->get_name()
									<< " ";
								first = false;
							}
						}

						o.evaluating_expressions_mode = optimizer::GET_MODEL;
						o.get_model_state1 = *j;
						o.get_model_state2 = *j2;
						
						std::cout << "}," << std::showpoint << o.get_model_probability(*j, **k, *j2) << ");\n";
					}
				}
			}
		}
	}
	std::cout << "}\n";
}
