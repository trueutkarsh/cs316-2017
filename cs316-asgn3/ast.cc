#include<iostream>
#include<fstream>
#include<typeinfo>

using namespace std;

#include"common-classes.hh"
#include"error-display.hh"
#include"user-options.hh"
#include"local-environment.hh"
#include"symbol-table.hh"
#include"ast.hh"
#include"procedure.hh"
#include"program.hh"

Ast::Ast()
{}

Ast::~Ast()
{}

bool Ast::check_ast()
{
	stringstream msg;
	msg << "No check_ast() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Data_Type Ast::get_data_type()
{
	stringstream msg;
	msg << "No get_data_type() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

Symbol_Table_Entry & Ast::get_symbol_entry()
{
	stringstream msg;
	msg << "No get_symbol_entry() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

bool Ast::is_value_zero()
{
	stringstream msg;
	msg << "No is_value_zero() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::set_data_type(Data_Type dt)
{
	stringstream msg;
	msg << "No set_data_type() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
Sequence_Ast::Sequence_Ast(int line)
{
	lineno = line;
}

Sequence_Ast::~Sequence_Ast()
{

}

void Sequence_Ast:: ast_push_back(Ast * ast)
{
	statement_list.push_back(ast);
}

void Sequence_Ast:: print (ostream & file_buffer)
{
	
}


////////////////////////////////////////////////////////////////

Assignment_Ast::Assignment_Ast(Ast * temp_lhs, Ast * temp_rhs, int line)
{
	//ADD CODE HERE
	lhs = temp_lhs;
	rhs = temp_rhs;
	lineno = line;
	ast_num_child = binary_arity;
	node_data_type = lhs->get_data_type();

}

Assignment_Ast::~Assignment_Ast()
{
	//ADD CODE HERE
	delete lhs;
	delete rhs;

}

bool Assignment_Ast::check_ast()
{
	CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");
	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");

	// use typeid(), get_data_type()
	//ADD CODE HERE
	// CHECK_INVARIANT((lhs->get_data_type() == rhs->get_data_type()), "Lhs of Assignment_Ast cannot be null");


	bool result;

	result = lhs->get_data_type() == rhs->get_data_type();

	if(!result){
		CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, "Arithmetic statement data type not compatible", lineno);
	}

	return result;
}

void Assignment_Ast::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer<<"\n Asgn: \n";
	lhs->print(file_buffer);
	file_buffer<<" = ";
	rhs->print(file_buffer);
	file_buffer<<"\n";
}

/////////////////////////////////////////////////////////////////

Name_Ast::Name_Ast(string & name, Symbol_Table_Entry & var_entry, int line)
{

	variable_symbol_entry = &var_entry;
	CHECK_INVARIANT((variable_symbol_entry->get_variable_name() == name),
		"Variable's symbol entry is not matching its name");
	//ADD CODE HERE
	lineno = line;
	ast_num_child = zero_arity;
	node_data_type = variable_symbol_entry->get_data_type();
}

Name_Ast::~Name_Ast()
{}

Data_Type Name_Ast::get_data_type()
{
	// refer to functions for Symbol_Table_Entry 

	return variable_symbol_entry->get_data_type();
	//ADD CODE HERE
}

Symbol_Table_Entry & Name_Ast::get_symbol_entry()
{
	//ADD CODE HERE
	CHECK_INVARIANT((variable_symbol_entry != NULL), "Vairable_Symbol_Table_Entry cannot be null");
	return *variable_symbol_entry;
}

void Name_Ast::set_data_type(Data_Type dt)
{
	//ADD CODE HERE
	CHECK_INVARIANT((variable_symbol_entry != NULL), "Vairable_Symbol_Table_Entry cannot be null");
	variable_symbol_entry->set_data_type(dt);
	node_data_type = dt;
}

void Name_Ast::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer<<" ";
	file_buffer<<variable_symbol_entry->get_variable_name();
	file_buffer<<" ";
}

///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::Number_Ast(DATA_TYPE number, Data_Type constant_data_type, int line)
{
	// use Ast_arity from ast.hh
	//ADD CODE HERE
	constant = number;
	node_data_type = constant_data_type;
	lineno = line;
	ast_num_child = zero_arity;
	
}

template <class DATA_TYPE>
Number_Ast<DATA_TYPE>::~Number_Ast()
{}

template <class DATA_TYPE>
Data_Type Number_Ast<DATA_TYPE>::get_data_type()
{
	//ADD CODE HERE
	return node_data_type;
}

template <class DATA_TYPE>
void Number_Ast<DATA_TYPE>::set_data_type(Data_Type dt)
{
	//ADD CODE HERE
	node_data_type = dt;
}

template <class DATA_TYPE>
bool Number_Ast<DATA_TYPE>::is_value_zero()
{
	//ADD CODE HERE
	// if (constant == 0){

	// 	return true;
	// }
	// else{

	// 	return false;
	// }
	bool result;
	if (node_data_type == int_data_type){
		result = constant == 0;
	}
	else if (node_data_type == double_data_type){
		result = constant == 0.0;
	}

	return result;

}

template <class DATA_TYPE>
void Number_Ast<DATA_TYPE>::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer<<constant;
	file_buffer<<" ";

}

///////////////////////////////////////////////////////////////////////////////

Data_Type Arithmetic_Expr_Ast::get_data_type()
{
	//ADD CODE HERE
	return node_data_type;
}

void Arithmetic_Expr_Ast::set_data_type(Data_Type dt)
{
	//ADD CODE HERE
	node_data_type = dt;

}

bool Arithmetic_Expr_Ast::check_ast()
{
	// use get_data_type(), typeid()
	//ADD CODE HERE
	// CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, "Arithmetic statement data type not compatible", lineno);
	// CHECK_INVARIANT((rhs != NULL), "Rhs of Assignment_Ast cannot be null");

	CHECK_INVARIANT((lhs != NULL), "Lhs of Assignment_Ast cannot be null");


	bool result ;
	if (ast_num_child == binary_arity){

		result = lhs->get_data_type() == rhs->get_data_type();
	}
	else{

		result = true; 
	}

	if(!result){
		CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, "Arithmetic statement data type not compatible", lineno);
	}

	return result;

}

