%scanner scanner.h
%scanner-token-function d_scanner.lex()
%filenames parser
%parsefun-source parser.cc

%union 
{
	//ADD CODE HERE
	int integer_value;
	double double_value;
	std::string * string_value;
	Procedure * procedure;
	pair<Data_Type, string> * decl;
	Sequence_Ast * sequence_ast;
	Symbol_Table * symbol_table;
	// Symbol_Table_Entry * symbol_entry;

	vector<string> * var_vec;
	vector<pair<Data_Type, string>*> * decl_vec;
	vector<Symbol_Table_Entry *> * symbl_tabl;

	Ast * ast;
	Assignment_Ast * asmt_ast;
	Arithmetic_Expr_Ast * arthmexpr_ast;
	Relational_Expr_Ast * reltnexpr_ast;
	Boolean_Expr_Ast * boolexpr_ast;
	Selection_Statement_Ast * selecstmt_ast;
	Iteration_Statement_Ast * itrnstmt_ast;
	Conditional_Operator_Ast * condn_Ast;
};

//ADD TOKENS HERE
%token <integer_value> INTEGER_NUMBER
%token <string_value> NAME
%token <double_value> DOUBLE_NUMBER;
%token RETURN INTEGER FLOAT
%token ASSIGN VOID
%token IF WHILE DO 

%right SAATH

%left OR
%left AND
// %left '?'
%left EQ NEQ
%left LE LEQ GE GEQ
// %right ELSE

%left '+' '-'
%left '*' '/'
%right UMINUS

%right NOT

%nonassoc '('
%nonassoc ')'

%nonassoc IFX
%nonassoc ELSE


%type <symbol_table> optional_variable_declaration_list
%type <symbol_table> variable_declaration_list
%type <var_vec> var_list
%type <decl_vec> declaration
%type <symbl_tabl> variable_declaration
// %type <decl> declaration
//ADD CODE HERE
%type <sequence_ast> statement_list
%type <ast> assignment_statement
%type <ast> statement
%type <ast> variable
%type <ast> constant
%type <ast> arith_expression
%type <ast> boolean_expr
%type <ast> relational_expr
%type <ast> expression_term



// precedence defined here
// %prec THEN
// %prec ELSE

%start program

%%

program:
	declaration_list procedure_definition
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT((current_procedure != NULL), "Current procedure cannot be null");

		program_object.set_procedure(current_procedure, get_line_number());
		program_object.global_list_in_proc_check();
	}
	}
;

declaration_list:
	procedure_declaration
	{
	if (NOT_ONLY_PARSE)
	{
		Symbol_Table * global_table = new Symbol_Table();
		program_object.set_global_table(*global_table);
	}
	}
|
	variable_declaration_list
	procedure_declaration
	{
	if (NOT_ONLY_PARSE)
	{
		Symbol_Table * global_table = $1;

		CHECK_INVARIANT((global_table != NULL), "Global declarations cannot be null");

		program_object.set_global_table(*global_table);
	}
	}
|
	procedure_declaration
	variable_declaration_list
	{
	if (NOT_ONLY_PARSE)
	{
		Symbol_Table * global_table = $2;

		CHECK_INVARIANT((global_table != NULL), "Global declarations cannot be null");

		program_object.set_global_table(*global_table);
	}
	}
;

procedure_declaration:
	VOID NAME '(' ')' ';'
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($2 != NULL), "Procedure name cannot be null");
		CHECK_INVARIANT((*$2 == "main"), "Procedure name must be main in declaration");
	}
	}
;

