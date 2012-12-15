//============================================================================
//----------------------------------------------------------------------------
//								ScParser.cpp
//----------------------------------------------------------------------------
//============================================================================

// All functions having to do with the parser for the scenario objects.

// Enjoy.

// Some of this stuff is actually code from the game, and is commented out.
// Might make interesting reading.

//#include <Carbon/Carbon.h>	// included in pre-compiled header
//#include "string.h"
//#include "stdio.h"
//#include "stdlib.h"
//#include <ctype.h>
#include "global.h"
#include "tokntype.h"


// stacks for evaluating numerical expressions
#define MAX_NUMERICAL_EXPR_LENGTH	100


extern scenario_data_type scenario;
extern scen_item_data_type scen_data;

extern short start_volume;
extern long start_dir;

extern FSSpec current_scenario_file_info;


// SCRIPT STORAGE
// HERE, ALL CREATURE, SCENARIO, OBJECT SCRIPTS ARE STORED
//script_type *creature_scripts[NUM_CHARS];
//script_type *object_scripts[NUM_OBJ];
//script_type *zone_script = NULL;
//script_type *zone_dialogue_script = NULL;


// function prototype
void patch_corescendata2( void );


void init_scripts()
{
	//for (short i = 0; i < NUM_CHARS; i++)
	//	creature_scripts[i] = NULL;
	//for (short i = 0; i < NUM_OBJ; i++)
	//	object_scripts[i] = NULL;
}

	
// METHODS FOR text_block_type

text_block_type::text_block_type()
{
	text_block = NULL;
	block_length = 0;
}

text_block_type::~text_block_type()
{
	if (text_block != NULL) {
		DisposePtr(text_block);
		}
}

// Goes to filename, assumed to be a text file, and loads said text file into text string
// buffer. 
// file_location: 0 - in main data folder, 1 - in scenario folder
Boolean text_block_type::load_text_file_into_buffer(char *file_name_to_load,short file_location)
{
	assert(file_location==0 || file_location==1);
	Str255 file_name;
	long file_length = 0;
	OSErr error = noErr;
	
	sprintf((char *) file_name,"%s",file_name_to_load);

	c2p(file_name);

	short file_id;
	
	// first, figure out length of file
	switch (file_location) {
		case 0: error = HOpen(start_volume,start_dir,
		  file_name,1,&file_id); break;
		case 1: error = HOpen(current_scenario_file_info.vRefNum,current_scenario_file_info.parID,
		  file_name,1,&file_id); break;
		}
	if (error != noErr) {return FALSE;}
	
	GetEOF(file_id,&file_length);
	
	if (file_length == 0) {
		FSClose(file_id);
		return TRUE;
		}
		
	error = FSClose(file_id);
	if (error != noErr) {return FALSE;}
	
	text_block = NewPtr(file_length + 25);
	
	// now, load in text file
	switch (file_location) {
		case 0: error = HOpen(start_volume,start_dir,
		  file_name,1,&file_id); break;
		case 1: error = HOpen(current_scenario_file_info.vRefNum,current_scenario_file_info.parID,
		  file_name,1,&file_id); break;
		}
	if (error != noErr) {return FALSE;}

	if ((error = FSRead(file_id, &file_length, (char *) text_block)) != 0)
		{return FALSE;}
	
	error = FSClose(file_id);
	if (error != noErr) {return FALSE;}
	
	text_block[file_length] = 0;
	block_length = file_length;
	return TRUE;
}

// Goes through text and eliminates all commented text and makes all text not in quotes
// lowercase
void text_block_type::preprocess_text()
{
	if (block_length <= 0)
		return;
		
	Boolean in_comments = FALSE;
	for (long i = 0; i < block_length; i++) 
		if (text_block[i] == 10)
			text_block[i] = 13;
	for (long i = 0; i < block_length; i++) {
		if ((i < block_length - 1) && (text_block[i] == '/') && (text_block[i + 1] == '/'))
			in_comments = TRUE;
			else if (text_block[i] == 13)
				in_comments = FALSE;
		if (in_comments == TRUE)
			text_block[i] = ' ';
		}
	Boolean in_quotes = FALSE;
	for (long i = 0; i < block_length; i++) {
		if (text_block[i] == '"')
			in_quotes = !(in_quotes);
		if ((in_quotes == FALSE) && (text_block[i] >= 65) && (text_block[i] <= 90))
			text_block[i] = text_block[i] - 'A' + 'a';
		}
}


// Gives an UPPER bound for the number of tokens in the script. It MUST
// be >= the number of actual tokens
short text_block_type::estimate_num_of_tokens()
{
	if (block_length <= 0)
		return 0;
		
	long num_tokens = 0;
	long current_mode = -1; // 0 - space, 1 - letter, 2 - number, 3 - other
	Boolean space;
	
	//char dbug[5];
	
	for (long i = 0; i < block_length; i++) {
		long old_mode = current_mode;
		space = FALSE;
		
		if ((text_block[i] == ' ') || (text_block[i] == '\r') || (text_block[i] == '\t')) {
			current_mode = 0;
			space = TRUE;
		}
		else if ((text_block[i] >= '0') && (text_block[i] <= '9'))
			current_mode = 2;
		else if ((text_block[i] >= 'a') && (text_block[i] <= 'z'))
			current_mode = 1;
		else if ((text_block[i] >= 'A') && (text_block[i] <= 'Z'))
			current_mode = 1;
		else if (text_block[i] == '_')
			current_mode = 1;
		else 
			current_mode = 3;
		if ((space == FALSE) && ((old_mode != current_mode) || (current_mode == 3))) {
			//dbug[0] = text_block[i];
			//dbug[1] = text_block[i + 1];
			//dbug[2] = text_block[i + 2];
			//dbug[3] = text_block[i + 3];
			//dbug[4] = 0;
			num_tokens++;
		}
	}
	return num_tokens + 20;
}

token_type::token_type()
{
	type = 0;
	line = 0;
	what_sort = 0;
}

script_type::script_type()
{
	script_name[0] = 0;
	type_of_script = -1;
	token_list = NULL;
	num_tokens = 0;
	script_killed = FALSE;
	which_object = 0;
	
	for (short i = 0; i < NUM_SCRIPT_STRINGS; i++)
		string_data[i] = NULL;

	for (short i = 0; i < NUM_SCRIPT_INTS; i++) {
		int_var_names[i][0] = 0;
		int_var_values[i] = 0;
	}
	for (short i = 0; i < NUM_SCRIPT_LOCATIONS; i++) 
		location_var_names[i][0] = 0;
	for (short i = 0; i < NUM_SCRIPT_STRING_VARS; i++) {
		string_var_names[i][0] = 0;
		string_var_values[i] = NULL;
	}

	for (short i = 0; i < NUM_PROCEDURE_PASS_VARS; i++) {
		procedure_passed_variable_types[i] = 0;
		procedure_passed_values[i] = 0;
		procedure_passed_locations[i].x = procedure_passed_locations[i].y = 0;
	}
}
script_type::~script_type()
{

	flush_data();
	
}

void script_type::flush_data () 
{
	for (short i = 0; i < NUM_SCRIPT_STRINGS; i++){
		if (string_data[i] != NULL) {
			delete[] string_data[i];
			string_data[i] = NULL;
		}
	}
	if (token_list != NULL)
		delete[] token_list;
	token_list = NULL;
	for (short i = 0; i < NUM_SCRIPT_STRING_VARS; i++){
		if (string_var_values[i] != NULL) {
			delete[] string_var_values[i];
			string_var_values[i] = NULL;
			string_var_names[i][0] = 0;
		}
	}
	for (short i = 0; i < NUM_PROCEDURE_PASS_VARS; i++) {
		procedure_passed_variable_types[i] = 0;
		procedure_passed_values[i] = 0;
		procedure_passed_locations[i].x = procedure_passed_locations[i].y = 0;
	}
	script_killed = FALSE;
}

Boolean script_type::IsWhiteSpace (char c) 
{ 
	return c == ' ' || c == '\t' || c == '\n' || c == '\r'; 
}

Boolean script_type::IsIdentifier (char c)
{ 
	return isalpha(c) || c == '_'; 
}
Boolean script_type::IsNumber (char c)
{ 
	return isdigit(c); 
	//return isdigit(c) || c == '.'; 
}

Boolean script_type::IsOperatorCharacter (char c) 
{ 
	return c == '+' || c == '-' || c == '/' || c == '*' || c == '=' || c == '|' || c == '&' || c == '>' || c == '<' || c == '!'; 
}

short script_type::IsVariable (char * string, int length, token_type *token)
{
	for (short i = 0; i < NUM_SCRIPT_INTS; i++)
		if (((int)strlen(int_var_names[i])  == length) && 
		  (strncmp(string,int_var_names[i],length) == 0)) {
		  	token->type = INT_VARIABLE_TYPE;
		  	token->what_sort = i;
		  	return i;
		  	}
	for (short i = 0; i < NUM_SCRIPT_LOCATIONS; i++)
		if (((int)strlen(location_var_names[i]) == length) && 
		  (strncmp(string,location_var_names[i],length) == 0)) {
		  	token->type = LOCATION_VARIABLE_TYPE;
		  	token->what_sort = i;
		  	return i;
		  	}
	for (short i = 0; i < NUM_SCRIPT_STRING_VARS; i++)
		if (((int)strlen(string_var_names[i]) == length) && 
		  (strncmp(string,string_var_names[i],length) == 0)) {
		  	token->type = STRING_VARIABLE_TYPE;
		  	token->what_sort = i;
		  	return i;
		  	}

    return -1;
}

//---------------------------------------------------------------------------
short script_type::IsBlockDefiner (char *string, int length, short *value)
{

	for (short i = 0; i < NUM_BLOCK_DEFINERS; i++){
		if (((int)strlen(block_definers[i].token_text) == length) && 
		  (strncmp(string,block_definers[i].token_text,length) == 0)) {
		  	*value = i;
		  	return i;
		}
	}  	
    return -1;
}

short script_type::IsVarDefiner (char * string, int length, short *value)
{
	/// This is only in Scenario data files
	if (type_of_script != 0)
		return -1;
	
	for (short i = 0; i < NUM_VAR_DEFINERS; i++){
		if (((int)strlen(variable_definers[i].token_text) == length) && 
		  (strncmp(string,variable_definers[i].token_text,length) == 0)) {
		  	*value = i;
		  	return i;
		}
	}
    return -1;
}

short script_type::IsVarArrayDefiner (char * string, int length, short *value)
{
	/// This is only in Scenario data files
	if (type_of_script != 0)
		return -1;

	for (short i = 0; i < NUM_ARRAY_VAR_DEFINERS; i++){
		if (((int)strlen(variable_array_definers[i].token_text) == length) && 
		  (strncmp(string,variable_array_definers[i].token_text,length) == 0)) {
		  	*value = i;
		  	return i;
		}
	}  	
    return -1;
}

short script_type::IsVarStringDefiner (char * string, int length, short *value)
{
	/// This is only in Scenario data files
	if (type_of_script != 0)
		return -1;

	for (short i = 0; i < NUM_STRING_VAR_DEFINERS; i++){
		if (((int)strlen(variable_string_definers[i].token_text)  == length) && 
		  (strncmp(string,variable_string_definers[i].token_text,length) == 0)) {
		  	*value = i;
		  	return i;
		}
	}  	
    return -1;
}

short script_type::IsNewVariableDefiner (char * string, int length, short *value)
{

	for (short i = 0; i < NUM_VAR_TYPE_DEFINERS; i++){
		if (((int)strlen(new_variable_definers[i].token_text) == length) && 
		  (strncmp(string,new_variable_definers[i].token_text,length) == 0)) {
		  	*value = i;
		  	return i;
		}
	}  	
    return -1;
}

short script_type::IsConstant (char * string, int length, short *value)
{
	for (short i = 0; i < NUM_CONST_TYPE_DEFINERS; i++){
		if (((int)strlen(constant_definers[i].token_text) == length) && 
		  (strncmp(string,constant_definers[i].token_text,length) == 0)) {
		  	*value = i;
		  	return i;
		}
	}
    return -1;
}


short script_type::IsOperator (char * string, int length, short *value)
{
	for (short i = 0; i < NUM_OPERATORS; i++){
		if (((int)strlen(operator_definers[i].token_text) == length) && 
		  (strncmp(string,operator_definers[i].token_text,length) == 0)) { 
		  	*value = i;
		  	return i;
		}
	} 
    return -1;
}

short script_type::IsNoParameterFunction (char * string, int length, short *value)
{
	for (short i = 0; i < NUM_NO_PARAM_FUNCTIONS; i++){
		if (((int)strlen(no_parameter_function_definers[i].token_text) == length) && 
		  (strncmp(string,no_parameter_function_definers[i].token_text,length) == 0)) {
		  	*value = i;
		  	return i;
		}
	}
    return -1;
}


short script_type::IsFunction (char * string, int length, short *value)
{
	/// This is never in Scenario data files
	if (type_of_script == 0)
		return -1;
		
	for (short i = 0; i < NUM_UNARY_FUNCTIONS; i++){
		if (((int)strlen(unary_function_definers[i].token_text) == length) && 
		  (strncmp(string,unary_function_definers[i].token_text,length) == 0)) { 
		  	*value = i;
		  	return i;
		}
	}
    return -1;
}


short script_type::IsProcedure (char * string, int length, short *value)
{
	/// This is never in Scenario data files
	if (type_of_script == 0)
		return -1;

	for (short i = 0; i < NUM_PROCEDURES; i++){
		if ((strlen(procedure_definers[i].token_text) > 0) && 
		  (strncmp(string,procedure_definers[i].token_text,length) == 0) &&
		  ((int)strlen(procedure_definers[i].token_text) == length)) {
		  	*value = i;
		  	return i;
		}
	} 
    return -1;
}

short script_type::IsFlowController (char * string, int length, short *value)
{
	for (short i = 0; i < NUM_FLOW_CONTROLLERS; i++){
		if (((int)strlen(flow_controller_definers[i].token_text) == length) && 
		  (strncmp(string,flow_controller_definers[i].token_text,length) == 0)) {
		  	*value = i;
		  	return i;
		}
	} 
    return -1;
}

short script_type::IsBinaryFunction (char * string, int length, short *value)
{
	/// This is never in Scenario data files
	if (type_of_script == 0)
		return -1;

	for (short i = 0; i < NUM_BINARY_FUNCTIONS; i++){
		if (((int)strlen(binary_function_definers[i].token_text) == length) && 
		  (strncmp(string,binary_function_definers[i].token_text,length) == 0)) {
		  	*value = i;
		  	return i;
		}
	} 
    return -1;
}

