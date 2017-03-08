#include <iostream>
#include <fstream>
#include <typeinfo>

using namespace std;

#include "common-classes.hh"
#include "error-display.hh"
#include "user-options.hh"
#include "local-environment.hh"
#include "symbol-table.hh"
#include "ast.hh"
#include "procedure.hh"
#include "program.hh"

// declare the static variable here
int Ast::labelCounter = 0;

Ast::Ast()
{
}

Ast::~Ast()
{
}

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

Symbol_Table_Entry &Ast::get_symbol_entry()
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

Assignment_Ast::Assignment_Ast(Ast *temp_lhs, Ast *temp_rhs, int line)
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

    if (!result)
    {
	CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, "Assignment statement data type not compatible", lineno);
    }

    return result;
}

void Assignment_Ast::print(ostream &file_buffer)
{
    //ADD CODE HERE
    file_buffer << "\n Asgn: \n";
    lhs->print(file_buffer);
    file_buffer << " = ";
    rhs->print(file_buffer);
    file_buffer << "\n";
}

/////////////////////////////////////////////////////////////////

Name_Ast::Name_Ast(string &name, Symbol_Table_Entry &var_entry, int line)
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
{
}

Data_Type Name_Ast::get_data_type()
{
    // refer to functions for Symbol_Table_Entry

    return variable_symbol_entry->get_data_type();
    //ADD CODE HERE
}

Symbol_Table_Entry &Name_Ast::get_symbol_entry()
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

void Name_Ast::print(ostream &file_buffer)
{
    //ADD CODE HERE
    file_buffer << " ";
    file_buffer << variable_symbol_entry->get_variable_name();
    file_buffer << " ";
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
{
}

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
    if (node_data_type == int_data_type)
    {
	result = constant == 0;
    }
    else if (node_data_type == double_data_type)
    {
	result = constant == 0.0;
    }

    return result;
}

template <class DATA_TYPE>
void Number_Ast<DATA_TYPE>::print(ostream &file_buffer)
{
    //ADD CODE HERE
    file_buffer << constant;
    file_buffer << " ";
}

///////////////////////////////////////////////////////////////////////////////

Arithmetic_Expr_Ast::~Arithmetic_Expr_Ast()
{
}

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

    bool result;
    if (ast_num_child == binary_arity)
    {

	result = lhs->get_data_type() == rhs->get_data_type();
    }
    else
    {

	result = true;
    }

    if (!result)
    {
	CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, "Arithmetic statement data type not compatible", lineno);
    }

    return result;
}

/////////////////////////////////////////////////////////////////////

Plus_Ast::Plus_Ast(Ast *l, Ast *r, int line)
{
    // set arity and data type
    //ADD CODE HERE
    lhs = l;
    rhs = r;
    lineno = line;
    ast_num_child = binary_arity;
    node_data_type = lhs->get_data_type();
}

void Plus_Ast::print(ostream &file_buffer)
{
    //ADD CODE HERE
    file_buffer << "\n Plus \n LHS: \n";
    lhs->print(file_buffer);
    file_buffer << " \n RHS: \n";
    rhs->print(file_buffer);
    file_buffer << "\n";
}

/////////////////////////////////////////////////////////////////

Minus_Ast::Minus_Ast(Ast *l, Ast *r, int line)
{
    //ADD CODE HERE
    lhs = l;
    rhs = r;
    lineno = line;
    ast_num_child = binary_arity;
    node_data_type = lhs->get_data_type();
}

void Minus_Ast::print(ostream &file_buffer)
{
    //ADD CODE HERE
    file_buffer << "\n Minus  LHS: ";
    lhs->print(file_buffer);
    file_buffer << " - \n RHS: \n";
    rhs->print(file_buffer);
    file_buffer << "\n";
}

//////////////////////////////////////////////////////////////////

Mult_Ast::Mult_Ast(Ast *l, Ast *r, int line)
{
    //ADD CODE HERE
    lhs = l;
    rhs = r;
    lineno = line;
    ast_num_child = binary_arity;
    node_data_type = lhs->get_data_type();
}

void Mult_Ast::print(ostream &file_buffer)
{
    //ADD CODE HERE
    file_buffer << "\n Mult :LHS: ";
    lhs->print(file_buffer);
    file_buffer << " * \n RHS: \n";
    rhs->print(file_buffer);
    file_buffer << "\n";
}

////////////////////////////////////////////////////////////////////

Divide_Ast::Divide_Ast(Ast *l, Ast *r, int line)
{
    //ADD CODE HERE
    lhs = l;
    rhs = r;
    lineno = line;
    ast_num_child = binary_arity;
    node_data_type = lhs->get_data_type();
}

void Divide_Ast::print(ostream &file_buffer)
{
    //ADD CODE HERE

    file_buffer << "\n Divide :LHS: ";
    lhs->print(file_buffer);
    file_buffer << " / \n RHS: \n";
    rhs->print(file_buffer);
    file_buffer << "\n";
}

//////////////////////////////////////////////////////////////////////