procedure_definition:
	NAME '(' ')'
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($1 != NULL), "Procedure name cannot be null");
		CHECK_INVARIANT((*$1 == "main"), "Procedure name must be main");

		string proc_name = *$1;

		current_procedure = new Procedure(void_data_type, proc_name, get_line_number());

		CHECK_INPUT ((program_object.variable_in_symbol_list_check(proc_name) == false),
			"Procedure name cannot be same as global variable", get_line_number());
	}
	}

	'{' optional_variable_declaration_list
	{
	if (NOT_ONLY_PARSE)
	{

		CHECK_INVARIANT((current_procedure != NULL), "Current procedure cannot be null");

		Symbol_Table * local_table = $6;

		if (local_table == NULL)
			local_table = new Symbol_Table();

		current_procedure->set_local_list(*local_table);
	}
	}

	statement_list '}'
	{
	if (NOT_ONLY_PARSE)
	{
		Sequence_Ast* seq = $8;

		CHECK_INVARIANT((current_procedure != NULL), "Current procedure cannot be null");
		CHECK_INVARIANT((seq != NULL), "statement list cannot be null");

		current_procedure->set_sequence_ast(*seq);
	}
	}
;

optional_variable_declaration_list:
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = NULL;
	}
	}
|
	variable_declaration_list
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($1 != NULL), "Declaration statement list cannot be null here");

		$$ = $1;
	}
	}
;

variable_declaration_list:
	variable_declaration
	{
	if (NOT_ONLY_PARSE)
	{
		
		Symbol_Table * decl_list = new Symbol_Table();

		vector<Symbol_Table_Entry *> ste_vec = *$1;


		for(int i = 0; i < (ste_vec).size(); i++)
		{
			Symbol_Table_Entry * decl_stmt = ste_vec[i];

			CHECK_INVARIANT((decl_stmt != NULL), "Non-terminal declaration statement cannot be null");

			string decl_name = decl_stmt->get_variable_name();
			CHECK_INPUT ((program_object.variable_proc_name_check(decl_name) == false),
					"Variable name cannot be same as the procedure name", get_line_number());

			if(current_procedure != NULL)
			{
				CHECK_INPUT((current_procedure->get_proc_name() != decl_name),
					"Variable name cannot be same as procedure name", get_line_number());
			}

			decl_list->push_symbol(decl_stmt);
		}	
		
		$$ = decl_list;
	}
	}
|
	variable_declaration_list variable_declaration
	{
	if (NOT_ONLY_PARSE)
	{
		// if declaration is local then no need to check in global list
		// if declaration is global then this list is global list

		Symbol_Table * decl_list = $1;
		vector<Symbol_Table_Entry *> ste_vec = *$2;
		CHECK_INVARIANT((decl_list != NULL), "The declaration statement list cannot be null");

		for(int i = 0; i < (ste_vec).size() ; i++ )
		{

		Symbol_Table_Entry * decl_stmt = (ste_vec)[i];

		CHECK_INVARIANT((decl_stmt != NULL), "The declaration statement cannot be null");

		string decl_name = decl_stmt->get_variable_name();

		CHECK_INPUT((program_object.variable_proc_name_check(decl_name) == false),
			"Procedure name cannot be same as the variable name", get_line_number());
		if(current_procedure != NULL)
		{
			CHECK_INPUT((current_procedure->get_proc_name() != decl_name),
				"Variable name cannot be same as procedure name", get_line_number());
		}

		CHECK_INPUT((decl_list->variable_in_symbol_list_check(decl_name) == false), 
				"Variable is declared twice", get_line_number());

		decl_list->push_symbol(decl_stmt);

		}


		$$ = decl_list;


	}
	}
;

variable_declaration:
	declaration ';'
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($1 != NULL), "Declaration cannot be null");

		
		vector<Symbol_Table_Entry *> * symbl_tabl = new vector<Symbol_Table_Entry *>();
		// assert(symbl_tabl->size() > 0);
		vector<pair<Data_Type, string>*>  nlist = *$1;

		for (int i = 0; i < nlist.size(); i++)
		{
			Symbol_Table_Entry * decl_entry = new Symbol_Table_Entry(nlist[i]->second, nlist[i]->first, get_line_number());
			
			symbl_tabl->push_back(decl_entry);

		}

		

		$$ = symbl_tabl;


	}
	}
