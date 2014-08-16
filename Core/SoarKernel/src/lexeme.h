/*************************************************************************
 * PLEASE SEE THE FILE "license.txt" (INCLUDED WITH THIS SOFTWARE PACKAGE)
 * FOR LICENSE AND COPYRIGHT INFORMATION. 
 *************************************************************************/

#ifndef LEXEME_H
#define LEXEME_H

#include <string>
#include "stdint.h"

/**
 * Types of tokens read by the lexer
 */
enum lexer_token_type {
  EOF_LEXEME,                        /**< end-of-file */
  IDENTIFIER_LEXEME,                 /**< identifier */
  VARIABLE_LEXEME,                   /**< variable */
  SYM_CONSTANT_LEXEME,               /**< symbolic constant */
  INT_CONSTANT_LEXEME,               /**< integer constant */
  FLOAT_CONSTANT_LEXEME,             /**< floating point constant */
  L_PAREN_LEXEME,                    /**< "(" */
  R_PAREN_LEXEME,                    /**< ")" */
  L_BRACE_LEXEME,                    /**< "{" */
  R_BRACE_LEXEME,                    /**< "}" */
  PLUS_LEXEME,                       /**< "+" */
  MINUS_LEXEME,                      /**< "-" */
  RIGHT_ARROW_LEXEME,                /**< "-->" */
  GREATER_LEXEME,                    /**< ">" */
  LESS_LEXEME,                       /**< "<" */
  EQUAL_LEXEME,                      /**< "=" */
  LESS_EQUAL_LEXEME,                 /**< "<=" */
  GREATER_EQUAL_LEXEME,              /**< ">=" */
  NOT_EQUAL_LEXEME,                  /**< "<>" */
  LESS_EQUAL_GREATER_LEXEME,         /**< "<=>" */
  LESS_LESS_LEXEME,                  /**< "<<" */
  GREATER_GREATER_LEXEME,            /**< ">>" */
  AMPERSAND_LEXEME,                  /**< "&" */
  AT_LEXEME,                         /**< "@" */
  TILDE_LEXEME,                      /**< "~" */
  UP_ARROW_LEXEME,                   /**< "^" */
  EXCLAMATION_POINT_LEXEME,          /**< "!" */
  COMMA_LEXEME,                      /**< "," */
  PERIOD_LEXEME,                     /**< "." */
  QUOTED_STRING_LEXEME,              /**< string in double quotes */
  DOLLAR_STRING_LEXEME,              /**< string for shell escape */
  NULL_LEXEME                        /**< Initial value */ 
};  

namespace soar {
    /**
     * A class representing a single lexeme.
     */
    class Lexeme {
        friend class Lexer;
    public:
        Lexeme() : 
            type(NULL_LEXEME),
            int_val(0),
            float_val(0.0),
            id_letter('A'),
            id_number(0){}
        ~Lexeme(){}
        enum lexer_token_type type;         /**< what kind of lexeme it is */
        int64_t int_val;                    /**< for INT_CONSTANT_LEXEME's */
        double float_val;                   /**< for FLOAT_CONSTANT_LEXEME's */
        char id_letter;                     /**< for IDENTIFIER_LEXEME's */
        uint64_t id_number;                 /**< for IDENTIFIER_LEXEME's */
        /**
         * @return the text of the lexeme
         */
        const char* string();
        /**
         * @return the length of the lexeme string
         */
        int length();
        /**
         * Returns the value of the lexer_token_type represented
         * by the input string
         * @param  name of lexer token type
         * @return lexer_token_type value
         */
        static lexer_token_type GetTypeValue(char* string);
        /**
         * Returns the string representation of the input 
         * lexer_token_type
         * @param  lexer_token_type value for which is a name is 
         * desired
         * @return name of the input lexer_token_type
         */
        static const char* GetTypeName(lexer_token_type);
    private:
        /** text of the lexeme */
        std::string lex_string;
        // array providing names for the lexer_token_type members
        static const char* type_strings[32];

        // initialize type_strings statically at run time
        static bool initialized;
        static bool init();
    };
}

#endif //LEXEME_H