/////////////////////////////////////////////////////////////////////

Plus_Ast::Plus_Ast(Ast * l, Ast * r, int line)
{
	// set arity and data type
	//ADD CODE HERE
	lhs = l;
	rhs = r;
	lineno = line;
	ast_num_child = binary_arity;
	node_data_type = lhs->get_data_type();
}

void Plus_Ast::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer<<"\n Plus \n LHS: \n";
	lhs->print(file_buffer);
	file_buffer<<" \n RHS: \n";
	rhs->print(file_buffer);
	file_buffer<<"\n";
}

/////////////////////////////////////////////////////////////////

Minus_Ast::Minus_Ast(Ast * l, Ast * r, int line)
{
	//ADD CODE HERE
	lhs = l;
	rhs = r;
	lineno = line;
	ast_num_child = binary_arity;
	node_data_type = lhs->get_data_type();

}

void Minus_Ast::print(ostream & file_buffer)
{
	//ADD CODE HERE	
	file_buffer<<"\n Minus  LHS: ";
	lhs->print(file_buffer);
	file_buffer<<" - \n RHS: \n";
	rhs->print(file_buffer);
	file_buffer<<"\n";
}

//////////////////////////////////////////////////////////////////

Mult_Ast::Mult_Ast(Ast * l, Ast * r, int line)
{
	//ADD CODE HERE
	lhs = l;
	rhs = r;
	lineno = line;
	ast_num_child = binary_arity;
	node_data_type = lhs->get_data_type();
}

void Mult_Ast::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer<<"\n Mult :LHS: ";
	lhs->print(file_buffer);
	file_buffer<<" * \n RHS: \n";
	rhs->print(file_buffer);
	file_buffer<<"\n";
}

////////////////////////////////////////////////////////////////////

Divide_Ast::Divide_Ast(Ast * l, Ast * r, int line)
{
	//ADD CODE HERE
	lhs = l;
	rhs = r;
	lineno = line;
	ast_num_child = binary_arity;
	node_data_type = lhs->get_data_type();
}

void Divide_Ast::print(ostream & file_buffer)
{
	//ADD CODE HERE

	file_buffer<<"\n Divide :LHS: ";
	lhs->print(file_buffer);
	file_buffer<<" / \n RHS: \n";
	rhs->print(file_buffer);
	file_buffer<<"\n";
}

//////////////////////////////////////////////////////////////////////

UMinus_Ast::UMinus_Ast(Ast * l, Ast * r, int line)
{
	//ADD CODE HERE
	lhs = l;
	rhs = r;
	lineno = line;
	ast_num_child = unary_arity;
	node_data_type = lhs->get_data_type();
}

void UMinus_Ast::print(ostream & file_buffer)
{
	//ADD CODE HERE
	file_buffer<<"\n Uminus :LHS: ";
	lhs->print(file_buffer);
	file_buffer<<"\n";
}


template class Number_Ast<double>;
template class Number_Ast<int>;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////LAB 3 CODE FROM HERE///////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