;

declaration:
	INTEGER var_list
	{
	if (NOT_ONLY_PARSE)
	{
		// CHECK_INVARIANT(($2 != NULL), "Name cannot be null");

		Data_Type type = int_data_type;
		vector<string>  nlist = *$2;

		vector<pair<Data_Type, string>*>  *vlist = new vector<pair<Data_Type, string>*>();
		for(int i = 0; i < (nlist).size(); i++)
		{
			pair<Data_Type, string> * declr = new pair<Data_Type, string>(type, nlist[i]);
			vlist->push_back(declr);			
		}

		$$ = vlist;
	}
	}
|
	FLOAT var_list
	{
	if (NOT_ONLY_PARSE)
	{


		Data_Type type = double_data_type;
		vector<string>  nlist = *$2;

		vector<pair<Data_Type, string>*> * vlist = new vector<pair<Data_Type, string>*>();
		for(int i = 0; i < nlist.size(); i++)
		{
			pair<Data_Type, string> * declr = new pair<Data_Type, string>(type, nlist[i]);
			vlist->push_back(declr);			
		}

		$$ = vlist;
	}
	}
;

var_list:
	NAME
	{
	if(NOT_ONLY_PARSE)
	{
		vector<string> * vlist = new vector<string>();

		vlist->push_back(*$1);
		$$ = vlist;	
	}
	}

	|

	var_list ',' NAME
	{
	if(NOT_ONLY_PARSE)
	{

		vector<string> * vlist = $1;

		vlist->push_back(*$3);
		$$ = vlist;
	}
	}


;

statement_list:
	{
	if (NOT_ONLY_PARSE)
	{
		//ADD CODE HERE

		Sequence_Ast * stmt_list = new Sequence_Ast(get_line_number());
		
		$$ = stmt_list;
	}
	}
|
	statement_list statement
	{
	if (NOT_ONLY_PARSE)
	{

		Sequence_Ast * stmt_list = $1;
		Ast * stmt = $2;
		// check if  both are not null
		//CHECK_INVARIANT((stmt != NULL), "The assignment statement cannot be null");

		// add statement to statement_list
		stmt_list->ast_push_back(stmt);
		$$ = stmt_list;
		// cout<<"after checkast"<<endl;

	}
	}
;

// Make sure to call check_ast in assignment_statement and arith_expression
// Refer to error_display.hh for displaying semantic errors if any

statement:
	
	assignment_statement
	{
	if (NOT_ONLY_PARSE)
	{
		// CHECK_INVARIANT(($1 != NULL), "Name cannot be null");
		$$ = $1;
	}
	}
	 
	|

	'{' statement_list '}'
	{
	if (NOT_ONLY_PARSE)
	{
		($$) = $2;
	}
	}

	|


	WHILE '(' boolean_expr ')' statement
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * cond = $3;
		Ast * body = $5;
		
		Ast * loopstmt = new Iteration_Statement_Ast(cond, body,get_line_number(), false);
		$$ = loopstmt;
		($$)->check_ast();
	}
	}

	|

	IF '(' boolean_expr ')'  statement  %prec IFX
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * cond = $3;
		Ast * then = $5;
		// ask why
		Ast * sequence_ast = new Sequence_Ast(get_line_number());

		Ast * selectnstmt = new Selection_Statement_Ast($3, $5, sequence_ast, get_line_number());

		$$ = selectnstmt;
		($$)->check_ast();
	}
	}

	|

	IF '(' boolean_expr ')'  statement ELSE statement
	{
	if (NOT_ONLY_PARSE)
	{

		Ast * cond = $3;
		Ast * then = $5;
		Ast * els =  $7;

		Ast * selectnstmt = new Selection_Statement_Ast(cond, then, els, get_line_number());

		$$ = selectnstmt;
		($$)->check_ast();;		
		
	}
	}

	|

	DO statement WHILE '(' boolean_expr ')' ';'
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * cond = $5;
		Ast * body = $2;

		Ast * dowhile = new Iteration_Statement_Ast(cond, body, get_line_number(), true);

		$$ = dowhile;
		($$)->check_ast();;
	}
	}

	;