short script_type::IsTrinaryFunction (char * string, int length, short *value)
{
	/// This is never in Scenario data files
	if (type_of_script == 0)
		return -1;

	for (short i = 0; i < NUM_TRINARY_FUNCTIONS; i++){
		if (((int)strlen(trinary_function_definers[i].token_text)  == length) && 
		 (strncmp(string,trinary_function_definers[i].token_text,length) == 0)) {
		  	*value = i;
		  	return i;
		}
	} 
    return -1;
}
short script_type::IsLocationFunction (char * string, int length, short *value)
{
	/// This is never in Scenario data files
	if (type_of_script == 0)
		return -1;

	for (short i = 0; i < NUM_LOCATION_FUNCTIONS; i++){
		if (((int)strlen(location_returning_function_definers[i].token_text) == length) && 
		  (strncmp(string,location_returning_function_definers[i].token_text,length) == 0)) {
		  	*value = i;
		  	return i;
		}
	} 
    return -1;
}
// Loads in the text of the script. Tokenizes it. Sets up all variables and initializes them. Gets rid of loaded text.
// type_of_script_to_load:
// 0 - Scenario data
// 1 - Scenario data file
// 2 - Town - specials
// 3 - Town - dialogue
// 4 - Outdoor - specials
// 5 - Outdoor - dialogue
// 6 - Creature
// 7 - Terrain Spot		
// file_location: 0 - in main data folder, 1 - in scenario folder
Boolean script_type::load_script(short type_of_script_to_load,const char *script_to_load,short file_location)
{
	// first, flush out old crap.
	type_of_script = -1;
	flush_data();

	num_tokens = 0;
	
	// Check script name
	if ((strlen(script_to_load) <= 0) || (strlen(script_to_load) >= 100)){
		ASB_big("Script name empty or too long.","","","",-1,"");
		return FALSE;
	}
	//strcpy(script_name,script_to_load);
	sprintf(script_name,"%s.txt",script_to_load);	
	
	// load in text
	text_block_type *script = new text_block_type;
	
	if (script == NULL) {
		ASB("Failed to allocate script memory.");
		return FALSE;
	}
		
	if (script->load_text_file_into_buffer(script_name,file_location) == FALSE) {
		ASB("Failed to load script text.");
		delete script;	
		script = NULL;
		return FALSE;
	}
		
	script->preprocess_text();
	
	// estimate tokens and make token list
	num_tokens = script->estimate_num_of_tokens() + 2;
	if (num_tokens < 0) {
		ASB_big("The script ",script_name," is too long.","",-1,"");
		return FALSE;
	}
	token_list = new token_type[num_tokens];
	if (token_list == NULL) {
		ASB("Failed to allocate memory for token list.");
		return FALSE;
	}

	// Everything worked out. So far.
	type_of_script = type_of_script_to_load;
	
	// tokenize script
	token_type token;
	char *s = script->text_block;
	short cur_line = 1,cur_list_position = 0;
	Boolean defining_new_variables = FALSE;
	short defining_new_variable_type = 0;
	short current_variable_created = -1;
	
	
	for (char *f = s; *f; f = s){
    	int length;


		if ((*s == 13) || (*s == 10))
			cur_line++;
			
    	// get token from input string
        if ( IsWhiteSpace(*s) ){
        	s++;
            continue;
        }

		if ( *s == '(' ) {
        	token.type = 18;
        	s++;
        }
        else if ( *s == ')' ) {
        	token.type = 19;
            s++;
        }
        //else if ( *s == '=' ) {
        //	token.type = 40;
        //    s++;
        //}
        else if ( *s == ';' ) {
        	token.type = 41;
            s++;
            defining_new_variables = FALSE;
        }
        else if ( *s == '{' )  {
        	token.type = 42;
            s++;
        }
        else if ( *s == '}' ) {
        	token.type = 43;
            s++;
        }
        else if ( *s == ',' ) {
        	token.type = 20;
        	if (defining_new_variables) 
        		current_variable_created = -1;
            s++;
        }
		else if ( IsIdentifier(*s) ){
//        	short length;
        	
        	for (length = 0; IsIdentifier(*s) || isdigit(*s) ; length++)
            	s++;
            	
			if (length >= MAX_DESCRIBER_LEN) {
				ASB_big(script_to_load," script failure: Token longer than max length in line ","","",cur_line,".");
				delete[] token_list;
				token_list = NULL;
				return FALSE;
				}
         
            if ( IsBlockDefiner(f,length,&token.what_sort) >= 0) {
            	token.type = 1;
			}
            else if ( IsVarDefiner(f,length,&token.what_sort) >= 0) {
            	token.type = 44;
            }
            else if ( IsVarArrayDefiner(f,length,&token.what_sort) >= 0) {
            	token.type = 45;
            }
            else if ( IsVarStringDefiner(f,length,&token.what_sort) >= 0) {
            	token.type = 46;
            }
            else if ( IsNewVariableDefiner(f,length,&token.what_sort) >= 0) {
            	token.type = 48;
            	defining_new_variables = TRUE;
            	defining_new_variable_type = token.what_sort;
            	current_variable_created = -1;
            }
            else if ( IsVariable(f,length,&token)  >= 0) {
            	//token.type = ttVariable;
            }
            else if ( IsConstant(f,length,&token.what_sort)  >= 0) {
				token.type = 17;
				token.what_sort = constant_values[token.what_sort];
            }
            else if ( IsProcedure(f,length,&token.what_sort)  >= 0)
            {
            	token.type = PROCEDURE_TYPE;
            }
			else if ( IsNoParameterFunction(f,length,&token.what_sort)  >= 0)
            {
            	token.type = NO_PARAM_FUNCTION_TYPE;
            }
			else if ( IsFunction(f,length,&token.what_sort)  >= 0)
            {
            	token.type = UNARY_FUNCTION_TYPE;
            }
            else if ( IsFlowController(f,length,&token.what_sort)  >= 0)
            {
            	token.type = FLOW_CONTROLLER_TYPE;
            }
            else if ( IsBinaryFunction(f,length,&token.what_sort)  >= 0)
            {
            	token.type = BINARY_FUNCTION_TYPE;
            }
            else if ( IsTrinaryFunction(f,length,&token.what_sort)  >= 0)
            {
            	token.type = TRINARY_FUNCTION_TYPE;
            }
            else if ( IsLocationFunction(f,length,&token.what_sort)  >= 0)
            {
            	token.type = LOCATION_FUNCTION_TYPE;
            }
            else {
            	// We don't recognize the string. So. Either it's maybe a new variable type 
            	// or it's crap.
            	if (defining_new_variables) { // maybe a new variable
            		short i;
            		Boolean no_room_in_list = FALSE;
            		
            		if (length >= SCRIPT_VAR_NAME_LEN - 1) {
						ASB_big(script_to_load," script failure: Variable name too long in line ","","",cur_line,".");
						delete[] token_list;
		                token_list = NULL;
						return FALSE;
					}
            		
            		switch (defining_new_variable_type) {
            			case 0: case 1: // int
            				for (i = 0; i < NUM_SCRIPT_INTS; i++){
								if (strlen(int_var_names[i]) == 0) {
 		            			    strncpy(int_var_names[i],f,length);
		         			    	int_var_names[i][length] = 0;
		         			    	current_variable_created = i;           						
           						 	token.type = INT_VARIABLE_TYPE;
            						token.what_sort = i;
            						i = NUM_SCRIPT_INTS + 10;
								}
							}
            				if (i < NUM_SCRIPT_INTS + 10)
            					no_room_in_list = TRUE;	
            				break;
            			case 2: // string
            				for (i = 0; i < NUM_SCRIPT_STRING_VARS; i++){
								if (strlen(string_var_names[i]) == 0) {
 		            			    strncpy(string_var_names[i],f,length);
		         			    	string_var_names[i][length] = 0;
		         			    	current_variable_created = i;           						
           						 	token.type = STRING_VARIABLE_TYPE;
            						token.what_sort = i;
            						i = NUM_SCRIPT_STRING_VARS + 10;
								}
							}
            				if (i < NUM_SCRIPT_STRING_VARS + 10)
            					no_room_in_list = TRUE;	
            				break;
            			case 3: // location
            				for (i = 0; i < NUM_SCRIPT_LOCATIONS; i++){
								if (strlen(location_var_names[i]) == 0) {
 		            			    strncpy(location_var_names[i],f,length);
		         			    	location_var_names[i][length] = 0;
		         			    	current_variable_created = i;           						
           						 	token.type = LOCATION_VARIABLE_TYPE;
            						token.what_sort = i;
	           						i = NUM_SCRIPT_LOCATIONS + 10;
								}
							}
            				if (i < NUM_SCRIPT_LOCATIONS + 10)
            					no_room_in_list = TRUE;	
            				break;
					}
            			
					if (no_room_in_list) {
						ASB_big(script_to_load," script failure: Too many variables defined in line ","","",cur_line,".");
						delete[] token_list;
						token_list = NULL;
						return FALSE;
					}
				}
				else { // it's junk. error out
					char* save = new char[length+1];
					strncpy(save,f,length);
					save[length] = 0;
					ASB_big(script_to_load," script failure: Invalid identifier ",save," in line ",cur_line,".");
					delete[] save;
					delete[] token_list;
					token_list = NULL;
					return FALSE;
				}
            }
        }

        else if ((IsNumber(*s)) || ((*s == '-') && (IsNumber(*(s + 1))))) {
			char number_string[100]; // used for loding numbers into scripts
			Boolean minus_present = FALSE;
        	if (*s == '-') {
        		minus_present = TRUE;
        		s++;
			}
        	for (length = 0; IsNumber(*s); length++)
                s++;

            //char* number = new char[length+1];
        	if (minus_present == TRUE)
	            strncpy(number_string,f + 1,length);
			else strncpy(number_string,f,length);
            number_string[length] = 0;
            token.type = 17;
            token.what_sort = (short) strtol(number_string,NULL,0);
            if (minus_present == TRUE)
            	token.what_sort = token.what_sort * -1;
            //delete[] number;
        }
        else if ( IsOperatorCharacter(*s) ){
			for (length = 0; IsOperatorCharacter(*s); length++)
				s++;        
			if (length > 2) {
				ASB_big(script_to_load," script failure: Overlong operator in line ","","",cur_line,".");
				delete[] token_list;
                token_list = NULL;
				return FALSE;
				}
			if ( IsOperator(f,length,&token.what_sort) >= 0) {
				token.type = BINARY_OPERATOR_TYPE;
			}
			else if ((length == 1) && (*f == '=')) {
				token.type = EQUALS_TYPE;
			}
			else {
				ASB_big(script_to_load," script failure: Invalid operator ",""," in line ",cur_line,".");
				delete[] token_list;
				token_list = NULL;
				return FALSE;
			}

        }
    	else if ( *s == '"' ) { // create string data
            s++;
      	 	for (length = 0; (*s != '"') && (*s != 0) && (length < 255); length++)
                s++;
            if ((length >= 255) || (*s == 0)) {
				ASB_big(script_to_load," script failure: Overlong or unending string in line ","","",cur_line,".");
				delete[] token_list;
                token_list = NULL;
				return FALSE;
			}

			short i = 0;
			
			// put string in either string storage (constants) or variables
			if ((defining_new_variables) && (defining_new_variable_type == 2) && (current_variable_created >= 0)) {
				if (string_var_values[current_variable_created] == NULL) {
					string_var_values[current_variable_created] = new char[length + 1];
					strncpy(string_var_values[current_variable_created],f + 1,length);
					string_var_values[current_variable_created][length] = 0;
					if (length >= 255)
						string_var_values[current_variable_created][255] = 0;
						
				}
			} 
			else {
				for (i = 0; i < NUM_SCRIPT_STRINGS; i++) {
					if (string_data[i] == NULL) {
						string_data[i] = new char[length + 1];
						strncpy(string_data[i],f + 1,length);
						string_data[i][length] = 0;
						if (length >= 255)
							string_data[i][255] = 0;
						token.what_sort = i;
						i = 2 * NUM_SCRIPT_STRINGS;
					}
				} 
			}
            if (i == NUM_SCRIPT_STRINGS) {
				ASB_big(script_to_load," script failure: Too many strings in line ","","",cur_line,".");
				delete[] token_list;
                token_list = NULL;
				return FALSE;
			}

        	token.type = 47;
        	s++;
		}
    	else {
	        if ( *s ){
				ASB_big(script_to_load," script failure: Invalid symbol in line ","","",cur_line,".");
				delete[] token_list;
                token_list = NULL;
				return FALSE;
	        }
        }
       
       // add token to list
       if (cur_list_position >= num_tokens) {
			ASB_big(script_to_load," internal error: Token stack overflow in ","","",cur_line,".");
 			delete[] token_list;
 		    token_list = NULL;
			return FALSE;
		}
       	
       if (token.type == 17) {
            // Now, we may want to take this number and use it to initialize a variable
            if ((defining_new_variables) && (current_variable_created >= 0)) {
        		switch (defining_new_variable_type) {
        			case 0: case 1: // int
        				int_var_values[current_variable_created] = token.what_sort;
        				break;
        			case 2: // string
						ASB_big(script_to_load," internal error: Bad initialization in ","","",cur_line,".");
        				break;
        			case 3: // location
        				if (token_list[cur_list_position - 1].type == 17)
        					location_var_values[current_variable_created].y = token.what_sort;
        					else location_var_values[current_variable_created].x = token.what_sort;
        				break;
				}

			}
		}
       token.line = cur_line;
       token_list[cur_list_position] = token;
       cur_list_position++;
   }

	delete script;	
	script = NULL;

	return TRUE;
}

Boolean script_type::token_type_match(short which_token,short type)
{
	if (which_token >= num_tokens)
		return FALSE;
	if (token_list[which_token].type == type)
		return TRUE;
	return FALSE;
}

Boolean script_type::token_sort_match(short which_token,short what_sort)
{
	if (which_token >= num_tokens)
		return FALSE;
	if (token_list[which_token].what_sort == what_sort)
		return TRUE;
	return FALSE;
}
Boolean script_type::token_match(short which_token,short type,short what_sort)
{
	if (which_token >= num_tokens)
		return FALSE;
	if (token_list[which_token].what_sort != what_sort)
		return FALSE;
	if (token_list[which_token].type != type)
		return FALSE;
	return TRUE;	
}

// given a token type and sort, finds the first token in the script matching it.
// if what_sort is -1, just find first token with type matching type_of_token
short script_type::find_first_matching_token(short type_of_token,short what_sort)
{
	short i;
	for (i = 0; i < num_tokens; i++){
		if ((token_list[i].type == type_of_token) && ((what_sort < 0) || (what_sort == token_list[i].what_sort)))
		  	return i;
	}
	return -1;
}

// given a token type and sort and a start_token from which to begin search, finds the 
// next token in the script matching it.
// if what_sort is -1, just find first token with type matching type_of_token
short script_type::find_next_matching_token(short start_token,short type_of_token,short what_sort)
{
	if (start_token >= num_tokens)
		return -1;
	for (short i = start_token; i < num_tokens; i++){
		if ((token_list[i].type == type_of_token) && ((what_sort < 0) || (what_sort == token_list[i].what_sort)))
		  	return i;
	}
	return -1;
}

// given a token type and sort and a start_token from which to begin search, finds the 
// closest token in the script before the start point matching it.
// if what_sort is -1, just find first token with type matching type_of_token
short script_type::find_previous_matching_token(short start_token,short type_of_token,short what_sort)
{
	if (start_token >= num_tokens)
		return -1;
	for (short i = start_token; i >= 0; i--){
		if ((token_list[i].type == type_of_token) && ((what_sort < 0) || (what_sort == token_list[i].what_sort)))
		  	return i;
	}
	return -1;
}

// start_token is some token in the script. This function returns the end of the current 
// line. in other words, it returns the last token before when the script needs to start running code
// again. This is going to be either a semicolon or a right bracket.
// This procedure assumes that the token is at the beginning of a line/command. It won't work right
// if it is called, say, inside the code that it should be telling the script to skip.
// returns -1 if hits unexpected EOL.
short script_type::end_of_current_line(short start_token)
{
	short current_bracket_count = 0;
	Boolean got_brackets = FALSE;

	if (start_token >= num_tokens)
		return -1;
	while (start_token < num_tokens) {
		
	  	if (token_type_match(start_token,LEFTBRACKET_TYPE)) {
	  		current_bracket_count++;
	  		got_brackets = TRUE;
		}
		else if (token_type_match(start_token,RIGHTBRACKET_TYPE))
			current_bracket_count--;
	  			
	  	if ((token_type_match(start_token,SEMICOLON_TYPE)) && (got_brackets == FALSE))
	  		return start_token;
	  	if ((current_bracket_count == 0) && (got_brackets == TRUE))
	  		return start_token;
	  		
	  	start_token++;
	}
	return -1;
}

