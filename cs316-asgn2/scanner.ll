%filenames="scanner"
%lex-source="scanner.cc"



digit [0-9]
alpha [a-zA-Z]


%%
// ADD REGEX FOR INTEGER_NUMBER
-?{digit}+	{

 	  ParserBase::STYPE__ *val = getSval();
          val->integer_value = atoi(matched().c_str());
		  //cout<<"Found integer number"<<endl;
	  return Parser::INTEGER_NUMBER; 

	}

// ADD REGEX FOR DOUBLE_NUMBER
[-]?{digit}*\.{digit}+([eE][+-]?{digit}+)?  {

 	  ParserBase::STYPE__ *val = getSval();
          val->double_value = atof(matched().c_str());
		  //cout<<"Found double number"<<endl;
	  return Parser::DOUBLE_NUMBER;
}



// ADD REGEX FOR INTEGER
int   {
		return Parser::INTEGER;	
	   }

// ADD REGEX FOR float

float   {
	
		return Parser::FLOAT;	
	   }

// ADD REGEX FOR VOID
void  {
		return Parser::VOID;
	  }

// ADD REGEX FOR ASSIGN
 = {
		return Parser::ASSIGN;
	}

// ADD REGEX FOR NAME
[[:alpha:]_][[:alpha:][:digit:]_]* {

	  ParserBase::STYPE__ *val = getSval();	
	  val->string_value = new std::string(matched());
	  return Parser::NAME; 
	}


\+			{  return matched()[0];}
\-			{  return matched()[0];}
\/			{  return matched()[0];}
\*			{  return matched()[0];}
\( 			{  return matched()[0];}	
\{			{  return matched()[0];}
\}			{  return matched()[0];}
\) 			{  return matched()[0];}
\;			{  return matched()[0];}

\n    		|
";;".*  	|
[ \t]*";;".*	|
[ \t]*"//".*	|
[ \t]		{
			if (command_options.is_show_tokens_selected())
				ignore_token();
		}

.		{ 
			string error_message;
			error_message =  "Illegal character `" + matched();
			error_message += "' on line " + lineNr();
			
			CHECK_INPUT(CONTROL_SHOULD_NOT_REACH, error_message, lineNr());
		}