UMinus_Ast::UMinus_Ast(Ast *l, Ast *r, int line)
{
    //ADD CODE HERE
    lhs = l;
    rhs = r;
    lineno = line;
    ast_num_child = unary_arity;
    node_data_type = lhs->get_data_type();
}

void UMinus_Ast::print(ostream &file_buffer)
{
    //ADD CODE HERE
    file_buffer << "\n Uminus :LHS: ";
    lhs->print(file_buffer);
    file_buffer << "\n";
}

template class Number_Ast<double>;
template class Number_Ast<int>;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
///////////////////////////LAB 3 CODE FROM HERE///////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
Sequence_Ast::Sequence_Ast(int line)
{
    // initialize variables
    lineno = line;
    node_data_type = void_data_type;
    ast_num_child = zero_arity;
}

Sequence_Ast::~Sequence_Ast()
{
    // Go through each element in statement list and delete it
    while( !statement_list.empty() ){
        delete statement_list.front();
        statement_list.pop_front();
    }
}

void Sequence_Ast::ast_push_back(Ast *ast)
{
    // add it to statement_list
    statement_list.push_back(ast);
}

void Sequence_Ast::print(ostream &file_buffer)
{
    file_buffer<<"Sequence_Ast->"<<endl;
    // iterate through each ast
    for(list<Ast*>::iterator it = statement_list.begin() ; it!=statement_list.end(); it++)
    {
        // print it 
        (*it)->print(file_buffer);
    }
}


////////////////////////////////////////////////////////////////

Relational_Expr_Ast::Relational_Expr_Ast(Ast * lhs, Relational_Op rop, Ast * rhs, int line)
{   
    // initialize variables
    lhs_condition = lhs;
    rel_op = rop;
    rhs_condition = rhs;
    lineno = line;
    // set default data type to int_data_type
    node_data_type = int_data_type;
    ast_num_child = binary_arity;
}

Relational_Expr_Ast:: ~Relational_Expr_Ast()
{
    // delete lhs and rhs
    delete lhs_condition;
    delete rhs_condition;
}

Data_Type Relational_Expr_Ast:: get_data_type()
{
    return node_data_type;
}

void Relational_Expr_Ast::set_data_type(Data_Type dt)
{
    node_data_type = dt;
}

bool Relational_Expr_Ast::check_ast()
{
    // check that both left and right have same data type
    if (ast_num_child == binary_arity)
    {
        CHECK_INPUT(lhs_condition->get_data_type() == rhs_condition->get_data_type(), "Relational statement data type not compatible", lineno);
        return true;
    }
    
    CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, "Relational statement data type not compatible", lineno);

}

void Relational_Expr_Ast::print(ostream & file_buffer)
{
    file_buffer<<endl<<AST_NODE_SPACE<<"Condition: ";
    if (rel_op == less_than)
    {
        file_buffer<<" LT ";
    }
    else if(rel_op == greater_than)
    {
        file_buffer<<" GT ";
    }
    else if(rel_op == less_equalto)
    {
        file_buffer<<" LE ";
    }
    else if(rel_op == greater_equalto)
    {
        file_buffer<<" GE ";
    }
    else if(rel_op == equalto)
    {
        file_buffer<<" EQ ";
    }
    else if(rel_op == not_equalto)
    {
        file_buffer<<" NE ";
    }
    file_buffer<<endl;
    file_buffer<<AST_NODE_SPACE<<"LHS (";
    lhs_condition->print(file_buffer);
    file_buffer<<") "<<endl<<"RHS (";
    rhs_condition->print(file_buffer);
    file_buffer<<" )";
}

////////////////////////////////////////////////////////////////

Boolean_Expr_Ast::Boolean_Expr_Ast(Ast * lhs, Boolean_Op bop, Ast * rhs, int line)
{
    // initialize variables
    lhs_op = lhs;
    rhs_op = rhs;
    bool_op = bop;
    node_data_type = rhs->get_data_type();
    ast_num_child = binary_arity;
}

Boolean_Expr_Ast::~Boolean_Expr_Ast()
{
    delete lhs_op;
    delete rhs_op;
}

Data_Type Boolean_Expr_Ast::get_data_type()
{
    return node_data_type;
}

void Boolean_Expr_Ast::set_data_type(Data_Type dt)
{
    node_data_type = dt;
}

bool Boolean_Expr_Ast::check_ast()
{
    // no data type for bool so just just check the type_id
    // check conditions
    // possible cases B B, R B, B R, R R
    bool result = true;

    bool cond1 = (typeid(*lhs_op) == typeid(Relational_Expr_Ast)) || (typeid(*lhs_op) == typeid(Boolean_Expr_Ast) );
    bool cond2 = (typeid(*rhs_op) == typeid(Relational_Expr_Ast)) || (typeid(*rhs_op) == typeid(Boolean_Expr_Ast)) ;
    if (lhs_op == NULL)
    {
        result = result && cond2;
    }
    else
    {
        result = result && cond2 && cond1;

    }

    if (! result)
    {
        CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, "Boolean statement data type not compatible", lineno);    
    }

    return result;
}