// Suppose we hit a right bracket while running the script. There MUST be a unique conditional
// attached to it. This function assumes start_token points at a right bracket,
// and returns the if/else/while that corresponds to it.
short script_type::find_conditional_matching_bracket(short start_token)
{

	if (start_token >= num_tokens)
		return -1;
	if (token_type_match(start_token,RIGHTBRACKET_TYPE) == FALSE) {
		ASB_big_color(script_name," Error: Internal error 1 in line ","","",token_list[start_token].line,".",1); 
		script_killed = TRUE;
		return -1;		
	}

	short left_matching = find_matching_bracket(start_token);
	if (left_matching < 0) {
		ASB_big_color(script_name," Error: Unmatched right bracket in line ","","",token_list[start_token].line,".",1); 
		script_killed = TRUE;
		return -1;		
	}
	
	left_matching--;
	if (token_type_match(left_matching,FLOW_CONTROLLER_TYPE)) 
		return left_matching;
	
	if (token_type_match(left_matching,RIGHTPARAN_TYPE) == FALSE) {
		ASB_big_color(script_name," Error: Internal error 2 in line ","","",token_list[start_token].line,".",1); 
		script_killed = TRUE;
		return -1;		
	}
	
	short left_matching2 = find_matching_bracket(left_matching);
		
	left_matching2--;
	if (token_type_match(left_matching2,FLOW_CONTROLLER_TYPE)) 
		return left_matching2;
	
	ASB_big_color(script_name," Error: Right bracket without matching conditional in line ","","",token_list[start_token].line,".",1); 
	script_killed = TRUE;
	return -1;	
}

// start_token is one of these tokens: (, ), {, }
// Returns the number of the bracket's partner, or -1 if none such exists
short script_type::find_matching_bracket(short start_token)
{
	if (start_token >= num_tokens)
		return -1;

	short current_count = 1;
	switch (token_list[start_token].type) {
		case LEFTPARAN_TYPE:
			start_token++;
			while (start_token < num_tokens) {
				if (token_type_match(start_token,RIGHTPARAN_TYPE))
					current_count--;
					else if (token_type_match(start_token,LEFTPARAN_TYPE))
						current_count++;
				if (current_count == 0)
					return start_token;
				start_token++;
			}
			break;
		case RIGHTPARAN_TYPE:
			start_token--;
			while (start_token >= 0) {
				if (token_type_match(start_token,LEFTPARAN_TYPE))
					current_count--;
					else if (token_type_match(start_token,RIGHTPARAN_TYPE))
						current_count++;
				if (current_count == 0)
					return start_token;
				start_token--;
			}
			break;
		case LEFTBRACKET_TYPE:
			start_token++;
			while (start_token < num_tokens) {
				if (token_type_match(start_token,RIGHTBRACKET_TYPE))
					current_count--;
					else if (token_type_match(start_token,LEFTBRACKET_TYPE))
						current_count++;
				if (current_count == 0)
					return start_token;
				start_token++;
			}
			break;
		case RIGHTBRACKET_TYPE:
			start_token--;
			while (start_token >= 0) {
				if (token_type_match(start_token,LEFTBRACKET_TYPE))
					current_count--;
					else if (token_type_match(start_token,RIGHTBRACKET_TYPE))
						current_count++;
				if (current_count == 0)
					return start_token;
				start_token--;
			}
			break;
	}
	return -1;
}

// makes sure this is a semicolon. if not, errors
Boolean script_type::semicolon_check(short check_token)
{
	if (token_type_match(check_token,SEMICOLON_TYPE) == FALSE) {
		ASB_big_color(script_name," Error: Missing semicolon in line ","","",token_list[check_token].line,".",1); 
		script_killed = TRUE;
		return FALSE;
	}
	return TRUE;
}

// Given a state number, finds the first node inside that state.
// In error, returns -1.
// optim this can be vastly improved by just making a library of them.
short script_type::find_start_of_state(short which_state)
{
	short current_pos = find_first_matching_token(BLOCK_TYPE,12);
	if (current_pos < 0) {
		script_killed = TRUE;
		ASB_big_color(script_name," Error: No body declared.","","",-1,".",1); 
		return -1;
	}

	current_pos++;	
	if (semicolon_check(current_pos) == FALSE)
		return -1;


	while ((current_pos = find_next_matching_token(current_pos,BLOCK_TYPE,16)) >= 0) {
		current_pos++;
		if (token_type_match(current_pos,NUMBER_TYPE) == FALSE) {
			ASB_big_color(script_name," Error: State with no number in line ","","",token_list[current_pos].line,".",1); 
			return -1;
		}
		if (token_list[current_pos].what_sort == which_state) {
			current_pos++;
			if (semicolon_check(current_pos) == FALSE)
				return -1;
			return current_pos + 1;
		}
	}
	return -1;
}


// CREATURE CALLING FUNCTIONS
// All of these are the functions called to make the script do things for a creature.

// Assumes this is a creature script. Parses through the tokens and initializes the creature properly.
// Only sets up tokens and variable. DOES NOT run any actual code, but it does call function
// which does. The intro state needs to be
// launched somewhere separately.
Boolean script_type::process_creature_data()
{
/*
	if (script_killed)
		return FALSE;
	if ((type_of_script != 6) || (num_tokens == 0) || (token_list == NULL)) {
		ASB_big_color(script_name," Error: Tried to run invalid creature script.","","",-1,".",1); 
		script_killed = TRUE; return FALSE;
		}

	// make sure creature is still usable.
	if ((which_object < 0) || (chars[which_object].ok() == FALSE))
		return FALSE;
		
	// now parse through the tokens
	if ((token_match(0,BLOCK_TYPE,13) == FALSE) || (token_type_match(1,SEMICOLON_TYPE) == FALSE)){
		ASB_big_color(script_name," Error: Creature script bad header.","","",-1,".",1); 
		script_killed = TRUE; return FALSE;
		}
	
	// Find the position of the first state
	short start_state = chars[which_object].current_state;
	short start_node = find_start_of_state(start_state);
	if (start_node < 0) {
		ASB_big_color(script_name," Error: State not found - ","","",chars[which_object].current_state,"",1); 
		script_killed = TRUE; return FALSE;
		}
		
	run_script(start_node);
*/
	return TRUE;
}

// GIven the dialogue script for a town/out section, We will now commence parsing through all of the script and loading the approproate 
// information into the store_words array for later use. We're going to be very liberal in he parsing, since we're allowing
// missing date in the talk nodes that will never be used. 
Boolean script_type::process_dialogue_data()
{
/*
	if (script_killed)
		return FALSE;
	if ((type_of_script != 1) || (num_tokens == 0) || (token_list == NULL)) {
		ASB_big_color(script_name," Error: Tried to process invalid dialogue script.","","",-1,".",1); 
		script_killed = TRUE; return FALSE;
		}
		
	// now parse through the tokens
	if ((token_match(0,BLOCK_TYPE,20) == FALSE) || (token_type_match(1,SEMICOLON_TYPE) == FALSE)){
		ASB_big_color(script_name," Error: Dialogue script bad header.","","",-1,".",1); 
		script_killed = TRUE; return FALSE;
		}
	
	// We will now commence parsing through all of the script.
	reinitialize_store_words_array();

	short next_token = 0;
	short current_dialogue_node = -1;
	short which_type;
	short start_point = find_next_matching_token(0,BLOCK_TYPE,21);
	if (start_point < 0)
		return TRUE;
	next_token = start_point;
	
	// Now go through every token in the Scenario data file, figure out what each line
	// is doing, and, if necessary, set a data value.
	for (short f = start_point; (f < num_tokens) && (token_list[f].type != 0); f = next_token) {
		switch (token_list[next_token].type) { // We're at the beginning of a line now
			case 1: // block definer
				if ((token_list[next_token].what_sort > 21) && (current_dialogue_node < 0)) {
					ASB_big("Dialogue script error: No dialogue node selected in line ","","","",token_list[next_token].line,".");
					script_killed = TRUE; return FALSE;
					}
				switch (token_list[next_token].what_sort) {
					case 20: // begintalkscript
						if (next_token > 0) {
							ASB_big("Dialogue script error: Extra begintalkscript in line ","","","",token_list[next_token].line,".");
							script_killed = TRUE; return FALSE;
							}
						next_token++;
						break;

					case 21: // begintalknode
						next_token++;
						if (token_type_match(next_token,NUMBER_TYPE) == FALSE) {
							ASB_big("Dialogue script error: Missing number in line ","","","",token_list[next_token].line,".");
							script_killed = TRUE; return FALSE;
							}
						current_dialogue_node = token_list[next_token].what_sort;

						if ((current_dialogue_node < 0) || (current_dialogue_node >= NUM_TALK_NODES)) {
							ASB_big("Dialogue script error: Dialogue node out of range in line ","","","",token_list[next_token].line,".");
							script_killed = TRUE; return FALSE;
							}
						next_token++;
						break;
					case 22: case 23: case 24:
						which_type = token_list[next_token].what_sort;
						next_token++;
						if (token_type_match(next_token,EQUALS_TYPE) == FALSE) {
							ASB_big("Dialogue script error: Missing = in line ","","","",token_list[next_token].line,".");
							script_killed = TRUE; return FALSE;
							}
						next_token++;
						if (token_type_match(next_token,NUMBER_TYPE) == FALSE) {
							ASB_big("Dialogue script error: Missing number in line ","","","",token_list[next_token].line,".");
							script_killed = TRUE; return FALSE;
							}
							
						switch (which_type) {
							case 22: // personality
								t_store_words[current_dialogue_node].personality = token_list[next_token].what_sort;
								break;
							case 23: // state
								t_store_words[current_dialogue_node].state = token_list[next_token].what_sort;
								break;
							case 24: // next_state
								t_store_words[current_dialogue_node].next_state = token_list[next_token].what_sort;
								break;
							}
						next_token++;
						break;
					case 25: // condition
						next_token++;
						if (token_type_match(next_token,EQUALS_TYPE) == FALSE) {
							ASB_big("Dialogue script error: Missing = in line ","","","",token_list[next_token].line,".");
							script_killed = TRUE; return FALSE;
							}
						next_token++;
						t_store_words[current_dialogue_node].condition_token = next_token;
						next_token = find_next_matching_token(next_token,SEMICOLON_TYPE,-1);
						if (next_token < 0) {
							ASB_big("Dialogue script error: Missing ; in line ","","","",token_list[next_token].line,".");
							script_killed = TRUE; return FALSE;
							}
						break;
					case 26: case 27: case 28: case 29: 
					case 30: case 31: case 32: case 33: // All texts 
						which_type = token_list[next_token].what_sort;
						next_token++;
						if (token_type_match(next_token,EQUALS_TYPE) == FALSE) {
							ASB_big("Dialogue script error: Missing = in line ","","","",token_list[next_token].line,".");
							script_killed = TRUE; return FALSE;
							}
						next_token++;
						if (token_type_match(next_token,STRING_TYPE) == FALSE) {
							ASB_big("Dialogue script error: Missing string data in line ","","","",token_list[next_token].line,".");
							script_killed = TRUE; return FALSE;
							}
						t_store_words[current_dialogue_node].text_string_nums[which_type - 26] = token_list[next_token].what_sort;
						next_token++;
						break;						
					case 34: // action
						next_token++;
						if (token_type_match(next_token,EQUALS_TYPE) == FALSE) {
							ASB_big("Dialogue script error: Missing = in line ","","","",token_list[next_token].line,".");
							script_killed = TRUE; return FALSE;
							}
						next_token++;
						short count = 0;
						while ((token_type_match(next_token + count,NUMBER_TYPE)) && (count < 5)) {
							t_store_words[current_dialogue_node].action_values[count] = token_list[next_token + count].what_sort;
							count++;
							}
						next_token += count;
						break;
					case 35: // question
						next_token++;
						if (token_type_match(next_token,EQUALS_TYPE) == FALSE) {
							ASB_big("Dialogue script error: Missing = in line ","","","",token_list[next_token].line,".");
							script_killed = TRUE; return FALSE;
							}
						next_token++;
						if (token_type_match(next_token,STRING_TYPE) == FALSE) {
							ASB_big("Dialogue script error: Missing string data in line ","","","",token_list[next_token].line,".");
							script_killed = TRUE; return FALSE;
							}
						t_store_words[current_dialogue_node].question_string_num = token_list[next_token].what_sort;
						next_token++;
						break;			
					case 36: // code_token
						next_token++;
						if (token_type_match(next_token,EQUALS_TYPE) == FALSE) {
							ASB_big("Dialogue script error: Missing = in line ","","","",token_list[next_token].line,".");
							script_killed = TRUE; return FALSE;
							}
						next_token++;
						t_store_words[current_dialogue_node].code_token = next_token;
						next_token = find_next_matching_token(next_token,BLOCK_TYPE,18);
						if (next_token < 0) {
							ASB_big("Dialogue script error: Missing matching break in line ","","","",token_list[next_token].line,".");
							script_killed = TRUE; return FALSE;
							}
						next_token++;
						break;


					default:
						ASB_big("Dialogue script error: Improper block definer in line ","","","",token_list[next_token].line,".");
						return FALSE;
						break;
					}
				break;
			case SEMICOLON_TYPE:
				break;
			default:
				ASB_big("Dialogue script error:  in line ","","","",token_list[next_token].line,".");
				return FALSE;
				break;
			}
		if (token_type_match(next_token,SEMICOLON_TYPE) == FALSE){
			ASB_big("Dialogue script error: Missing semicolon in line ","","","",token_list[next_token].line,".");
			return FALSE;
			}
		next_token++;
		} 		

*/
	return TRUE;
}
Boolean script_type::run_zone_script()
{
/*
	if (script_killed)
		return FALSE;
	if ((type_of_script != 4) || (num_tokens == 0) || (token_list == NULL)) {
		ASB_big_color(script_name," Error: Tried to run invalid zone script.","","",-1,".",1); 
		script_killed = TRUE; return FALSE;
		}
		
	// now parse through the tokens
	if ((token_match(0,BLOCK_TYPE,15) == FALSE) || (token_type_match(1,SEMICOLON_TYPE) == FALSE)){
		ASB_big_color(script_name," Error: Zone file bad header.","","",-1,".",1); 
		script_killed = TRUE; return FALSE;
		}
	
	// Find the position of the first state
	short start_state = game.current_zone_script_state;
	short start_node = find_start_of_state(start_state);
	if (start_node < 0) {
		ASB_big_color(script_name," Error: State not found - ","","",game.current_zone_script_state,"",1); 
		script_killed = TRUE; return FALSE;
		}
		
	run_script(start_node);
*/
	return TRUE;
}

// Assumes this is a creature's script. Runs it. Figures out what creature it's for.
// RUns. Tells creature what to do. If necessary, changes creature's active state.
Boolean script_type::run_creature_script()
{
/*
	if (script_killed)
		return FALSE;
	if ((type_of_script != 2) || (num_tokens == 0) || (token_list == NULL)) {
		ASB_big_color(script_name," Error: Tried to run invalid creature script.","","",-1,".",1); 
		script_killed = TRUE; return FALSE;
		}

	// make sure creature is still usable.
	if ((which_object < 0) || (chars[which_object].ok() == FALSE))
		return FALSE;
		
	// now parse through the tokens
	if ((token_match(0,BLOCK_TYPE,14) == FALSE) || (token_type_match(1,SEMICOLON_TYPE) == FALSE)){
		ASB_big_color(script_name," Error: Creature file bad header.","","",-1,".",1); 
		script_killed = TRUE; return FALSE;
		}
	
	// Find the position of the first state
	short start_state = chars[which_object].current_state;
	short start_node = find_start_of_state(start_state);
	if (start_node < 0) {
		ASB_big_color(script_name," Error: State not found - ","","",chars[which_object].current_state,"",1); script_killed = TRUE;
		script_killed = TRUE; return FALSE;
		}
		
	run_script(start_node);
*/
	return TRUE;
}


