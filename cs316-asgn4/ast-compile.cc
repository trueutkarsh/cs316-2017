#include <iostream>
#include <fstream>
#include <typeinfo>

using namespace std;

#include "common-classes.hh"
#include "error-display.hh"
#include "user-options.hh"
#include "icode.hh"
#include "reg-alloc.hh"
#include "symbol-table.hh"
#include "ast.hh"
#include "procedure.hh"
#include "program.hh"

Code_For_Ast & Ast::create_store_stmt(Register_Descriptor * store_register)
{
	stringstream msg;
	msg << "No create_store_stmt() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::print_assembly()
{
	stringstream msg;
	msg << "No print_assembly() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

void Ast::print_icode()
{
	stringstream msg;
	msg << "No print_icode() function for " << typeid(*this).name();
	CHECK_INVARIANT(CONTROL_SHOULD_NOT_REACH, msg.str());
}

////////////////////////////////////////////////////////////////

Code_For_Ast & Assignment_Ast::compile()
{
	CHECK_INVARIANT((lhs != NULL), "Lhs cannot be null in Assignment_Ast");
	CHECK_INVARIANT((rhs != NULL), "Rhs cannot be null in Assignment_Ast");

	Code_For_Ast & load_stmt = rhs->compile();

	Register_Descriptor * load_register = load_stmt.get_reg();
	CHECK_INVARIANT(load_register, "Load register cannot be null in Assignment_Ast");
	load_register->set_use_for_expr_result();

	Code_For_Ast store_stmt = lhs->create_store_stmt(load_register);

	CHECK_INVARIANT((load_register != NULL), "Load register cannot be null in Assignment_Ast");
	load_register->reset_use_for_expr_result();

	// Store the statement in ic_list

	list<Icode_Stmt *> & ic_list = *new list<Icode_Stmt *>;

	if (load_stmt.get_icode_list().empty() == false)
		ic_list = load_stmt.get_icode_list();

	if (store_stmt.get_icode_list().empty() == false)
		ic_list.splice(ic_list.end(), store_stmt.get_icode_list());

	Code_For_Ast * assign_stmt;
	if (ic_list.empty() == false)
		assign_stmt = new Code_For_Ast(ic_list, load_register);

	return *assign_stmt;
}


/////////////////////////////////////////////////////////////////

Code_For_Ast & Name_Ast::compile()
{	
	// declare operand here
	Mem_Addr_Opd * mem_opd = new Mem_Addr_Opd(*variable_symbol_entry);
	// declare new register descriptor here
	machine_desc_object.clear_local_register_mappings();
	Register_Descriptor * load_register;

	if (get_data_type() == double_data_type)
	{
		load_register = machine_desc_object.get_new_register<float_reg>();
	}
	else{
		load_register = machine_desc_object.get_new_register<gp_data>();
	}

	Register_Addr_Opd * reg_opd = new Register_Addr_Opd(load_register);
	Move_IC_Stmt * store_stmt = new Move_IC_Stmt(load, mem_opd, reg_opd);
	list<Icode_Stmt *> & icslist = *new list<Icode_Stmt*>();
	icslist.push_back(store_stmt);
	Code_For_Ast * cfa = new Code_For_Ast(icslist, load_register );
	return *cfa;	
}

Code_For_Ast & Name_Ast::create_store_stmt(Register_Descriptor * store_register)
{
	// lhs = rhs
	// 	
	Mem_Addr_Opd * mem_opd = new Mem_Addr_Opd(*variable_symbol_entry);
	Register_Addr_Opd * reg_opd = new Register_Addr_Opd(store_register);
	Move_IC_Stmt * store_stmt = new Move_IC_Stmt(store, reg_opd, mem_opd);
	list<Icode_Stmt *> & icslist = *new list<Icode_Stmt*>();
	icslist.push_back(store_stmt);
	Code_For_Ast * cfa = new Code_For_Ast(icslist, store_register );
	return *cfa;
}


///////////////////////////////////////////////////////////////////////////////

template <class DATA_TYPE>
Code_For_Ast & Number_Ast<DATA_TYPE>::compile()
{
	Const_Opd<DATA_TYPE> * const_opd = new Const_Opd<DATA_TYPE>(constant);
	// declare new register descriptor here
	machine_desc_object.clear_local_register_mappings();
	Register_Descriptor * load_register;

	if (get_data_type() == double_data_type)
	{
		load_register = machine_desc_object.get_new_register<float_reg>();
	}
	else{
		load_register = machine_desc_object.get_new_register<gp_data>();
	}

	Register_Addr_Opd * reg_opd = new Register_Addr_Opd(load_register);
	Move_IC_Stmt * store_stmt = new Move_IC_Stmt(load, const_opd, reg_opd);
	list<Icode_Stmt *> & icslist = *new list<Icode_Stmt*>();
	icslist.push_back(store_stmt);
	Code_For_Ast * cfa = new Code_For_Ast(icslist, load_register );
	return *cfa;	
}

///////////////////////////////////////////////////////////////////////////////

Code_For_Ast & Relational_Expr_Ast::compile()
{
	// find the code for ast for lhs
	Code_For_Ast & lhs_stmt = lhs_condition->compile();
	// find the code for ast for rhs
	Code_For_Ast & rhs_stmt = rhs_condition->compile();
	// create a new icslist
	list<Icode_Stmt *> & icslist = *new list<Icode_Stmt*>();	
	// add both the list elements to it
	icslist.splice(icslist.end(), lhs_stmt.get_icode_list());
	icslist.splice(icslist.end(), rhs_stmt.get_icode_list());
	// create a new register for result 
	// declare new register descriptor here
	machine_desc_object.clear_local_register_mappings();
	Register_Descriptor * result_register;
	// return type always int
	result_register = machine_desc_object.get_new_register<gp_data>();

	// create a new compute stmt with arguments as result of both ast, result as new register
	Ics_Opd * result_opd = new Register_Addr_Opd(result_register);
	Ics_Opd * lhs_opd = new Register_Addr_Opd(lhs_stmt.get_reg());
	Ics_Opd * rhs_opd = new Register_Addr_Opd(rhs_stmt.get_reg());
	// make cases on relational operator
	Tgt_Op op;
	if (rel_op == less_than ) { op = slt;}
	else if ( rel_op == less_equalto) { op = sle; }
	else if ( rel_op == greater_than) { op = sgt; }
	else if ( rel_op == greater_equalto) { op = sge; }
	else if ( rel_op == equalto ) { op = seq; }
	else if ( rel_op == not_equalto ) { op = sne; }

	Compute_IC_Stmt * add_stmt = new Compute_IC_Stmt(op, result_opd, lhs_opd, rhs_opd);
	// add it to icslist
	icslist.push_back(add_stmt);
	// return the Code_For_Ast with icslist and result register
	Code_For_Ast * cfa = new Code_For_Ast(icslist, result_register );
	return *cfa;	
}

//////////////////////////////////////////////////////////////////////

Code_For_Ast & Boolean_Expr_Ast::compile()
{
	Code_For_Ast * cfa;

	if (lhs_op != NULL){
		// find the code for ast for lhs
		Code_For_Ast & lhs_stmt = lhs_op->compile();
		// find the code for ast for rhs
		Code_For_Ast & rhs_stmt = rhs_op->compile();
		// create a new icslist
		list<Icode_Stmt *> & icslist = *new list<Icode_Stmt*>();	
		// add both the list elements to it
		icslist.splice(icslist.end(), lhs_stmt.get_icode_list());
		icslist.splice(icslist.end(), rhs_stmt.get_icode_list());
		// create a new register for result 
		// declare new register descriptor here
		machine_desc_object.clear_local_register_mappings();
		Register_Descriptor * result_register;

		result_register = machine_desc_object.get_new_register<gp_data>();

		// create a new compute stmt with arguments as result of both ast, result as new register
		Ics_Opd * result_opd = new Register_Addr_Opd(result_register);
		Ics_Opd * lhs_opd = new Register_Addr_Opd(lhs_stmt.get_reg());
		Ics_Opd * rhs_opd = new Register_Addr_Opd(rhs_stmt.get_reg());
		// make cases on relational operator
		Tgt_Op op;
		if (bool_op == boolean_and ) { op = and_t;}
		else if ( bool_op == boolean_or) { op = or_t; }


		Compute_IC_Stmt * add_stmt = new Compute_IC_Stmt(op, result_opd, lhs_opd, rhs_opd);
		// add it to icslist
		icslist.push_back(add_stmt);
		// return the Code_For_Ast with icslist and result register
		cfa = new Code_For_Ast(icslist, result_register );
	}
	else{
		// raise a BUG
	}
	return *cfa;	
	
}
///////////////////////////////////////////////////////////////////////

Code_For_Ast & Selection_Statement_Ast::compile()
{
	// if else statement
	// compute ic for condition 
	// make a 
}

///////////////////////////////////////////////////////////////////////////////////////////

Code_For_Ast & Iteration_Statement_Ast::compile()
{
	
}

///////////////////////////////////////////////////////////////////////////////////////////

Code_For_Ast & Plus_Ast::compile()
{
	// find the code for ast for lhs
	Code_For_Ast & lhs_stmt = lhs->compile();
	// find the code for ast for rhs
	Code_For_Ast & rhs_stmt = rhs->compile();
	// create a new icslist
	list<Icode_Stmt *> & icslist = *new list<Icode_Stmt*>();	
	// add both the list elements to it
	icslist.splice(icslist.end(), lhs_stmt.get_icode_list());
	icslist.splice(icslist.end(), rhs_stmt.get_icode_list());
	// create a new register for result 
	// declare new register descriptor here
	machine_desc_object.clear_local_register_mappings();
	Register_Descriptor * result_register;

	if (get_data_type() == double_data_type)
	{
		result_register = machine_desc_object.get_new_register<float_reg>();
	}
	else{
		result_register = machine_desc_object.get_new_register<gp_data>();
	}

	// create a new compute stmt with arguments as result of both ast, result as new register
	Ics_Opd * result_opd = new Register_Addr_Opd(result_register);
	Ics_Opd * lhs_opd = new Register_Addr_Opd(lhs_stmt.get_reg());
	Ics_Opd * rhs_opd = new Register_Addr_Opd(rhs_stmt.get_reg());

	Compute_IC_Stmt * add_stmt = new Compute_IC_Stmt(add, result_opd, lhs_opd, rhs_opd);
	// add it to icslist
	icslist.push_back(add_stmt);
	// return the Code_For_Ast with icslist and result register
	Code_For_Ast * cfa = new Code_For_Ast(icslist, result_register );
	return *cfa;	
}

/////////////////////////////////////////////////////////////////

Code_For_Ast & Minus_Ast::compile()
{
	// find the code for ast for lhs
	Code_For_Ast & lhs_stmt = lhs->compile();
	// find the code for ast for rhs
	Code_For_Ast & rhs_stmt = rhs->compile();
	// create a new icslist
	list<Icode_Stmt *> & icslist = *new list<Icode_Stmt*>();	
	// add both the list elements to it
	icslist.splice(icslist.end(), lhs_stmt.get_icode_list());
	icslist.splice(icslist.end(), rhs_stmt.get_icode_list());
	// create a new register for result 
	// declare new register descriptor here
	machine_desc_object.clear_local_register_mappings();
	Register_Descriptor * result_register;

	if (get_data_type() == double_data_type)
	{
		result_register = machine_desc_object.get_new_register<float_reg>();
	}
	else{
		result_register = machine_desc_object.get_new_register<gp_data>();
	}

	// create a new compute stmt with arguments as result of both ast, result as new register
	Ics_Opd * result_opd = new Register_Addr_Opd(result_register);
	Ics_Opd * lhs_opd = new Register_Addr_Opd(lhs_stmt.get_reg());
	Ics_Opd * rhs_opd = new Register_Addr_Opd(rhs_stmt.get_reg());

	Compute_IC_Stmt * sub_stmt = new Compute_IC_Stmt(sub, result_opd, lhs_opd, rhs_opd);
	// Add it to icslist
	icslist.push_back(sub_stmt);
	Code_For_Ast * cfa = new Code_For_Ast(icslist, result_register );
	// return the Code_For_Ast with icslist and result register
	return *cfa;	
}

//////////////////////////////////////////////////////////////////

Code_For_Ast & Mult_Ast::compile()
{
	// find the code for ast for lhs
	Code_For_Ast & lhs_stmt = lhs->compile();
	// find the code for ast for rhs
	Code_For_Ast & rhs_stmt = rhs->compile();
	// create a new icslist
	list<Icode_Stmt *> & icslist = *new list<Icode_Stmt*>();	
	// add both the list elements to it
	icslist.splice(icslist.end(), lhs_stmt.get_icode_list());
	icslist.splice(icslist.end(), rhs_stmt.get_icode_list());
	// create a new register for result 
	// declare new register descriptor here
	machine_desc_object.clear_local_register_mappings();
	Register_Descriptor * result_register;

	if (get_data_type() == double_data_type)
	{
		result_register = machine_desc_object.get_new_register<float_reg>();
	}
	else{
		result_register = machine_desc_object.get_new_register<gp_data>();
	}

	// create a new compute stmt with arguments as result of both ast, result as new register
	Ics_Opd * result_opd = new Register_Addr_Opd(result_register);
	Ics_Opd * lhs_opd = new Register_Addr_Opd(lhs_stmt.get_reg());
	Ics_Opd * rhs_opd = new Register_Addr_Opd(rhs_stmt.get_reg());

	Compute_IC_Stmt * mult_stmt = new Compute_IC_Stmt(mult, result_opd, lhs_opd, rhs_opd);
	// Add it to icslist
	icslist.push_back(mult_stmt);
	Code_For_Ast * cfa = new Code_For_Ast(icslist, result_register );
	// return the Code_For_Ast with icslist and result register
	return *cfa;	
}

////////////////////////////////////////////////////////////////////

Code_For_Ast & Conditional_Operator_Ast::compile()
{
	
}


////////////////////////////////////////////////////////////////////

Code_For_Ast & Divide_Ast::compile()
{
	// find the code for ast for lhs
	Code_For_Ast & lhs_stmt = lhs->compile();
	// find the code for ast for rhs
	Code_For_Ast & rhs_stmt = rhs->compile();
	// create a new icslist
	list<Icode_Stmt *> & icslist = *new list<Icode_Stmt*>();	
	// add both the list elements to it
	icslist.splice(icslist.end(), lhs_stmt.get_icode_list());
	icslist.splice(icslist.end(), rhs_stmt.get_icode_list());
	// create a new register for result 
	// declare new register descriptor here
	machine_desc_object.clear_local_register_mappings();
	Register_Descriptor * result_register;

	if (get_data_type() == double_data_type)
	{
		result_register = machine_desc_object.get_new_register<float_reg>();
	}
	else{
		result_register = machine_desc_object.get_new_register<gp_data>();
	}

	// create a new compute stmt with arguments as result of both ast, result as new register
	Ics_Opd * result_opd = new Register_Addr_Opd(result_register);
	Ics_Opd * lhs_opd = new Register_Addr_Opd(lhs_stmt.get_reg());
	Ics_Opd * rhs_opd = new Register_Addr_Opd(rhs_stmt.get_reg());

	Compute_IC_Stmt * divd_stmt = new Compute_IC_Stmt(divd, result_opd, lhs_opd, rhs_opd);
	// Add it to icslist
	icslist.push_back(divd_stmt);
	Code_For_Ast * cfa = new Code_For_Ast(icslist, result_register );
	// return the Code_For_Ast with icslist and result register
	return *cfa;	
}


//////////////////////////////////////////////////////////////////////

Code_For_Ast & UMinus_Ast::compile()
{
	// find the code for ast for lhs
	Code_For_Ast & lhs_stmt = lhs->compile();
	// rhs is null 
	list<Icode_Stmt *> & icslist = *new list<Icode_Stmt*>();	
	// add both the list elements to it
	icslist.splice(icslist.end(), lhs_stmt.get_icode_list());
	// create a new register for result 
	// declare new register descriptor here
	machine_desc_object.clear_local_register_mappings();
	Register_Descriptor * result_register;

	if (get_data_type() == double_data_type)
	{
		result_register = machine_desc_object.get_new_register<float_reg>();
	}
	else{
		result_register = machine_desc_object.get_new_register<gp_data>();
	}

	// create a new compute stmt with arguments as result of both ast, result as new register
	Ics_Opd * result_opd = new Register_Addr_Opd(result_register);
	Ics_Opd * lhs_opd = new Register_Addr_Opd(lhs_stmt.get_reg());
	// 
	Move_IC_Stmt * uminus_stmt = new Move_IC_Stmt(uminus, lhs_opd, result_opd);
	// Add it to icslist
	icslist.push_back(uminus_stmt);
	Code_For_Ast * cfa = new Code_For_Ast(icslist, result_register );
	// return the Code_For_Ast with icslist and result register
	return *cfa	;
}

//////////////////////////////////////////////////////////////////////////////

Code_For_Ast & Sequence_Ast::compile()
{

	list<Icode_Stmt *> & icslist = *new list<Icode_Stmt*>();	
	for(list<Ast*>::iterator it = statement_list.begin() ; it!=statement_list.end(); it++)
    {
        // print it 
		Code_For_Ast & stmt = (*it)->compile();
        icslist.splice(icslist.end(), stmt.get_icode_list() );
    }
	Code_For_Ast * cfa = new Code_For_Ast(icslist, NULL);
	return *cfa;
}

void Sequence_Ast::print_assembly(ostream & file_buffer)
{
	
}

void Sequence_Ast::print_icode(ostream & file_buffer)
{
	
}

//////////////////////////////////////////////////////////////////////////////

template class Number_Ast<double>;
template class Number_Ast<int>;