// Make sure to call check_ast in assignment_statement and arith_expression
// Refer to error_display.hh for displaying semantic errors if any
assignment_statement:
	variable ASSIGN arith_expression ';'
	{
	if (NOT_ONLY_PARSE)
	{
		//ADD CODE HERE
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");

		Ast * left = $1;
		Ast * right = $3;
		
		Assignment_Ast * assgn_stmt = new Assignment_Ast(left, right, get_line_number());
		// make a joint ast

		// call check funcctiosns here
		// CHECK_INPUT()

		$$ = assgn_stmt;
		//CHECK_INPUT(($$)->check_ast(), "invalid assignment exp", get_line_number());
		($$)->check_ast();
	}
	}




;

arith_expression:
		//ADD RELEVANT CODE ALONG WITH GRAMMAR RULES HERE
                // SUPPORT binary +, -, *, / operations, unary -, and allow parenthesization
                // i.e. E -> (E)
                // Connect the rules with the remaining rules given below
	arith_expression '+' arith_expression
	{
	if (NOT_ONLY_PARSE)
	{
		// add code for add here
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");
		Ast * left = $1;
		Ast * right = $3;

		Arithmetic_Expr_Ast * plus_stmt = new Plus_Ast(left, right, get_line_number());

		$$ = plus_stmt;
		($$)->check_ast();
	}

	}
|
	arith_expression '-' arith_expression
	{
	if (NOT_ONLY_PARSE)
	{
			// add code for add here
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");
		Ast * left = $1;
		Ast * right = $3;

		Arithmetic_Expr_Ast * minus_stmt = new Minus_Ast(left, right, get_line_number());

		$$ = minus_stmt;
		//cout<<""	
		($$)->check_ast();
	}

	}
|
	arith_expression '*' arith_expression
	{
	if (NOT_ONLY_PARSE)
	{
				// add code for add here
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");
		Ast * left = $1;
		Ast * right = $3;

		Arithmetic_Expr_Ast * mult_stmt = new Mult_Ast(left, right, get_line_number());

		$$ = mult_stmt;
		($$)->check_ast();
	}

	}
|
	arith_expression '/' arith_expression
	{
	if (NOT_ONLY_PARSE)
	{
				// add code for add here
		CHECK_INVARIANT((($1 != NULL) && ($3 != NULL)), "lhs/rhs cannot be null");
		Ast * left = $1;
		Ast * right = $3;

		Arithmetic_Expr_Ast * divide_stmt = new Divide_Ast(left, right, get_line_number());

		$$ = divide_stmt;
		($$)->check_ast();
	}

	}
|
	'(' arith_expression ')'
	{
	if (NOT_ONLY_PARSE)
	{

		$$ = $2;
	}

	}
|
	'-' arith_expression %prec UMINUS
	{
	if (NOT_ONLY_PARSE)
	{
		CHECK_INVARIANT(($2 != NULL), "lhs/rhs cannot be null");
		UMinus_Ast * uminus_stmt = new UMinus_Ast($2, NULL, get_line_number());

		$$ = uminus_stmt;
		($$)->check_ast();
	}

	}
|
	expression_term
	{
	if (NOT_ONLY_PARSE)
	{
		$$ =  $1;
		//HECK_INPUT(($$)->check_ast(), "invalid expr term exp", get_line_number());
	}

	}

|

	 boolean_expr '?' arith_expression ':' arith_expression  %prec SAATH
	{
	if (NOT_ONLY_PARSE)
	{

		Ast * condn_ast = new Conditional_Operator_Ast($1, $3, $5, get_line_number());

		$$ = condn_ast;
		($$)->check_ast();

	}
	}

;