// Here is where the rubber meets the road. Here, the script is actually activated and does stuff.
// It goes until it hits a break, an error, or the end of the file.
// Returns true if running was successful with no errors.
Boolean script_type::run_script(short next_token)
{
/*
	short i,j;
	
	if (next_token <= 0)
		return FALSE;
	if (script_killed)
		return FALSE;

	Boolean quit_run = FALSE;
	
	short expression_value,new_position;
	short matching_bracket;
	Boolean need_semicolon;
	short num_nodes_done = 0;
	
	while (quit_run == FALSE) {
		if (num_nodes_done++ > 1000) {
			ASB_big_color(script_name," Error: Too many nodes used in line ","","",token_list[next_token].line,".",1);
			ASB_color("  Your script is doing too much, hot-shot!",1);
			script_killed = TRUE; return FALSE;
		}
		
		need_semicolon = TRUE;
		
		// At this point, we must be at the beginning of a line. We look for 
		// one of the legal things a line can begin with, and process based on that.
		// The next time we get here, we will be right at the beginning of the newest command.
		// This is usually, but not always, right after a semicolon.
		switch (token_list[next_token].type) { // We're at the beginning of a line now
			case BLOCK_TYPE: // block definer. If not break, it's an error.
				if (token_list[next_token].what_sort == 18)
					quit_run = TRUE;
				else {
					ASB_big(script_name," Error: State not ended properly in line ","","",token_list[next_token].line,".");
					script_killed = TRUE; 
					return FALSE;
				}
				next_token++;
				break;
			case FLOW_CONTROLLER_TYPE:
				short controller_type = token_list[next_token].what_sort;
				next_token++;
				if (controller_type != 2) { //if else, need a conditional
					if (token_type_match(next_token,LEFTPARAN_TYPE) == FALSE) {
						ASB_big(script_name," Error: Missing ( in line ","","",token_list[next_token].line,".");
						script_killed = TRUE; 
						return FALSE;
					}				
					next_token++;
				}
					
				switch (controller_type) { 
					case 0: // if
						if (evaluate_int_expression(next_token,&expression_value,&new_position) == FALSE)
							return FALSE;

						next_token = new_position;

						if (token_type_match(next_token,RIGHTPARAN_TYPE) == FALSE) {
							ASB_big(script_name," Error: Missing ) in line ","","",token_list[next_token].line,".");
							script_killed = TRUE; 
							return FALSE;
						}				
						next_token++;

						matching_bracket = -1;
						if (token_type_match(next_token,LEFTBRACKET_TYPE)) {
							matching_bracket = find_matching_bracket(next_token);
							if (matching_bracket < 0) {
								ASB_big(script_name," Error: Unmatched left bracket in line ","","",token_list[next_token].line,".");
								script_killed = TRUE; return FALSE;
							}				
						}

						need_semicolon = FALSE;
						if (expression_value == 0) { // FALSE
							// first, skip stuff we don't want to call.
							if (matching_bracket >= 0)
								next_token = matching_bracket;
							else 
								next_token = end_of_current_line(next_token);
							next_token++;

							
							// next, see if we have an else statement to enter. If so, jump over the
							// else so it doesn't mess things up.
							if (token_match(next_token,FLOW_CONTROLLER_TYPE,2)) {
								next_token++;
								if (token_type_match(next_token,LEFTBRACKET_TYPE)) 
									next_token++;
								}
							}
							else { // TRUE
								if (token_type_match(next_token,LEFTBRACKET_TYPE)) 
									next_token++;
							}
						
						break;
					case 1: // while
						if (evaluate_int_expression(next_token,&expression_value,&new_position) == FALSE)
							return FALSE;

						next_token = new_position;

						if (token_type_match(next_token,RIGHTPARAN_TYPE) == FALSE) {
							ASB_big(script_name," Error: Missing ) in line ","","",token_list[next_token].line,".");
							script_killed = TRUE; 
							return FALSE;
						}				
						next_token++;

						matching_bracket = -1;
						if (token_type_match(next_token,LEFTBRACKET_TYPE)) {
							matching_bracket = find_matching_bracket(next_token);
							if (matching_bracket < 0) {
								ASB_big(script_name," Error: Unmatched left bracket in line ","","",token_list[next_token].line,".");
								script_killed = TRUE; 
								return FALSE;
							}				
						}
						else {
							ASB_big(script_name," Error: While loop without left bracket in line ","","",token_list[next_token].line,".");
							script_killed = TRUE; 
							return FALSE;
						}

						need_semicolon = FALSE;
						if (expression_value == 0) { // FALSE
							// Skip stuff we don't want to call.
							if (matching_bracket >= 0)
								next_token = matching_bracket;
							else 
								next_token = end_of_current_line(next_token);
							next_token++;
						}
						else { // TRUE
							if (token_type_match(next_token,LEFTBRACKET_TYPE)) 
								next_token++;
						}
						
						break;
					case 2: // else
						// if we reach an else this way, that means that we've just exited a successful if.
						// in that case, just skip it.
						need_semicolon = FALSE;
						next_token = end_of_current_line(next_token);
						next_token++;
				
						break;
					}
					
				break;

// short procedure_passed_variable_types[NUM_PROCEDURE_PASS_VARS];
//	0 - int 1 - loc 2 - string constant 3 - string variable
// short procedure_passed_values[NUM_PROCEDURE_PASS_VARS];
// pix_loc procedure_passed_locations[NUM_PROCEDURE_PASS_VARS];
			case PROCEDURE_TYPE:
				// First, load in all the variables. Then call the procedure running feature
				short procedure_type = token_list[next_token].what_sort;
				short procedure_line = token_list[next_token].line;
				next_token++;
				if (token_type_match(next_token,LEFTPARAN_TYPE) == FALSE) {
					ASB_big(script_name," Error: Missing ( in line ","","",token_list[next_token].line,".");
					script_killed = TRUE; return FALSE;
				}				
				next_token++;
									
				for (i = 0; i < NUM_PROCEDURE_PASS_VARS; i++)
					procedure_passed_variable_types[i] = -1;
				short current_variable = 0;			
				while (token_type_match(next_token,RIGHTPARAN_TYPE) == FALSE) {
					if (token_type_match(next_token,STRING_VARIABLE_TYPE)) {
						procedure_passed_variable_types[current_variable] = 3;
						procedure_passed_values[current_variable] = token_list[next_token].what_sort;
						next_token++;
					}
					else if (token_type_match(next_token,STRING_TYPE)) {
						procedure_passed_variable_types[current_variable] = 2;
						procedure_passed_values[current_variable] = token_list[next_token].what_sort;
						next_token++;
					}
					else if (token_type_match(next_token,LOCATION_VARIABLE_TYPE)) {
						procedure_passed_variable_types[current_variable] = 1;
						procedure_passed_locations[current_variable] = location_var_values[token_list[next_token].what_sort];
						next_token++;
					}
					else {
						if (evaluate_int_expression(next_token,&expression_value,&new_position) == FALSE)
							return FALSE;

						next_token = new_position;

						procedure_passed_variable_types[current_variable] = 0;
						procedure_passed_values[current_variable] = expression_value;
					}

					current_variable++;
					if (token_type_match(next_token,COMMA_TYPE) == TRUE) 
						next_token++;

					}
				quit_run = run_procedure(procedure_type,procedure_line);
				next_token++;
				break;
			case INT_VARIABLE_TYPE:
				short int_used = token_list[next_token].what_sort;
				next_token++;
				if (token_type_match(next_token,EQUALS_TYPE) == FALSE) {
					ASB_big(script_name," Error: Missing = in line ","","",token_list[next_token].line,".");
					script_killed = TRUE; return FALSE;
					}
				next_token++;
				if (evaluate_int_expression(next_token,&expression_value,&new_position) == FALSE)
					return FALSE;
				int_var_values[int_used] = expression_value;
				next_token = new_position;
				break;
			case LOCATION_VARIABLE_TYPE:
				short loc_used = token_list[next_token].what_sort;
				next_token++;
				if (token_type_match(next_token,EQUALS_TYPE) == FALSE) {
					ASB_big(script_name," Error: Missing = in line ","","",token_list[next_token].line,".");
					script_killed = TRUE; 
					return FALSE;
				}
				next_token++;
				if (token_type_match(next_token,LOCATION_FUNCTION_TYPE) == FALSE) {
					ASB_big(script_name," Error: Wrong function type in line ","","",token_list[next_token].line,".");
					script_killed = TRUE; 
					return FALSE;
				}
				short function_used = token_list[next_token].what_sort;
				short which_line = token_list[next_token].line;
				next_token++;
				if (token_type_match(next_token,LEFTPARAN_TYPE) == FALSE) {
					ASB_big(script_name," Error: Missing left parenthesis in line ","","",token_list[next_token].line,".");
					script_killed = TRUE; 
					return FALSE;
				}
				next_token++;
				if (evaluate_int_expression(next_token,&expression_value,&new_position) == FALSE)
					return FALSE;								
				
				next_token = new_position + 1;
				location_var_values[loc_used] = location_function_value(function_used,expression_value,which_line);
				break;
			case STRING_VARIABLE_TYPE:
				break;
			case SEMICOLON_TYPE: // multiple semicolons ok
				break;
			case RIGHTBRACKET_TYPE: 
				// We look back and see where this bracket is from. If it's from a while, we jump
				// back to it.
				need_semicolon = FALSE;

				short matching_conditional = find_conditional_matching_bracket(next_token);
				if (token_match(matching_conditional,FLOW_CONTROLLER_TYPE,1)) { // is while
					next_token = matching_conditional;
					}
					else next_token++;				
				break;
			case UNARY_FUNCTION_TYPE: case BINARY_FUNCTION_TYPE: case TRINARY_FUNCTION_TYPE: case NO_PARAM_FUNCTION_TYPE:
				if (evaluate_int_expression(next_token,&expression_value,&new_position) == FALSE)
					return FALSE;
				next_token = new_position;
				break; 
			default:

				
				ASB_big(script_name," Error: Improper command in line ","","",token_list[next_token].line,".");
				script_killed = TRUE;
				return FALSE;
				break;
			}
		
		// We should be pointing at a semicolon now.
		if ((need_semicolon) && (semicolon_check(next_token) == FALSE))
			return FALSE;

		if (need_semicolon)
			next_token++;
		}
*/
	return TRUE;
}


// short next_token is the first node in a mathematical expression. This function
// evaluates it and sets result to the correct value.
// Returns TURE if successful or FALSE if an error. 
Boolean script_type::evaluate_int_expression(short next_token,short *result,short *new_position)
{
    short tokenListLength = 0;
    short tokenStackTop = -1;
	Boolean expression_ended = FALSE;
	token_type token,topOp,op;
	// stacks for evaluating numerical expressions
	token_type tokenList[MAX_NUMERICAL_EXPR_LENGTH];
	token_type tokenStack[MAX_NUMERICAL_EXPR_LENGTH];
	short stack[MAX_NUMERICAL_EXPR_LENGTH];

	// STEP 1: Create a stack of tokens with all of the operations and numbers.
	while (expression_ended == FALSE) {
		token = token_list[next_token];
		
		if (token.type == SEMICOLON_TYPE) {
			expression_ended = TRUE;
		}
		else if (token.type == NUMBER_TYPE || token.type == INT_VARIABLE_TYPE ) {
			tokenList[tokenListLength++] = token;
			next_token++;
		}
		else {
			if ( tokenStackTop >= 0 )
				topOp = tokenStack[tokenStackTop];

			while ((tokenStackTop >= 0) && (higher_precedence(topOp,token))) {
				op = tokenStack[tokenStackTop--];
				tokenList[tokenListLength++] = op;
				if ( tokenStackTop >= 0 )
				topOp = tokenStack[tokenStackTop];
			}

			if ( token.type == COMMA_TYPE ) {
			// If there is no ( on stack, comma is an end condition.
				if ( tokenStackTop < 0 )
					expression_ended = TRUE;
			}
			else if ( token.type != RIGHTPARAN_TYPE ) {
				tokenStack[++tokenStackTop] = token;
			}
			else if ( tokenStackTop >= 0 ) {
				op = tokenStack[tokenStackTop--];
			} 
			else {
			// unmatched right parenthesis. This is a stop condition.
				expression_ended = TRUE;
									
			//ASB_big(script_name," Error: Unmatched right parenthesis in line ","","",token_list[next_token].line,".");
			//script_killed = TRUE; return FALSE;
			}
			if (expression_ended == FALSE)
				next_token++;
		}
		if (next_token >= num_tokens) {
			ASB_big(script_name," Error: Unexpected end of file in line ","","",token_list[next_token].line,".");
			script_killed = TRUE; return FALSE;
		}
	}
	while ( tokenStackTop >= 0 ) {
		op = tokenStack[tokenStackTop--];
		if ( op.type == LEFTPARAN_TYPE ) {
			ASB_big(script_name," Error: Unmatched left parenthesis in line ","","",token_list[next_token].line,".");
			script_killed = TRUE; return FALSE;
		}
		tokenList[tokenListLength++] = op;
	}
	if ( tokenListLength == 0 ) {
		ASB_big(script_name," Error: Empty expression in line ","","",token_list[next_token].line,".");
		script_killed = TRUE; return FALSE;
	}

	if (tokenListLength >= MAX_NUMERICAL_EXPR_LENGTH) {
		ASB_big(script_name," Major Error: Expression TOO LONG in line ","","",token_list[next_token].line,".");
		script_killed = TRUE; return FALSE;
	}
    
    // STEP 2: Pop everything off of the stack and evaluate the number
    short top = -1;
	short op1,op2,op3;

	for (short i = 0; i < tokenListLength; i++) {
		token = tokenList[i];

		switch ( token.type ) {
			case NUMBER_TYPE:
				stack[++top] = token.what_sort;       // push
				break;
			case INT_VARIABLE_TYPE:
				stack[++top] = int_var_values[token.what_sort];    // push
				break;
			case NO_PARAM_FUNCTION_TYPE:
				stack[++top] = evaluate_no_param_function(token.what_sort,token.line);      // push
				break;
			case UNARY_FUNCTION_TYPE:
				op1 = stack[top--];                // pop
				stack[++top] = evaluate_unary_function(token.what_sort,op1,token.line);      // push
				break;
			case BINARY_FUNCTION_TYPE:
				op2 = stack[top--];                // pop
				op1 = stack[top--];                // pop
				stack[++top] = evaluate_binary_function(token.what_sort,op1,op2,token.line);      // push
				break;
			case TRINARY_FUNCTION_TYPE:
				op3 = stack[top--];                // pop
				op2 = stack[top--];                // pop
				op1 = stack[top--];                // pop
				stack[++top] = evaluate_trinary_function(token.what_sort,op1,op2,op3,token.line);      // push
				break;
			case BINARY_OPERATOR_TYPE:
				op2 = stack[top--];                // pop
				op1 = stack[top--];                // pop
				stack[++top] = evaluate_operator(token.what_sort,op1,op2);  // push
				break;
			default:
				ASB_big(script_name," Error: Bad term in expression in line ","","",token_list[next_token].line,".");
				script_killed = TRUE; return FALSE;
				break;
			}
	}
	
	if (top != 0) {
 		ASB_big(script_name," Error: Bad expression in line ","","",token_list[next_token].line,".");
		script_killed = TRUE; return FALSE;
		}
		
    *result = stack[top--];
	*new_position = next_token;
	return TRUE;
}

