/*************************************************************************
 * PLEASE SEE THE FILE "license.txt" (INCLUDED WITH THIS SOFTWARE PACKAGE)
 * FOR LICENSE AND COPYRIGHT INFORMATION. 
 *************************************************************************/

#include "lexeme.h"
using soar::Lexeme;

const char* Lexeme::string(){
	return lex_string.c_str();
}

int Lexeme::length(){
	return lex_string.length();
}

//static array providing names for the lexer_token_type members
const char* Lexeme::type_strings[32];
bool Lexeme::initialized = init();
bool Lexeme::init() {
	type_strings[0] = "EOF_LEXEME";
	type_strings[1] = "IDENTIFIER_LEXEME";
	type_strings[2] = "VARIABLE_LEXEME";
	type_strings[3] = "SYM_CONSTANT_LEXEME";
	type_strings[4] = "INT_CONSTANT_LEXEME";
	type_strings[5] = "FLOAT_CONSTANT_LEXEME";
	type_strings[6] = "L_PAREN_LEXEME";
	type_strings[7] = "R_PAREN_LEXEME";
	type_strings[8] = "L_BRACE_LEXEME";
	type_strings[9] = "R_BRACE_LEXEME";
	type_strings[10] = "PLUS_LEXEME";
	type_strings[11] = "MINUS_LEXEME";
	type_strings[12] = "RIGHT_ARROW_LEXEME";
	type_strings[13] = "GREATER_LEXEME";
	type_strings[14] = "LESS_LEXEME";
	type_strings[15] = "EQUAL_LEXEME";
	type_strings[16] = "LESS_EQUAL_LEXEME";
	type_strings[17] = "GREATER_EQUAL_LEXEME";
	type_strings[18] = "NOT_EQUAL_LEXEME";
	type_strings[19] = "LESS_EQUAL_GREATER_LEXEME";
	type_strings[20] = "LESS_LESS_LEXEME";
	type_strings[21] = "GREATER_GREATER_LEXEME";
	type_strings[22] = "AMPERSAND_LEXEME";
	type_strings[23] = "AT_LEXEME";
	type_strings[24] = "TILDE_LEXEME";
	type_strings[25] = "UP_ARROW_LEXEME";
	type_strings[26] = "EXCLAMATION_POINT_LEXEME";
	type_strings[27] = "COMMA_LEXEME";
	type_strings[28] = "PERIOD_LEXEME";
	type_strings[29] = "QUOTED_STRING_LEXEME";
	type_strings[30] = "DOLLAR_STRING_LEXEME";
	type_strings[31] = "NULL_LEXEME";
	return true;
};

const char* Lexeme::GetTypeName(lexer_token_type type){
	return type_strings[type];
}

lexer_token_type Lexeme::GetTypeValue(char* name){
	int last = (int) NULL_LEXEME;
	for(int i = 0; i <= last; i++){
		if (!strcmp(name,type_strings[i])) 
			return (lexer_token_type) i;
	}
	throw std::invalid_argument("Input is not a legal lexer_token_type value");
}