void Boolean_Expr_Ast::print(ostream  & file_buffer)
{

    file_buffer<<endl<<AST_NODE_SPACE<<"Condition: "<<endl;
    if (bool_op == boolean_or)
    {
        file_buffer<<" OR ";
    }
    else if(bool_op == boolean_and)
    {
        file_buffer<<" AND ";
    }
    else if(bool_op ==boolean_not)
    {
        file_buffer<<" NOT ";
    }
    file_buffer<<endl;
    file_buffer<<AST_SUB_NODE_SPACE<<"LHS (";
    lhs_op->print(file_buffer);
    file_buffer<<") "<<endl<<AST_SUB_NODE_SPACE<<"RHS (";
    rhs_op->print(file_buffer);
    file_buffer<<" )"<<endl;    
}


////////////////////////////////////////////////////////////////
Selection_Statement_Ast::Selection_Statement_Ast(Ast * cond,Ast* then_part, Ast* else_part, int line)
{
    // ask why this ??
    this->cond = cond;
    this->then_part = then_part;
    this->else_part = else_part;
    lineno = line;
    ast_num_child = ternary_arity;
    node_data_type = void_data_type;

} 

Selection_Statement_Ast::~Selection_Statement_Ast()
{
	delete cond;
	delete then_part;
    if (else_part != NULL)
	{
        delete else_part;
    }
}

Data_Type Selection_Statement_Ast::get_data_type()
{
	return node_data_type;
}

void Selection_Statement_Ast::set_data_type(Data_Type dt)
{
	node_data_type = dt;
}

bool Selection_Statement_Ast::check_ast()
{
    // constraints
    // cond data type Boolean_Expr_Ast
    bool result = true;

    bool cond1 = (typeid(*this->cond) == typeid(Boolean_Expr_Ast)) || (typeid(*this->cond) == typeid(Relational_Expr_Ast));

    // cout<<"typeid(*this->cond) -> "<<typeid(*this->cond)<<endl;

    result = result && cond1;

    if(!result)
    {
        CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, "Selection statement  data type not compatible", lineno);    
    }

    return result;

}

void Selection_Statement_Ast::print(ostream & file_buffer)
{
	file_buffer<<endl<<AST_SPACE<<"IF -> "<<endl;
	file_buffer<<AST_SPACE<<"CONDITION (";
	cond->print(file_buffer);
	file_buffer<<")"<<endl;
	file_buffer<<AST_SPACE<<"THEN (";
	then_part->print(file_buffer);
	file_buffer<<")"<<endl;
    if(else_part != NULL)
    {
	file_buffer<<AST_SPACE<<"ELSE (";
	else_part->print(file_buffer);
	file_buffer<<")";
    }
}




////////////////////////////////////////////////////////////////

Iteration_Statement_Ast::Iteration_Statement_Ast(Ast * cond, Ast* body, int line, bool do_form)
{
	this->cond = cond;
	this->body = body;
	is_do_form = do_form;
	lineno = line;
	ast_num_child = binary_arity;
	node_data_type = void_data_type;
	// get_new_label();
}

Iteration_Statement_Ast::~Iteration_Statement_Ast()
{
	delete cond;
	delete body;
}

Data_Type Iteration_Statement_Ast::get_data_type()
{
	return node_data_type;
}

void Iteration_Statement_Ast::set_data_type(Data_Type dt)
{
	node_data_type = dt;
}

bool Iteration_Statement_Ast::check_ast()
{
	
    // constraints
    // cond data type Boolean_Expr_Ast
    bool result = true;

    bool cond1 = ( typeid(*this->cond) == typeid(Boolean_Expr_Ast) || (typeid(*this->cond) == typeid(Relational_Expr_Ast) ));

    result = result && cond1;

    if(!result)
    {
        CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, "Iteration statement data type not compatible", lineno);    
    }

    return result;

}

void Iteration_Statement_Ast::print(ostream & file_buffer)
{
	file_buffer<<endl<<AST_SPACE<<"WHILE -> "<<endl;
	file_buffer<<AST_SPACE<<"CONDITION (";
	cond->print(file_buffer);
	file_buffer<<")"<<endl;
	file_buffer<<AST_SPACE<<"BODY (";
	body->print(file_buffer);
	file_buffer<<")";
}

////////////////////////////////////////////////////////////////


Conditional_Operator_Ast::Conditional_Operator_Ast(Ast* cond, Ast* l, Ast* r, int line)
{
	this->cond = cond;
	lhs = l;
	rhs = r;
	lineno = line;
	node_data_type = lhs->get_data_type();
	ast_num_child = ternary_arity;
	// get_new_label();
}

Conditional_Operator_Ast::~Conditional_Operator_Ast()
{
	delete this->cond;
	delete lhs;
	delete rhs;
}

void Conditional_Operator_Ast::print(ostream & file_buffer)
{
	file_buffer<<endl<<AST_NODE_SPACE<<"Arith: Conditional"<<endl;
	file_buffer<<AST_SUB_NODE_SPACE<<"COND (";
	cond->print(file_buffer);
	file_buffer<<"LHS ";
	lhs->print(file_buffer);
	file_buffer<<"RHS ";
	rhs->print(file_buffer);
	file_buffer<<")";
}



////////////////////////////////////////////////////////////////