// Figures out whether op1 or op2 has higher precedence, which means, basically, that it's 
// done first. Returns TRUE is op1 has higher precedence that op2.
// In general, this is the order, least to most:
// 		( least
// 		+ -
// 		* /
// 		functions
// 		, ) most
Boolean script_type::higher_precedence (token_type op1, token_type op2)
{
	short result = 0;

	switch ( op1.type )
    {
		case LEFTPARAN_TYPE:
			result = 0;
        	break;
    	case BINARY_OPERATOR_TYPE:
    		if ((op2.type == RIGHTPARAN_TYPE) || (op2.type == COMMA_TYPE))
    			result = 1;
			else if ((op2.type == BINARY_OPERATOR_TYPE) && (operator_precedence[op1.what_sort] > operator_precedence[op2.what_sort]))
				result = 1;
			else
				result = 0;
			break;
        case UNARY_FUNCTION_TYPE: 
		case BINARY_FUNCTION_TYPE: 
		case TRINARY_FUNCTION_TYPE: 
		case NO_PARAM_FUNCTION_TYPE:
        	result = op2.type == RIGHTPARAN_TYPE || op2.type == COMMA_TYPE || op2.type == BINARY_OPERATOR_TYPE;
			break;
        default:
        	result = 0;
    }

	return result;
}

// CREATURE CALLING FUNCTIONS
// All of these are the functions called to make the script do things for a scenario.

// Assumes this is a Scenario data file. Parses through the tokens and loads in the 
// proper scenario data.
// Note about Scenario data files. When you start to define a data structure, it imports the
// values from the last structure of that type you edited, EXCEPT for graphics templates and
// the scenario data
Boolean script_type::process_scenario_data()
{
	short array_member;
	
	if ((type_of_script != 0) || (num_tokens == 0) || (token_list == NULL)) {
		ASB("Error: Tried to parse invalid scenario data file.");
		return FALSE;
		}

	
	// now parse through the tokens
	if ((token_match(0,1,0) == FALSE) || (token_type_match(1,SEMICOLON_TYPE) == FALSE)){
		ASB("Error: Scenario data file bad header. Needs beginscendatascript at top.");
		return FALSE;
		}
	
	// do error run on script. Eh. Feeling lazy. Maybe later.
	/*
	short next_token = 0;
	for (short f = 0; ((f >= num_tokens) || (token_list[f].type == 0); f = next_token) {
		switch (token_list[f].type) {
			case 1:
				if (token_type_match(f + 1,SEMICOLON_TYPE) == FALSE) {
					ASB_big("Scenario data script error: Improper command in line ","","",cur_line,".");
					return FALSE;
					}
					
				break;
	
			default:
				ASB_big("Scenario data script error: Improper command in line ","","",cur_line,".");
				return FALSE;
				break;
			}
		} 
	*/
	
	// Process script
	short current_char_edited = -1;
	short current_floor_edited = -1;
	short current_terrain_edited = -1;
	short current_item_edited = -1;

	short cur_editing_type = -1;
	short type_of_edit;
	short new_value;
	Boolean assignment_result;
				
	short next_token = 0;
	// Now go through every token in the Scenario data file, figure out what each line
	// is doing, and, if necessary, set a data value.
	for (short f = 0; (f < num_tokens) && (token_list[f].type != 0); f = next_token) {
		switch (token_list[next_token].type) { // We're at the beginning of a line now
			case 1: // block definer
				switch (token_list[next_token].what_sort) {
					case 0:
						if (next_token > 0) {
							ASB_big("Scenario data file error: Extra beginscenscript in line ","","","",token_list[next_token].line,".");
							return FALSE;
							}
						next_token++;
						break;

					case 1: case 2: case 3: case 4:  // edit creature, floor, ter, item
						cur_editing_type = token_list[next_token].what_sort;
						next_token++;
						if (token_type_match(next_token,NUMBER_TYPE) == FALSE) {
							ASB_big("Scenario data file error: Missing number in line ","","","",token_list[next_token].line,".");
							return FALSE;
							}
						
						// picking new char to edit
						 
						// give instance values of last instance of type edited
						// then switch what we're editing
						switch (cur_editing_type) {
							case 1:
								if (token_list[next_token].what_sort != minmax(0,255,token_list[next_token].what_sort)) {
									ASB_big("Scenario data file error: Edited creature out of range (0..255) in line ","","","",token_list[next_token].line,".");
									return FALSE;
									}
								if (current_char_edited >= 0)
									scen_data.scen_creatures[token_list[next_token].what_sort] = scen_data.scen_creatures[current_char_edited];
								current_char_edited = token_list[next_token].what_sort;
								break;
							case 2:
								if (token_list[next_token].what_sort != minmax(0,255,token_list[next_token].what_sort)) {
									ASB_big("Scenario data file error: Edited floor out of range (0..255) in line ","","","",token_list[next_token].line,".");
									return FALSE;
									}
								if (current_floor_edited >= 0)
									scen_data.scen_floors[token_list[next_token].what_sort] = scen_data.scen_floors[current_floor_edited];
								current_floor_edited = token_list[next_token].what_sort;
								break;
							case 3:
								if (token_list[next_token].what_sort != minmax(0,512,token_list[next_token].what_sort)) {
									ASB_big("Scenario data file error: Edited terrain type out of range (0..511) in line ","","","",token_list[next_token].line,".");
									return FALSE;
									}
								if (current_terrain_edited >= 0)
									scen_data.scen_terrains[token_list[next_token].what_sort] = scen_data.scen_terrains[current_terrain_edited];
								current_terrain_edited = token_list[next_token].what_sort;
								break;
							case 4:
								if (token_list[next_token].what_sort != minmax(0,500,token_list[next_token].what_sort)) {
									ASB_big("Scenario data file error: Edited item out of range (0..499) in line ","","","",token_list[next_token].line,".");
									return FALSE;
									}
								if (current_item_edited >= 0) {

									scen_data.scen_items[token_list[next_token].what_sort] = scen_data.scen_items[current_item_edited];
									}
								current_item_edited = token_list[next_token].what_sort;
								break;

							}
						next_token++;
						break;

					
					case 9: // scenario data
						cur_editing_type = token_list[next_token].what_sort;
						next_token++;
						break;	
					default:
						ASB_big("Scenario data file error: Improper block definer in line ","","","",token_list[next_token].line,".");
						return FALSE;
						break;
					}
				break;
			case SEMICOLON_TYPE:
				break;
			case 44: // variable definer ... we're setting a variable
				if (cur_editing_type < 0) {
					ASB_big("Scenario data file error: Unexpected variable definer in line ","","","",token_list[next_token].line,".");
					return FALSE;
					}
				type_of_edit = token_list[next_token].what_sort;
				
				next_token++;
				
				if (type_of_edit == 201) { // clean?
					switch (cur_editing_type) {
						case 1:
							scen_data.scen_creatures[current_char_edited].clear_creature_type();
							break;
						case 2:
							scen_data.scen_floors[current_floor_edited].clear_floor_values();
							break;
						case 3:
							scen_data.scen_terrains[current_terrain_edited].clear_terrain_values();
							break;
						case 4:
							scen_data.scen_items[current_item_edited].clear_item_record_type();
							break;

						default:
							ASB_big("Scenario data file error: Out of place import command in line ","","","",token_list[next_token].line,".");
							return FALSE;
							break;
						}
					
					break;
					}
				
				if (token_type_match(next_token,EQUALS_TYPE) == FALSE) {
					ASB_big("Scenario data file error: Missing = in line ","","","",token_list[next_token].line,".");
					return FALSE;
					}
				next_token++;
				if (token_type_match(next_token,NUMBER_TYPE) == FALSE) {
					ASB_big("Scenario data file error: Missing number in line ","","","",token_list[next_token].line,".");
					return FALSE;
					}

				new_value = token_list[next_token].what_sort;

				// Now, this may be an import command. if so, we have to handle it in a special way.
				if (type_of_edit == 200) { // import?
					assignment_result = TRUE;
					switch (cur_editing_type) {
						case 1:
							scen_data.scen_creatures[current_char_edited] =
								scen_data.scen_creatures[new_value];
							break;
						case 2:
							scen_data.scen_floors[current_floor_edited] =
								scen_data.scen_floors[new_value];
							break;
						case 3:
							scen_data.scen_terrains[current_terrain_edited] =
								scen_data.scen_terrains[new_value];
							break;
						case 4:
							scen_data.scen_items[current_item_edited] =
								scen_data.scen_items[new_value];
							break;

						default:
							ASB_big("Scenario data file error: Out of place import command in line ","","","",token_list[next_token].line,".");
							return FALSE;
							break;
					}
				}
				else {
					switch (cur_editing_type) {
						case 1: // editing a character
							assignment_result = set_char_variable(current_char_edited,type_of_edit,new_value);
							break;
						case 2: // editing a floor
							assignment_result = set_floor_variable(current_floor_edited,type_of_edit,new_value);
							break;
						case 3: // editing terrain
							assignment_result = set_terrain_variable(current_terrain_edited,type_of_edit,new_value);
							break;
						case 4: // editing item
							assignment_result = set_item_variable(current_item_edited,type_of_edit,new_value);
							break;

						default:
							ASB_big("Scenario data file error: Out of place variable definer in line ","","","",token_list[next_token].line,".");
							return FALSE;
							break;
					}
				}
				if (assignment_result == FALSE) {
					ASB_big("Scenario data file error: Assignment error in line ","","","",token_list[next_token].line,".");
					return FALSE;			
					}
				next_token++;
				break;
				
			case 45: // variable array definer ... we're setting a value in an array
				if (cur_editing_type < 0) {
					ASB_big("Scenario data file error: Unexpected array definer in line ","","","",token_list[next_token].line,".");
					return FALSE;
					}
				type_of_edit = token_list[next_token].what_sort;
				next_token++;
				if (token_type_match(next_token,NUMBER_TYPE) == FALSE) {
					ASB_big("Scenario data file error: Missing array member in line ","","","",token_list[next_token].line,".");
					return FALSE;
					}
					
				array_member = token_list[next_token].what_sort;
				
				next_token++;
				if (token_type_match(next_token,EQUALS_TYPE) == FALSE) {
					ASB_big("Scenario data file error: Missing = in line ","","","",token_list[next_token].line,".");
					return FALSE;
					}
				next_token++;
				if (token_type_match(next_token,NUMBER_TYPE) == FALSE) {
					ASB_big("Scenario data file error: Missing value in line ","","","",token_list[next_token].line,".");
					return FALSE;
					}

				new_value = token_list[next_token].what_sort;

				switch (cur_editing_type) {
					case 1: // editing a character
						assignment_result = set_char_array_variable(current_char_edited,array_member,type_of_edit,new_value);
						break;

					default:
						ASB_big("Scenario data file error: Out of place array definer in line ","","","",token_list[next_token].line,".");
						return FALSE;
						break;
					}
				if (assignment_result == FALSE) {
					ASB_big("Scenario data file error: Array assignment error in line ","","","",token_list[next_token].line,".");
					return FALSE;			
					}
				next_token++;
				break;
			case 46: // string definer ... we're setting a string in one of the records
				if (cur_editing_type < 0) {
					ASB_big("Scenario data file error: Unexpected string definer in line ","","","",token_list[next_token].line,".");
					return FALSE;
					}
				type_of_edit = token_list[next_token].what_sort;
				
				next_token++;
				if (token_type_match(next_token,EQUALS_TYPE) == FALSE) {
					ASB_big("Scenario data file error: Missing = in line ","","","",token_list[next_token].line,".");
					return FALSE;
					}
				next_token++;
				if (token_type_match(next_token,STRING_TYPE) == FALSE) {
					ASB_big("Scenario data file error: Missing string in line ","","","",token_list[next_token].line,".");
					return FALSE;
					}

				if (string_data[token_list[next_token].what_sort] == NULL) {
					ASB_big("Scenario data file error: Very weird. Missing string data in line ","","","",token_list[next_token].line,".");
					return FALSE;
					}
				switch (cur_editing_type) {
					case 1: // editing a character
						assignment_result = set_char_string(current_char_edited,type_of_edit,string_data[token_list[next_token].what_sort]);
						break;
					case 2: // editing a floor
						assignment_result = set_floor_string(current_floor_edited,type_of_edit,string_data[token_list[next_token].what_sort]);
						break;
					case 3: // editing a terrain
						assignment_result = set_terrain_string(current_terrain_edited,type_of_edit,string_data[token_list[next_token].what_sort]);
						break;
					case 4: // editing item
						assignment_result = set_item_string(current_item_edited,type_of_edit,string_data[token_list[next_token].what_sort]);
						break;

					default:
						ASB_big("Scenario data file error: Out of place string in line ","","","",token_list[next_token].line,".");
						return FALSE;
						break;
					}
				if (assignment_result == FALSE) {
					ASB_big("Scenario data file error: String assignment error in line ","","","",token_list[next_token].line,".");
					return FALSE;			
					}
				next_token++;
				break;
			default:
				ASB_big("Scenario data file error: Improper command in line ","","","",token_list[next_token].line,".");
				return FALSE;
				break;
			}
		if (token_type_match(next_token,SEMICOLON_TYPE) == FALSE){
			ASB_big("Scenario data file error: Missing semicolon in line ","","","",token_list[next_token].line,".");
			return FALSE;
			}
		next_token++;
		} 		
	return TRUE;
}


void scrub_clean_all_brains()
{
/*
	for (short i = 0; i < NUM_CHARS; i++)
		if (creature_scripts[i] != NULL) { 
			delete creature_scripts[i];
			creature_scripts[i] = NULL;
			}
	for (short i = 0; i < NUM_OBJ; i++)
		if (object_scripts[i] != NULL) { 
			delete object_scripts[i];
			object_scripts[i] = NULL;
			}
	if (zone_script != NULL) {
		delete zone_script;
		zone_script = NULL;
		}
	if (zone_dialogue_script != NULL) {
		delete zone_dialogue_script;
		zone_dialogue_script = NULL;
		}
		*/
}

// loads, initializes the special enc. script for the current town.
void load_town_script()
{
/*
	if (zone_script != NULL) { // brain already here! kill it!
		delete zone_script;
		zone_script = NULL;
		}

	//if ((strlen(zone.zone_script) == 10) &&
	  //(strncmp("NullZoneSc",zone.zone_script,10)))
		//return;
	if (!strcmp(zone.zone_script,"NullZoneSc"))
		return;
		
	zone_script = new script_type;

	if (zone_script == NULL) {
		ASB_big_color("Error: Couldn't create an AI for zone. Script: ",zone_script->script_name,"","",-1,"",1);
		return;
		}
	char message[256];
	zone.zone_script[12] = 0;
	sprintf(message,"%s.txt",zone.zone_script);
	
	zone_script->load_script(4,message);
	zone_script->which_object = game.current_zone;
	game.current_zone_script_state = 0;
	strcpy(zone_script->script_name,message);

	//zone_script->run_zone_script();
	if (zone_script->run_zone_script() == TRUE) {
		//ASB("Zone script initialized."); // dbug
		}
	if (game.current_zone_script_state == 0)
		game.current_zone_script_state = 2;
		*/
}

void run_zone_script()
{	
/*	
	if (zone_script == NULL) { // bad news. no brain to run.
		return;
		}
		
	zone_script->run_zone_script();		
*/
}