relational_expr:

	arith_expression LE arith_expression 
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * reltn_expr = new Relational_Expr_Ast($1, less_than, $3, get_line_number());
		$$ = reltn_expr;
		($$)->check_ast();;
	}
	}

	|

	arith_expression GE arith_expression 
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * reltn_expr = new Relational_Expr_Ast($1, greater_than, $3, get_line_number());
		$$ = reltn_expr;
		($$)->check_ast();;
	}
	}

	|

	arith_expression LEQ arith_expression 
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * reltn_expr = new Relational_Expr_Ast($1, less_equalto, $3, get_line_number());
		$$ = reltn_expr;
		($$)->check_ast();;
	}
	}

	|

	arith_expression GEQ arith_expression 
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * reltn_expr = new Relational_Expr_Ast($1, greater_equalto, $3, get_line_number());
		$$ = reltn_expr;
		($$)->check_ast();;
	}
	}

	|

	arith_expression EQ arith_expression 
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * reltn_expr = new Relational_Expr_Ast($1, equalto, $3, get_line_number());
		$$ = reltn_expr;
		($$)->check_ast();;
	}
	}

	|

	arith_expression NEQ arith_expression 
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * reltn_expr = new Relational_Expr_Ast($1, not_equalto, $3, get_line_number());
		$$ = reltn_expr;
		($$)->check_ast();;
	}
	}
;


boolean_expr:
	boolean_expr OR boolean_expr
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * boolexpr = new Boolean_Expr_Ast($1, boolean_or, $3, get_line_number());
		$$ = boolexpr;

		($$)->check_ast();;
	}
	}	

|
	boolean_expr AND boolean_expr
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * boolexpr = new Boolean_Expr_Ast($1, boolean_and, $3, get_line_number());
		$$ = boolexpr;

		($$)->check_ast();;
	}
	}	

|
	NOT boolean_expr 
	{
	if (NOT_ONLY_PARSE)
	{
		Ast * boolexpr = new Boolean_Expr_Ast(NULL, boolean_not, $2, get_line_number());
		$$ = boolexpr;

		($$)->check_ast();;
	}
	}	

|
	'(' boolean_expr ')'
	{
	if (NOT_ONLY_PARSE)
	{
		$$ = $2;
	}
	}	
	
|
	relational_expr
	{
	if (NOT_ONLY_PARSE)
	{

		$$ = $1;
	}
	}	

;




expression_term:
	variable
	{
	if (NOT_ONLY_PARSE)
	{
		//ADD CODE HERE
		$$ = $1;
	}
	}
|
	constant
	{
	if (NOT_ONLY_PARSE)
	{
		//ADD CODE HERE
		$$ = $1;
	}
	}
;

variable:
	NAME
	{
	if (NOT_ONLY_PARSE)
	{
		Symbol_Table_Entry * var_table_entry;

		CHECK_INVARIANT(($1 != NULL), "Variable name cannot be null");

		if (current_procedure->variable_in_symbol_list_check(*$1))
			 var_table_entry = &(current_procedure->get_symbol_table_entry(*$1));

		else if (program_object.variable_in_symbol_list_check(*$1))
			var_table_entry = &(program_object.get_symbol_table_entry(*$1));

		else
			CHECK_INPUT_AND_ABORT(CONTROL_SHOULD_NOT_REACH, "Variable has not been declared", get_line_number());

		$$ = new Name_Ast(*$1, *var_table_entry, get_line_number());

		delete $1;
	}
	}
;

constant:
	INTEGER_NUMBER
	{
	if (NOT_ONLY_PARSE)
	{
		//ADD CODE HERE
		Ast * num_ast = new Number_Ast<int>($1, int_data_type, get_line_number());

		$$ = num_ast;
	}
	}
|
	DOUBLE_NUMBER
	{
	if (NOT_ONLY_PARSE)
	{
		//ADD CODE HERE
		Ast * num_ast = new Number_Ast<double>($1, double_data_type, get_line_number());

		$$ = num_ast;
	}
	}
;