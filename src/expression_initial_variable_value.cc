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
#include <stdexcept>
#include <sstream>
using namespace svarog;

void expression_initial_variable_value::get_value(optimizer & o, value_or_something_else & target) const
{
	variable * v = o.get_variable(variable_name.c_str());
	if (v == NULL)
	{
		std::stringstream s;
		s << "variable " << variable_name << " not found";
		throw std::runtime_error(s.str());
	}
	if (v->get_is_input_variable())
	{
		target.set_mode(value_or_something_else::VALUE);
		
		switch (o.evaluating_expressions_mode)
		{
			case optimizer::GET_MODEL:
				target.set_value(const_cast<value*>(o.get_model_state1->get_visible_state().get_input_variable_value(v)));		
				break;
				
			case optimizer::GET_IS_ILLEGAL:
				target.set_value(const_cast<value*>(o.get_illegal_visible_state1->get_input_variable_value(v)));		
				break;
				
			case optimizer::GET_POSSIBLE:
				if (o.get_possible_state1 == NULL)
					throw std::runtime_error("empty get_possible_state1");
				
				target.set_value(const_cast<value*>(o.get_possible_state1->get_visible_state().get_input_variable_value(v)));
				break;
				
			case optimizer::GET_PAYOFF:
				target.set_value(const_cast<value*>(o.get_payoff_visible_state1->get_input_variable_value(v)));
				break;
		}
	}
	else
	if (v->get_is_hidden_variable())
	{
		switch (o.evaluating_expressions_mode)
		{
			case optimizer::GET_MODEL:
				target.set_mode(value_or_something_else::VALUE);
				target.set_value(const_cast<value*>(o.get_model_state1->get_hidden_variable_value(v)));
				break;
				
			case optimizer::GET_POSSIBLE:
				target.set_mode(value_or_something_else::VALUE);
				target.set_value(const_cast<value*>(o.get_possible_state1->get_hidden_variable_value(v)));
				break;
				
			default:
				std::stringstream s;
				s << "wrong evaluating expression mode for hidden variable " << v->get_name() << " at line " << get_line_number();
				
				/*
				std::cerr << "COMMENT:";
				report_kuna(std::cerr);
				std::cerr << "\n";
				*/
				
				throw std::runtime_error(s.str());
		}
	}
	else
	{
		throw std::runtime_error("neither input nor hidden variable");
	}
}