Boolean run_zone_script_at_state(short state)
{		
/*	
	if (zone_script == NULL) { // bad news. no brain to run.
		return FALSE;
		}
		
	short start_node = zone_script->find_start_of_state(state);
	if (start_node < 0)
		return FALSE;
		
	short my_current_state = game.current_zone_script_state;
	game.current_zone_script_state = state;
	zone_script->run_zone_script();
	if (game.current_zone_script_state == state)
		game.current_zone_script_state = my_current_state;
*/
	
	return TRUE;
	
}

// When loading a saved game, we will load in the saved variables for the scripts,
// so we'll just need to load in the tokens which go with them.
// script_data is a pointer to the script record which needs the tokens and already
// has all other data
void load_tokens_for_initialized_script(script_type *script_data)
{
	script_data->token_list = NULL;

	for (short i = 0; i < NUM_SCRIPT_STRINGS; i++)
		script_data->string_data[i] = NULL;
	for (short i = 0; i < NUM_SCRIPT_STRING_VARS; i++)
		script_data->string_var_values[i] = NULL;

	script_data->load_script(script_data->type_of_script,script_data->script_name,0);
}

// loads, initializes an ai script for a creature.
void load_creature_brain(short char_num)
{
/*
	if (creature_scripts[char_num] != NULL) { // brain already here! kill it!
		delete creature_scripts[char_num];
		creature_scripts[char_num] = NULL;
		}
	if (chars[char_num].ok() == FALSE)
		return;
	if (strlen(chars[char_num].script) == 0)
		return;
				
	creature_scripts[char_num] = new script_type;

	if (creature_scripts[char_num] == NULL) {
		ASB_big_color("Error: Couldn't create an AI for creature ","","","",char_num,"!",1);
		return;
		}
	char message[256];
	chars[char_num].script[12] = 0;
	sprintf(message,"%s.txt",chars[char_num].script);
	
	creature_scripts[char_num]->load_script(2,message);
	creature_scripts[char_num]->which_object = char_num;
	chars[char_num].current_state = 0;
	strcpy(creature_scripts[char_num]->script_name,message);

	creature_scripts[char_num]->process_creature_data();
	//if (creature_scripts[char_num]->process_creature_data() == TRUE)
	//	ASB("Creature data initialized.");

	if (chars[char_num].current_state == 0)
		chars[char_num].current_state = 2;
		*/
}

// Returns whether any script got run or not.
Boolean run_char_brain_at_state(short char_num,short state)
{
/*
	if (chars[char_num].ok() == FALSE)
		return FALSE;
		
	if (creature_scripts[char_num] == NULL) { // bad news. no brain to run.
		return FALSE;
		}
		
	short start_node = creature_scripts[char_num]->find_start_of_state(state);
	if (start_node < 0)
		return FALSE;
		
	short my_current_state = chars[char_num].current_state;
	chars[char_num].current_state = state;
	creature_scripts[char_num]->run_creature_script();
	if (chars[char_num].current_state == state)
		chars[char_num].current_state = my_current_state;
		*/
	
	return TRUE;
}

void try_to_run_creature_brain(short char_num)
{
/*
	if (chars[char_num].ok() == FALSE)
		return;


	if (creature_scripts[char_num] == NULL) { // bad news. no brain to run.
		return;
		}
	if ((chars[char_num].can_act_at_distance == FALSE) && (vdist_to_party(chars[char_num].pos.location) > CHAR_ACT_POINT_RANGE)) 
		return;
	
	
	creature_scripts[char_num]->run_creature_script();		

	// finally, purge anything we might be watching for in the script
	chars[char_num].who_attacked_me_last = -1;
	chars[char_num].current_message = -1;
		*/
}


void load_dialogue_script()
{
	/*
	if (zone_dialogue_script != NULL) { // brain already here! kill it!
		delete zone_dialogue_script;
		zone_dialogue_script = NULL;
		}
	if (strlen(zone.zone_name) == 0)
		return;
	if (!strcmp("NullZoneSc",zone.zone_script))
		return;
					
	zone_dialogue_script = new script_type;

	if (zone_dialogue_script == NULL) {
		ASB_color("Error: Couldn't create a zone dialogue script.",1);
		return;
		}
	char message[256];
	//sprintf(message,"%s.txt",zone.zone_name);
	sprintf(message,"%sdlg.txt",zone.zone_script);
	
	zone_dialogue_script->load_script(1,message);
	zone_dialogue_script->which_object = -1;
	strcpy(zone_dialogue_script->script_name,message);

	if (zone_dialogue_script->process_dialogue_data() == TRUE) {
		//ASB("Zone dialogue initialized."); // dbug
		}
	*/
}

Boolean ok_to_do_talking()
{return TRUE;}
/*
	if (zone_dialogue_script == NULL) 
		return FALSE;
	if (zone_dialogue_script->script_killed)
		return FALSE;
	return TRUE;
}

// For the dialogue node currently being displayed, make all strings being displayed not be displayed
  void turn_off_all_dialogue_strings()
{
	for (short i = 0; i < 8; i++) 
		t_current_string_active[i] = 0;
}
// For the dialogue node currently being displayed, make all strings be displayed
void turn_on_all_dialogue_strings()
{
	for (short i = 0; i < 8; i++) 
		t_current_string_active[i] = 1;
}
// For the dialogue node currently being displayed, make one string not be displayed
void turn_off_a_dialogue_string(short which_string)
{
	if ((which_string < 0) || (which_string >= 8))
		return;
	t_current_string_active[which_string] = 0;
}
// For the dialogue node currently being displayed, make one string not be displayed
void turn_on_a_dialogue_string(short which_string)
{
	if ((which_string < 0) || (which_string >= 8))
		return;
	t_current_string_active[which_string] = 1;
}

Boolean talk_node_shut_off(short which_node)
{
	if ((which_node < 0) || (which_node >= NUM_TALK_NODES))
		return TRUE;	
	
	talk_responses node = t_store_words[which_node];
	if (node.condition_token < 0)
		return FALSE;
		
	short conditional_result = 1,dummy;
	zone_dialogue_script->evaluate_int_expression(node.condition_token,&conditional_result,&dummy);
	if (conditional_result > 0)
		return FALSE;
	return TRUE;
}

void activate_a_talk_node(short which_node)
{
	short a,b,c,d,i,j,k;
	Boolean questions_set = FALSE;
	short num_nodes_used = 0,store_old_state;
	
	talk_responses node = t_store_words[which_node];

	t_current_node = which_node;
	store_old_state = t_current_state;
	t_current_state = node.next_state;
	
	for (i = 0; i < 8; i++) 
		if (node.text_string_nums[i] >= 0) {
			t_current_string_nums[i] = node.text_string_nums[i];
			t_current_string_active[i] = 1;
			}

	a = node.action_values[1];
	b = node.action_values[2];
	c = node.action_values[3];
	d = node.action_values[4];

	// figure out what text remains and process actions. First, do the quickie shortcuts
	if (node.action_values[0] > 0) {
		switch (node.action_values[0]) {
			case 1: // dep_on_sdf
				turn_off_all_dialogue_strings();
				if (GET_SDF(a,b) <= c) {
					turn_on_a_dialogue_string(0);
					turn_on_a_dialogue_string(1);
					}
					else {
						turn_on_a_dialogue_string(2);
						turn_on_a_dialogue_string(3);
						}
				break;
			case 2: // set_sdf
				SET_SDF(a,b,c);
				break;
			case 3: // shop
				t_talk_end_forced = TRUE;
				break;
			case 4: // set_to_1
				turn_off_all_dialogue_strings();
				if (GET_SDF(a,b) == 0) {
					turn_on_a_dialogue_string(0);
					turn_on_a_dialogue_string(1);
					}
					else {
						turn_on_a_dialogue_string(2);
						turn_on_a_dialogue_string(3);
						}
				SET_SDF(a,b,1);
				break;
			case 5: // pay
				break;
			case 6: // end_talk
				t_talk_end_forced = TRUE;
				break;
			case 7: // intro
				if (t_store_personality < 0) {
					turn_off_a_dialogue_string(4);
					turn_off_a_dialogue_string(5);
					turn_off_a_dialogue_string(6);
					turn_off_a_dialogue_string(7);
					break;
					}
				if (!(game.met_personality[game.current_zone][(t_store_personality % 20) / 8] & 
					  (char) (s_pow(2,(t_store_personality % 20) % 8)) )) {
					game.met_personality[game.current_zone][(t_store_personality % 20) / 8] = 
					  game.met_personality[game.current_zone][(t_store_personality % 20) / 8] | (char) (s_pow(2,(t_store_personality % 20) % 8));
					turn_off_a_dialogue_string(4);
					turn_off_a_dialogue_string(5);
					turn_off_a_dialogue_string(6);
					turn_off_a_dialogue_string(7);
					}
					else {
						turn_off_a_dialogue_string(0);
						turn_off_a_dialogue_string(1);
						turn_off_a_dialogue_string(2);
						turn_off_a_dialogue_string(3);
						}

				break;
			}
		}

	// Then run any code involved in activating this node
	if (node.code_token > 0)
		zone_dialogue_script->run_script(node.code_token);

	for (i = 0; i < 8; i++) 
		if (t_current_string_active[i] == 0) 
			t_current_string_nums[i] = -1;	
	for (i = 0; i < 8; i++) {
		t_response_rect[i].right = -1;
		t_response_dest[i] = -1;
		}
	if (t_talk_end_forced == FALSE) {
		while (questions_set == FALSE) {
			for (i = 0; i < NUM_TALK_NODES; i++) {

				if ((i != which_node) && (t_store_words[i].state == t_current_state) &&
				  (t_store_words[i].state > 0) && (t_node_used[i] == 0) &&
				  (t_store_words[i].question_string_num >= 0) && 
				  (strlen(zone_dialogue_script->string_data[t_store_words[i].question_string_num]) > 0)) {
				  	short conditional_result = 1,dummy;
				  	Boolean run_node = TRUE;
				  	if (t_store_words[i].condition_token > 0) {
				  		if (zone_dialogue_script->evaluate_int_expression(t_store_words[i].condition_token,&conditional_result,&dummy)) {
				  			if (conditional_result == 0)
				  				run_node = FALSE;
				  			}
						}
					// This node might be active. Check conditionals.
					if ((run_node) && (num_nodes_used < 8)) {
					//if (num_nodes_used < 8) {
							t_response_dest[num_nodes_used] = i;
							strcpy((char *) t_questions[num_nodes_used],zone_dialogue_script->string_data[t_store_words[i].question_string_num]);
							num_nodes_used++;
							}
					}
				}
			if (num_nodes_used > 0)
				questions_set = TRUE;
				else if (store_old_state == t_current_state)
					questions_set = TRUE;
					else t_current_state = store_old_state;
			}
			
		if (num_nodes_used == 0) {
			// if this is only node on conversation, just end it
			if (t_first_talk_node == which_node) {
				sprintf((char *) t_questions[0],"OK.");
				t_response_dest[0] = -2;				
				}
				else {
					sprintf((char *) t_questions[0],"I have another question.");
					//get_str(questions[0],5,30 + store_personality % 7);
					t_response_dest[0] = t_first_talk_node;
					sprintf((char *) t_questions[1],"That's all for now. Thanks.");
					//get_str(questions[1],5,40 + store_personality % 7);
					t_response_dest[1] = -2;
					}
			}
		
		}
		else {
			if (t_store_personality < 0) 
				sprintf((char *) t_questions[0],"OK.");
				else sprintf((char *) t_questions[0],"The conversation ends.");
			t_response_dest[1] = -2;
			}
	

	for (i = 0; i < 8; i++) 
		if (t_current_string_nums[i] >= 0) {
			strcpy(t_current_string_text[i],zone_dialogue_script->string_data[t_current_string_nums[i]]);
			
			for (short j = 0; j < strlen(t_current_string_text[i]) ; j++)
				if (t_current_string_text[i][j] == '_')
					t_current_string_text[i][j] = 34;
			}	
}
*/

// "Town: Corner Wall display in realistic mode" fix
// added patch_corescendata2() to fix the bug on corescendata2.txt

// Loads the core scenario data from the scripts corescendata.txt and corescendata2.txt
Boolean load_core_scenario_data()
{
	script_type *scenario_script = new script_type;
	if (scenario_script == NULL) {
		ASB("Major Error 1: Couldn't initialize core scenario data. Probably, either you ran out of memory or some change has been made to the files corescendata.txt or corescendata2.txt.");
		ASB("There was an error when loading the basic scenario data script which came with the editor. You won't be able to edit any scenarios until the problems is corrected. You may want to reinstall the game.");
		return FALSE;
		}
	
	// load first scen data
	if (scenario_script->load_script(0,"corescendata",0) == FALSE) {
		ASB("Major Error 2: Couldn't initialize core scenario data. Probably, either you ran out of memory or some change has been made to the files corescendata.txt or corescendata2.txt.");
		ASB("There was an error when loading the basic scenario data script which came with the editor. You won't be able to edit any scenarios until the problems is corrected. You may want to reinstall the game.");
		delete scenario_script;
		return FALSE;
		}
	if (scenario_script->process_scenario_data() == FALSE) {
		ASB("Major Error 3: Couldn't initialize core scenario data. Probably, either you ran out of memory or some change has been made to the files corescendata.txt or corescendata2.txt.");
		ASB("There was an error when loading the basic scenario data script which came with the editor. You won't be able to edit any scenarios until the problems is corrected. You may want to reinstall the game.");
		delete scenario_script;
		return FALSE;
		}		
	delete scenario_script;
	scenario_script = NULL;

	scenario_script = new script_type;
	if (scenario_script == NULL) {
		ASB("Major Error 4: Couldn't initialize core scenario data. Probably, either you ran out of memory or some change has been made to the files corescendata.txt or corescendata2.txt.");
		ASB("There was an error when loading the basic scenario data script which came with the editor. You won't be able to edit any scenarios until the problems is corrected. You may want to reinstall the game.");
		return FALSE;
		}
	
	// load first scen data
	if (scenario_script->load_script(0,"corescendata2",0) == FALSE) {
		ASB("Major Error 5: Couldn't initialize core scenario data. Probably, either you ran out of memory or some change has been made to the files corescendata.txt or corescendata2.txt.");
		ASB("There was an error when loading the basic scenario data script which came with the editor. You won't be able to edit any scenarios until the problems is corrected. You may want to reinstall the game.");
		delete scenario_script;
		return FALSE;
		}
	if (scenario_script->process_scenario_data() == FALSE) {
		ASB("Major Error 6: Couldn't initialize core scenario data. Probably, either you ran out of memory or some change has been made to the files corescendata.txt or corescendata2.txt.");
		ASB("There was an error when loading the basic scenario data script which came with the editor. You won't be able to edit any scenarios until the problems is corrected. You may want to reinstall the game.");
		delete scenario_script;
		return FALSE;
		}		
	delete scenario_script;

	patch_corescendata2();		// correct bugs on corescendata2.txt

	return TRUE;

}

// "Town: Corner Wall display in realistic mode" fix
// correct bugs on corescendata2.txt
// 
// In corescendata2.txt, the definition of terrain 6 has next bug
//
// begindefineterrain 6; // nw wall
//	te_which_icon = 5;
//	te_ed_which_icon = 4;
//	te_cutaway_which_sheet = -1;
//	te_full_move_block = 0;
//	te_full_look_block = 0;
//	te_blocks_view_e = 0;	// <-- this line is needed
//	te_move_block_n = 1;
//	te_look_block_n = 1;
//	te_blocks_view_n = 1;
//	te_move_block_w = 1;
//	te_look_block_w = 1;
//	te_blocks_view_w = 1;
//
// This bug affects not only terrain 6, but terrain 7, 42 and 43.
// And because of this bug, NW corner wall display in realistic mode becomes odd.
// It's better to fix this bug on corescendata2.txt itself, but it may cause unpredictable errors on the BoA Game.
// For this reason, correct them here.

void patch_corescendata2( void )
{
	// equivalant of "te_blocks_view_e = 0;"
	scen_data.scen_terrains[6].blocks_view[3] = 0;
	scen_data.scen_terrains[7].blocks_view[3] = 0;
	scen_data.scen_terrains[42].blocks_view[3] = 0;
	scen_data.scen_terrains[43].blocks_view[3] = 0;
}

// Loads the indivudual scenario script, if it exists. Returns true
// if there is no problem. Note that it returns true if the scenario's individual script
// doesn't exist (which will probably usually be true).
Boolean load_individual_scenario_data(char *scenario_name,Boolean force_reload)
{
	char scen_data_file_name[256];
	sprintf((char *) scen_data_file_name,"%sdata",scenario_name);
	if (check_script_exists(scen_data_file_name,1) == FALSE)
		return TRUE;
	
	script_type *scenario_script = new script_type;
	if (scenario_script == NULL) {
		ASB("Major Error 7: Couldn't initialize the script data for your scenario's data script. You ran out of memory, probably.");
		ASB("There was an error when loading the scenario data script. You won't be able to edit this scenario until the problems in the script are corrected.");
		return FALSE;
		}
		
	if (scenario_script->load_script(0,scen_data_file_name,1) == FALSE) {
		ASB("Major Error 9: There was an error when loading your scenario's custom data script. You won't be able to edit this scenario until the problems in the script are corrected.");
		return FALSE;
		}
		
	if (scenario_script->process_scenario_data() == FALSE) {
		ASB("Major Error 8: There was an error when loading your scenario's custom data script. You won't be able to edit this scenario until the problems in the script are corrected.");
		return FALSE;
		}
		
	delete scenario_script;
	return TRUE;

}

// file_location: 0 - in main data folder, 1 - in scenario folder
Boolean check_script_exists(char *check_file_name,short file_location)
{

	Str255 file_name;
	OSErr error = noErr;
	
	sprintf((char *) file_name,"%s.txt",check_file_name);

	c2p(file_name);

	short file_id;
	
	// first, figure out length of file
	switch (file_location) {
		case 0: error = HOpen(start_volume,start_dir,
		  file_name,1,&file_id); break;
		case 1: error = HOpen(current_scenario_file_info.vRefNum,current_scenario_file_info.parID,
		  file_name,1,&file_id); break;
		}
	if (error != noErr) {return FALSE;}
	error = FSClose(file_id);
	return TRUE;
}

short value_limit(short start_value,short min,short max,Boolean *error)
{
	if (start_value != minmax(min,max,start_value)) {
		ASB("A value assigned to a creature/terrain type/floor/item was out of range.");
		*error = TRUE;
		}
	return minmax(min,max,start_value);
}

Boolean set_char_variable(short which_char_type,short which_value,short new_value)
{
	Boolean error = FALSE;
	
	switch (which_value) {
		case 110: scen_data.scen_creatures[which_char_type].level = value_limit(new_value,0,200,&error); break;
		case 111: scen_data.scen_creatures[which_char_type].hp_bonus = value_limit(new_value,-30000,30000,&error); break;
		case 112: scen_data.scen_creatures[which_char_type].sp_bonus = value_limit(new_value,-30000,30000,&error); break;
		case 113: scen_data.scen_creatures[which_char_type].spec_abil = value_limit(new_value,0,50,&error); break;
		case 114: scen_data.scen_creatures[which_char_type].default_attitude = value_limit(new_value,0,5,&error); break;
	
		case 115: scen_data.scen_creatures[which_char_type].species = value_limit(new_value,0,19,&error); break;
		case 116: scen_data.scen_creatures[which_char_type].natural_armor = value_limit(new_value,0,30000,&error); break;
		case 117: scen_data.scen_creatures[which_char_type].a[0] = value_limit(new_value,0,30000,&error); break;
		case 118: scen_data.scen_creatures[which_char_type].a[1] = value_limit(new_value,0,30000,&error); break;
		case 119: scen_data.scen_creatures[which_char_type].a[2] = value_limit(new_value,0,30000,&error); break;
	
		case 120: scen_data.scen_creatures[which_char_type].a1_type = value_limit(new_value,0,10,&error); break;
		case 121: scen_data.scen_creatures[which_char_type].a23_type = value_limit(new_value,0,10,&error); break;
		case 122: scen_data.scen_creatures[which_char_type].ap_bonus = value_limit(new_value,-30000,30000,&error); break;
		case 123: scen_data.scen_creatures[which_char_type].default_strategy = value_limit(new_value,0,19,&error); break;
		case 124: scen_data.scen_creatures[which_char_type].default_aggression = value_limit(new_value,0,100,&error); break;
	
		case 125: scen_data.scen_creatures[which_char_type].default_courage = value_limit(new_value,0,100,&error); break;
		case 126: scen_data.scen_creatures[which_char_type].char_graphic.which_sheet = value_limit(new_value,-30000,30000,&error); break;
		case 127: scen_data.scen_creatures[which_char_type].char_graphic.graphic_adjust = value_limit(new_value,-30000,30000,&error); break;
		case 128: scen_data.scen_creatures[which_char_type].small_or_large_template = value_limit(new_value,-30000,30000,&error); break;
		case 129: scen_data.scen_creatures[which_char_type].char_upper_graphic_sheet = value_limit(new_value,-30000,30000,&error); break;
	
		case 130: scen_data.scen_creatures[which_char_type].summon_class = value_limit(new_value,-30000,30000,&error); break;
		
		default:
			return FALSE;
			break;
		}
	if (error == FALSE)
		return TRUE;
		else return FALSE;
}

Boolean set_char_array_variable(short which_char_type,short which_member,short which_value,short new_value)
{
	Boolean error = FALSE;
	switch (which_value) {
		case 0: scen_data.scen_creatures[which_char_type].which_to_add_to[which_member] = value_limit(new_value,-30000,30000,&error); break;
		case 1: scen_data.scen_creatures[which_char_type].how_much[which_member] = value_limit(new_value,-30000,30000,&error); break;
		case 2: scen_data.scen_creatures[which_char_type].items[which_member] = value_limit(new_value,-30000,30000,&error); break;
		case 3: scen_data.scen_creatures[which_char_type].item_chances[which_member] = value_limit(new_value,-30000,30000,&error); break;
		case 4: scen_data.scen_creatures[which_char_type].immunities[which_member] = value_limit(new_value,-30000,30000,&error); break;


		default:
			return FALSE;
			break;
		}
	if (error == FALSE)
		return TRUE;
		else return FALSE;
}

Boolean set_char_string(short which_char_type,short which_value,char *new_str)
{
	switch (which_value) {
		case 8: 
			if (strlen(new_str) > 19) {
				ASB_big("String error: ",new_str," is too long.","",-1,"");
				return FALSE;
				}
			strcpy(scen_data.scen_creatures[which_char_type].name,new_str); 
			break;
		case 9: 
			if (strlen(new_str) > SCRIPT_NAME_LEN - 1) {
				ASB_big("String error: ",new_str," is too long.","",-1,"");
				return FALSE;
				}
			strcpy(scen_data.scen_creatures[which_char_type].default_script,new_str);
			break;

		default:
			return FALSE;
			break;
		}
	return TRUE;
}

// FLOORS
Boolean set_floor_variable(short which_floor_type,short which_value,short new_value)
{
	Boolean error = FALSE;
	switch (which_value) {
		case 0: scen_data.scen_floors[which_floor_type].pic.which_sheet = value_limit(new_value,-30000,30000,&error); break;
		case 1: scen_data.scen_floors[which_floor_type].pic.which_icon = value_limit(new_value,-30000,30000,&error); break;
		case 2: scen_data.scen_floors[which_floor_type].pic.graphic_adjust = value_limit(new_value,-30000,30000,&error); break;
		case 3: scen_data.scen_floors[which_floor_type].ed_pic.which_sheet = value_limit(new_value,-30000,30000,&error); break;
		case 4: scen_data.scen_floors[which_floor_type].ed_pic.which_icon = value_limit(new_value,-30000,30000,&error); break;

		case 5: scen_data.scen_floors[which_floor_type].ed_pic.graphic_adjust = value_limit(new_value,-30000,30000,&error); break;
		case 6: scen_data.scen_floors[which_floor_type].blocked = value_limit(new_value,-30000,30000,&error); break;
		case 7: scen_data.scen_floors[which_floor_type].step_sound = value_limit(new_value,-30000,30000,&error); break;
		case 8: scen_data.scen_floors[which_floor_type].light_radius = value_limit(new_value,-30000,30000,&error); break;
		case 9: scen_data.scen_floors[which_floor_type].floor_height = value_limit(new_value,-30000,30000,&error); break;

		case 10: scen_data.scen_floors[which_floor_type].special = value_limit(new_value,-30000,30000,&error); break;
		case 11: scen_data.scen_floors[which_floor_type].effect_adjust = value_limit(new_value,-30000,30000,&error); break;
		case 12: scen_data.scen_floors[which_floor_type].is_water = value_limit(new_value,-30000,30000,&error); break;
		case 13: scen_data.scen_floors[which_floor_type].is_floor = value_limit(new_value,-30000,30000,&error); break;
		case 14: scen_data.scen_floors[which_floor_type].is_ground = value_limit(new_value,-30000,30000,&error); break;

		case 15: scen_data.scen_floors[which_floor_type].is_rough = value_limit(new_value,-30000,30000,&error); break;
		case 16: scen_data.scen_floors[which_floor_type].fly_over = value_limit(new_value,-30000,30000,&error); break;
		case 17: scen_data.scen_floors[which_floor_type].shortcut_key = value_limit(new_value,-30000,30000,&error); break;
		case 18: scen_data.scen_floors[which_floor_type].num_anim_steps = value_limit(new_value,-30000,30000,&error); break;
		case 19: scen_data.scen_floors[which_floor_type].shimmers = value_limit(new_value,0,8,&error); break;
		case 20: scen_data.scen_floors[which_floor_type].outdoor_combat_town_used = new_value; break;
		

		default:
			return FALSE;
			break;
		}
	if (error == FALSE)
		return TRUE;
		else return FALSE;
}


Boolean set_floor_string(short which_floor_type,short which_value,char *new_str)
{
	switch (which_value) {
		case 0: 
			if (strlen(new_str) > 19) {
				ASB_big("String error: ",new_str," is too long.","",-1,"");
				return FALSE;
				}

			strcpy(scen_data.scen_floors[which_floor_type].floor_name,new_str); 
			break;

		default:
			return FALSE;
			break;
		}
	return TRUE;
}

// TERRAIN TYPES
Boolean set_terrain_variable(short which_ter_type,short which_value,short new_value)
{
	Boolean error = FALSE;
	switch (which_value) {
		case 25: scen_data.scen_terrains[which_ter_type].pic.which_sheet = value_limit(new_value,-30000,30000,&error); break;
		case 26: scen_data.scen_terrains[which_ter_type].pic.which_icon = value_limit(new_value,-30000,30000,&error); break;
		case 27: scen_data.scen_terrains[which_ter_type].pic.graphic_adjust = value_limit(new_value,-30000,30000,&error); break;
		case 28: scen_data.scen_terrains[which_ter_type].ed_pic.which_sheet = value_limit(new_value,-30000,30000,&error); break;
		case 29: scen_data.scen_terrains[which_ter_type].ed_pic.which_icon = value_limit(new_value,-30000,30000,&error); break;

		case 30: scen_data.scen_terrains[which_ter_type].ed_pic.graphic_adjust = value_limit(new_value,-30000,30000,&error); break;
		case 31: scen_data.scen_terrains[which_ter_type].cut_away_pic.which_sheet = value_limit(new_value,-30000,30000,&error); break;
		case 32: scen_data.scen_terrains[which_ter_type].cut_away_pic.which_icon = value_limit(new_value,-30000,30000,&error); break;
		case 33: scen_data.scen_terrains[which_ter_type].cut_away_pic.graphic_adjust = value_limit(new_value,-30000,30000,&error); break;
		case 34: scen_data.scen_terrains[which_ter_type].second_icon = value_limit(new_value,-30000,30000,&error); break;

		case 35: scen_data.scen_terrains[which_ter_type].second_icon_offset_x = value_limit(new_value,-30000,30000,&error); break;
		case 36: scen_data.scen_terrains[which_ter_type].second_icon_offset_y = value_limit(new_value,-30000,30000,&error); break;
		case 37: scen_data.scen_terrains[which_ter_type].cut_away_second_icon = value_limit(new_value,-30000,30000,&error); break;
		case 38: scen_data.scen_terrains[which_ter_type].cut_away_offset_x = value_limit(new_value,-30000,30000,&error); break;
		case 39: scen_data.scen_terrains[which_ter_type].cut_away_offset_y = value_limit(new_value,-30000,30000,&error); break;

		case 40: scen_data.scen_terrains[which_ter_type].anim_steps = value_limit(new_value,-30000,30000,&error); break;
		case 41: scen_data.scen_terrains[which_ter_type].move_block[0] = value_limit(new_value,-30000,30000,&error); break;
		case 42: scen_data.scen_terrains[which_ter_type].move_block[1] = value_limit(new_value,-30000,30000,&error); break;
		case 43: scen_data.scen_terrains[which_ter_type].move_block[2] = value_limit(new_value,-30000,30000,&error); break;
		case 44: scen_data.scen_terrains[which_ter_type].move_block[3] = value_limit(new_value,-30000,30000,&error); break;

		case 45: scen_data.scen_terrains[which_ter_type].see_block[0] = value_limit(new_value,-30000,30000,&error); break;
		case 46: scen_data.scen_terrains[which_ter_type].see_block[1] = value_limit(new_value,-30000,30000,&error); break;
		case 47: scen_data.scen_terrains[which_ter_type].see_block[2] = value_limit(new_value,-30000,30000,&error); break;
		case 48: scen_data.scen_terrains[which_ter_type].see_block[3] = value_limit(new_value,-30000,30000,&error); break;
		case 49: scen_data.scen_terrains[which_ter_type].blocks_view[0] = value_limit(new_value,-30000,30000,&error); break;

		case 50: scen_data.scen_terrains[which_ter_type].blocks_view[1] = value_limit(new_value,-30000,30000,&error); break;
		case 51: scen_data.scen_terrains[which_ter_type].blocks_view[2] = value_limit(new_value,-30000,30000,&error); break;
		case 52: scen_data.scen_terrains[which_ter_type].blocks_view[3] = value_limit(new_value,-30000,30000,&error); break;
		case 53: scen_data.scen_terrains[which_ter_type].height_adj = value_limit(new_value,-30000,30000,&error); break;
		case 54: scen_data.scen_terrains[which_ter_type].suppress_floor = value_limit(new_value,-30000,30000,&error); break;

		case 55: scen_data.scen_terrains[which_ter_type].light_radius = value_limit(new_value,-30000,30000,&error); break;
		case 56: scen_data.scen_terrains[which_ter_type].step_sound = value_limit(new_value,-30000,30000,&error); break;
		case 57: scen_data.scen_terrains[which_ter_type].shortcut_key = value_limit(new_value,-30000,30000,&error); break;
		case 58: scen_data.scen_terrains[which_ter_type].crumble_type = value_limit(new_value,-30000,30000,&error); break;
		case 59: scen_data.scen_terrains[which_ter_type].beam_hit_type = value_limit(new_value,-30000,30000,&error); break;

		case 60: scen_data.scen_terrains[which_ter_type].terrain_to_crumble_to = value_limit(new_value,-30000,30000,&error); break;
		case 61: scen_data.scen_terrains[which_ter_type].hidden_town_terrain = value_limit(new_value,-30000,30000,&error); break;
		case 62: scen_data.scen_terrains[which_ter_type].swap_terrain = value_limit(new_value,-30000,30000,&error); break;
		case 63: scen_data.scen_terrains[which_ter_type].is_bridge = value_limit(new_value,-30000,30000,&error); break;
		case 64: scen_data.scen_terrains[which_ter_type].is_road = value_limit(new_value,-30000,30000,&error); break;

		case 65: scen_data.scen_terrains[which_ter_type].can_look_at = value_limit(new_value,-30000,30000,&error); break;
		case 66: scen_data.scen_terrains[which_ter_type].special = value_limit(new_value,-30000,30000,&error); break;
		case 67: scen_data.scen_terrains[which_ter_type].effect_adjust = value_limit(new_value,-30000,30000,&error); break;
		case 68: scen_data.scen_terrains[which_ter_type].draw_on_automap = value_limit(new_value,-30000,30000,&error); break;
		case 69: scen_data.scen_terrains[which_ter_type].icon_offset_x = value_limit(new_value,-30000,30000,&error); break;

		case 70: scen_data.scen_terrains[which_ter_type].icon_offset_y = value_limit(new_value,-30000,30000,&error); break;

		case 71: 
			scen_data.scen_terrains[which_ter_type].move_block[0] = value_limit(new_value,0,1,&error);
			scen_data.scen_terrains[which_ter_type].move_block[1] = value_limit(new_value,0,1,&error);
			scen_data.scen_terrains[which_ter_type].move_block[2] = value_limit(new_value,0,1,&error);
			scen_data.scen_terrains[which_ter_type].move_block[3] = value_limit(new_value,0,1,&error);
			break;
		case 72: 
			scen_data.scen_terrains[which_ter_type].see_block[0] = value_limit(new_value,0,1,&error);
			scen_data.scen_terrains[which_ter_type].see_block[1] = value_limit(new_value,0,1,&error);
			scen_data.scen_terrains[which_ter_type].see_block[2] = value_limit(new_value,0,1,&error);
			scen_data.scen_terrains[which_ter_type].see_block[3] = value_limit(new_value,0,1,&error);
			break;
		case 73: scen_data.scen_terrains[which_ter_type].shimmers = value_limit(new_value,0,8,&error); break;
		case 74: scen_data.scen_terrains[which_ter_type].outdoor_combat_town_used = new_value; break;
		default:
			return FALSE;
			break;
		}
	if (error == FALSE)
		return TRUE;
		else return FALSE;
}


Boolean set_terrain_string(short which_ter_type,short which_value,char *new_str)
{
	switch (which_value) {
		case 2: 
			if (strlen(new_str) > 19) {
				ASB_big("String error: ",new_str," is too long.","",-1,"");
				return FALSE;
				}
			strcpy(scen_data.scen_terrains[which_ter_type].ter_name,new_str); 
			break;
		case 3: 
			if (strlen(new_str) > SCRIPT_NAME_LEN - 1) {
				ASB_big("String error: ",new_str," is too long.","",-1,"");
				return FALSE;
				}
			strcpy(scen_data.scen_terrains[which_ter_type].default_script,new_str); 		
			break;

		default:
			return FALSE;
			break;
		}
	return TRUE;
}

// ITEM TYPES
Boolean set_item_variable(short which_item_type,short which_value,short new_value)
{
	
	Boolean error = FALSE;
	switch (which_value) {
		case 75: scen_data.scen_items[which_item_type].variety = value_limit(new_value,-30000,30000,&error); break;
		case 76: scen_data.scen_items[which_item_type].damage = value_limit(new_value,-30000,30000,&error); break;
		case 77: scen_data.scen_items[which_item_type].bonus = value_limit(new_value,-30000,30000,&error); break;
		case 78: scen_data.scen_items[which_item_type].weap_type = value_limit(new_value,-30000,30000,&error); break;
		case 79: scen_data.scen_items[which_item_type].protection = value_limit(new_value,-30000,30000,&error); break;
		
		case 80: scen_data.scen_items[which_item_type].charges = value_limit(new_value,-30000,30000,&error); break;
		case 81: scen_data.scen_items[which_item_type].encumbrance = value_limit(new_value,-30000,30000,&error); break;
		case 82: scen_data.scen_items[which_item_type].item_floor_graphic.which_sheet = value_limit(new_value,-30000,30000,&error); break;
		case 83: scen_data.scen_items[which_item_type].item_floor_graphic.which_icon = value_limit(new_value,-30000,30000,&error); break;
		case 84: scen_data.scen_items[which_item_type].item_floor_graphic.graphic_adjust = value_limit(new_value,-30000,30000,&error); break;
		
		case 85: scen_data.scen_items[which_item_type].inven_icon = value_limit(new_value,-30000,30000,&error); break;
		case 86: scen_data.scen_items[which_item_type].ability_in_slot[0] = value_limit(new_value,-30000,30000,&error); break;
		case 87: scen_data.scen_items[which_item_type].ability_in_slot[1] = value_limit(new_value,-30000,30000,&error); break;
		case 88: scen_data.scen_items[which_item_type].ability_in_slot[2] = value_limit(new_value,-30000,30000,&error); break;
		case 89: scen_data.scen_items[which_item_type].ability_in_slot[3] = value_limit(new_value,-30000,30000,&error); break;
		
		case 90: scen_data.scen_items[which_item_type].ability_strength[0] = value_limit(new_value,-30000,30000,&error); break;
		case 91: scen_data.scen_items[which_item_type].ability_strength[1] = value_limit(new_value,-30000,30000,&error); break;
		case 92: scen_data.scen_items[which_item_type].ability_strength[2] = value_limit(new_value,-30000,30000,&error); break;
		case 93: scen_data.scen_items[which_item_type].ability_strength[3] = value_limit(new_value,-30000,30000,&error); break;
		case 94: scen_data.scen_items[which_item_type].special_class = value_limit(new_value,-30000,30000,&error); break;
		
		case 95: scen_data.scen_items[which_item_type].value = value_limit(new_value,-30000,30000,&error); break;
		case 96: scen_data.scen_items[which_item_type].weight = value_limit(new_value,-30000,30000,&error); break;
		case 97: scen_data.scen_items[which_item_type].value_class = value_limit(new_value,-30000,30000,&error); break;
		case 98: scen_data.scen_items[which_item_type].ident = value_limit(new_value,-30000,30000,&error); break;
		case 99: scen_data.scen_items[which_item_type].magic = value_limit(new_value,-30000,30000,&error); break;
		
		case 100: scen_data.scen_items[which_item_type].cursed = value_limit(new_value,-30000,30000,&error); break;
		case 101: scen_data.scen_items[which_item_type].once_per_day = value_limit(new_value,-30000,30000,&error); break;
		case 102: scen_data.scen_items[which_item_type].junk_item = value_limit(new_value,-30000,30000,&error); break;
		case 103: scen_data.scen_items[which_item_type].missile_anim_type = value_limit(new_value,-1,20,&error); break;
		case 104: scen_data.scen_items[which_item_type].augment_item = new_value; break;
		


		default:
			return FALSE;
			break;
		}
	if (error == FALSE)
		return TRUE;
		else return FALSE;
}

Boolean set_item_array_variable(short which_item_type,short which_member,short which_value,short new_value)
{
	switch (which_value) {

		default:
			return FALSE;
			break;
		}
	return TRUE;
}

Boolean set_item_string(short which_item_type,short which_value,char *new_str)
{
	switch (which_value) {
		case 5: 
			if (strlen(new_str) > 19) {
				ASB_big("String error: ",new_str," is too long.","",-1,"");
				return FALSE;
				}
			strcpy(scen_data.scen_items[which_item_type].name,new_str); 
			break;
		case 6: 
			if (strlen(new_str) > 29) {
				ASB_big("String error: ",new_str," is too long.","",-1,"");
				return FALSE;
				}
			strcpy(scen_data.scen_items[which_item_type].full_name,new_str); 
			break;

		default:
			return FALSE;
			break;
		}
	return TRUE;
}


// FUNCTION EVALUATORS


short evaluate_operator(short operator_type,short op1,short op2)
{
	switch (operator_type) {
		case 0: return op1 + op2;
		case 1: return op1 - op2;
		case 2: return op1 * op2;
		case 3: if (op2 == 0) return 0;
			return op1 / op2;
		case 4: return ((op1 != 0) && (op2 != 0));
		case 5: return ((op1 != 0) || (op2 != 0));
		case 6: return (op1 == op2);
		case 7: return (op1 > op2);
		case 8: return (op1 < op2);
		case 9: return (op1 >= op2);
		case 10: return (op1 <= op2);
		case 11: return (op1 != op2);
		
		default: return 0;
		}
}

short script_type::evaluate_no_param_function(short fcn_type,short which_line)
{
	short to_return = 0;
	Boolean error = FALSE;

	//if ((fcn_type >= 10) && (fcn_type < 20) && (type_of_script != 2) && (type_of_script != 3)) {
	//	ASB_big(script_name," Error: Creature/object-only function in line ","","",which_line,".");
	//	script_killed = TRUE; return 0;
	//	}

		
	//short pc = which_char_is_pc();
	//if (pc < 0)
	//	return 0;
		
	switch (fcn_type) {
		//case 0: to_return = which_char_is_pc(); break;// pc_num
			
		default: return 0;
		}

	if (error) {
		ASB_big(script_name," Error: Function call error (no parameters) in line ","","",which_line,".");
		script_killed = TRUE; return -1;
		}
	return to_return;
}

short script_type::evaluate_unary_function(short fcn_type,short op1,short which_line)
{
	short to_return = 0;
	Boolean error = FALSE;

	//if ((fcn_type >= 20) && (fcn_type < 40) && (type_of_script != 2) && (type_of_script != 3)) {
	//	ASB_big(script_name," Error: Creature/object-only function in line ","","",which_line,".");
	//	script_killed = TRUE; return -1;
	//	}

	// init variable used for then this function has char op1 do something
	// short which_char = (op1 == -1) ? which_object : op1;
	//short pc = which_char_is_pc();
	
	switch (fcn_type) {
		//case 0: // char_ok
			//if ((op1 < 0) || (op1 >= NUM_CHARS)) {
			//	to_return = 0; break; }			
			//to_return = chars[op1].ok(); 
			//break;
			
		default: error = 1; to_return = -1;
		}

	if (error) {
		ASB_big(script_name," Error: Function call error (unary) in line ","","",which_line,".");
		script_killed = TRUE; return -1;
		}
	return to_return;
}

short script_type::evaluate_binary_function(short fcn_type,short op1,short op2,short which_line)
{
	short to_return = 0;
	Boolean error = FALSE;

	// init variable used for then this function has char op1 do something
	// short which_char = (op1 == -1) ? which_object : op1;

	switch (fcn_type) {
		//case 0: case 2: // get_flag, get_sdf
		//	if (game.sdf_ok(op1,op2) == FALSE) {
		//		ASB_big(script_name," Error: Bad SDF in line ","","",which_line,".");
		//		return -1;
		//		}
		//	to_return = game.get_sdf(op1,op2); 
		//	 break;
		//case 1: to_return = char_in_group(which_char,op2); break; // char_in_group
		default: error = 1; break;
		}

	if (error) {
		ASB_big(script_name," Error: Function call error (binary) in line ","","",which_line,".");
		script_killed = TRUE; return -1;
		}
	return to_return;
}

short script_type::evaluate_trinary_function(short fcn_type,short op1,short op2,short op3,short which_line)
{
	short to_return = 0;
	Boolean error = FALSE;
	
	// init variable used for then this function has char op1 do something
	// short which_char = (op1 == -1) ? which_object : op1;
	
	switch (fcn_type) {
	//	case 0: to_return = get_ran(op1,op2,op3); break;
	//	case 1: to_return = op2; break;
			
		default: error = 1;
		}

	if (error) {
		ASB_big(script_name," Error: Function call error (trinary) in line ","","",which_line,".");
		script_killed = TRUE; return -1;
		}
	return to_return;
}
// short procedure_passed_variable_types[NUM_PROCEDURE_PASS_VARS];
//	0 - int 1 - loc 2 - string constant 3 - string variable
// short procedure_passed_values[NUM_PROCEDURE_PASS_VARS];
// pix_loc procedure_passed_locations[NUM_PROCEDURE_PASS_VARS];
// Returns TRUE if this command ends the script.
Boolean script_type::run_procedure(short which_procedure,short which_line)
{
	short i;
	Boolean error = FALSE;

	//short pc = which_char_is_pc();
	//if (pc < 0)
	//	return TRUE;
		
	// first make sure we have right variables
	for (i = 0; i < NUM_PROCEDURE_PASS_VARS; i++)
		if (((procedure_expected_variable_types[which_procedure][i] < 0) && (procedure_passed_variable_types[i] >= 0)) ||
			((procedure_expected_variable_types[which_procedure][i] < 2) && (procedure_expected_variable_types[which_procedure][i] >= 0) &&
			(procedure_expected_variable_types[which_procedure][i] != procedure_passed_variable_types[i])) ||
			((procedure_expected_variable_types[which_procedure][i] == 2) && (procedure_passed_variable_types[i] < 2))) {
				ASB_big(script_name," Error: Wrong number/type of procedure parameters in line ","","",which_line,".");
				script_killed = TRUE; return TRUE;
				}

	//if ((which_procedure >= 10) && (which_procedure < 30) && (type_of_script != 2) && (type_of_script != 3) && (type_of_script != 4)) {
	//	ASB_big(script_name," Error: Creature/object-only procedure in line ","","",which_line,".");
	//	script_killed = TRUE; return TRUE;
	//	}
	//if ((which_procedure >= 30) && (which_procedure < 60) && (type_of_script != 2) && (type_of_script != 4)) {
	//	ASB_big(script_name," Error: Creature-only procedure in line ","","",which_line,".");
	//	script_killed = TRUE; return TRUE;
	//	}
	
	// here's some shortcut variables, to make typing easier. Might be undefined.
/*
	short op1 = procedure_passed_values[0];
	short op2 = procedure_passed_values[1];
	short op3 = procedure_passed_values[2];
	short op4 = procedure_passed_values[3];
	short op5 = procedure_passed_values[4];
*/
	// init variable used for then this function has char op1 do something
	//short which_char = (op1 == -1) ? which_object : op1;
		
	switch (which_procedure) {
		//case 0: ASB_big_color("Debug Value: ","","","",procedure_passed_values[0],"",3); break; // debug_num
		//case 1: print_nums(procedure_passed_values[0],procedure_passed_values[1],procedure_passed_values[2]); break;// print_nums
				
		default: 
			ASB("Peculiar error. Undefined function called.");
			return TRUE;
		}
	
	if (error) {
		ASB_big(script_name," Error: Procedure call error in line ","","",which_line,".");
		script_killed = TRUE; return TRUE;
	}
		
	return FALSE;
}

location script_type::location_function_value(short what_function,short what_passed,short what_line)
{
	Boolean error = FALSE;
	location dummy_loc = {-1,-1};
	
	switch (what_function) {
		//case 0: // character_loc
		//	if (what_passed != minmax(0,NUM_CHARS - 1,what_passed))
		//		error = TRUE;
		//		else if (chars[what_passed].ok() == FALSE)
		//			error = TRUE;
		//			else return chars[what_passed].pos.space_location;
		//	break;

		default: error = TRUE; break;
		}

	if (error) {
		ASB_big(script_name," Error: Location-returning function error in line ","","",what_line,".");
		script_killed = TRUE;
	}
	return dummy_loc;
}