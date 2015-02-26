/**
 * @file tokenizer.h
 * @author Jonathan Voigt <voigtjr@gmail.com>
 * @date 2010
 *
 * Implementation of a simple Tcl-like parser used by the Soar command line
 * interface. Detailed rules of parsing are listed under the class comments for
 * tokenizer below and are very similar to the rules followed by the Tcl
 * language.
 */

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>
#include "indexed_input_buffer.h"
#include "text_location.h"
#include "token.h"
using std::string;
using std::vector;

/**
 * Define PRINT_CALLBACKS to enable dumping of callback argvs to stdout before
 * the callback is called.
 */
//#define PRINT_CALLBACKS 1
#ifdef PRINT_CALLBACKS
#include <iostream>
#endif

/**
 * The soar namespace is not well defined yet, it would be nice if the Soar
 * kernel and surrounding utilities were all defined inside this namespace.
 */
namespace soar
{
    /**
     * Interface called by the tokenizer to be implemented by tokenizer users.
     * When a command is parsed, the command and its arguments (words) are
     * passed to this interface's handle_command function.
     */
    class tokenizer_callback
    {
        public:
            virtual ~tokenizer_callback() {}

            /**
             * Implement to handle commands. The words of the command are in the
             * passed argv vector. The first entry in the vector is the command.
             * The vector is guaranteed to never be empty, though the first command
             * could be.
             * @return true if the command was ok, or false if there is an error.
             *         Returning false will stop parsing and cause
             *         tokenizer::evaluate to return false.
             */
            virtual bool handle_command(vector< Token >& argv) = 0;
    };

    /**
     * Essentially implements a simple Tcl parser, with some exceptions.
     *
     * Takes a string and farily efficiently converts it in to a series of
     * callbacks with arguments separated in to a vector of strings (what Tcl
     * refers to as "words").
     *
     * Here are the rules, copied from the Tcl language summary (man tcl), with
     * modifications [in square brackets] as they apply to this parser. The only
     * deviations appear in rules 5 and 9.
     *
     * [1] A Tcl script is a string containing one or more commands. Semi-colons
     * and newlines are command separators unless quoted as described below.  Close
     * brackets are command terminators during command substitution (see below)
     * unless quoted. [Square brackets have no special meaning in this parser.]
     *
     * [2] A command is evaluated in two steps. First, the Tcl interpreter breaks
     * the command into words and performs substitutions as described below.  These
     * substitutions are performed in the same way for all commands. [The first
     * word of the command has no special meaning in this parser.] All of the words
     * of the command are passed to the command procedure [via a callback
     * interface]. The command procedure is free to interpret each of its words in
     * any way it likes, such as an integer, variable name, list, or Tcl script.
     * Different commands interpret their words differently.
     *
     * [3] Words of a command are separated by white space (except for newlines,
     * which are command separators).
     *
     * [4] If the first character of a word is double-quote (") then the word is
     * terminated by the next double-quote character. If semi-colons, close
     * brackets, or white space characters (including newlines) appear between the
     * quotes then they are treated as ordinary characters and included in the
     * word. Backslash substitution [but not command substitution or variable
     * substitution] is performed on the characters between the quotes as described
     * below. The double-quotes are not retained as part of the word.
     *
     * [5] If the first character of a word is an open brace ({) then the word is
     * terminated by the matching close brace (}). Braces nest within the word: for
     * each additional open brace there must be an additional close brace (however,
     * if an open brace or close brace within the word is quoted with a backslash
     * then it is not counted in locating the matching close brace). No
     * substitutions are performed on the characters between the braces except for
     * backslash-newline substitutions described below, nor do semi-colons,
     * newlines, close brackets, or white space receive any special interpretation.
     * The word will consist of exactly the characters between the outer braces,
     * not including the braces themselves.
     *
     * Soar addendum: due to the use of pipes as quotes, braces that appear between
     * pipes when embedded in braces are considered literals.
     *
     * [6] [Square brackets are not special in this parser. No command
     * substitution.]
     *
     * [7] [Dollar signs are not special in this parser. No variable substitution.]
     *
     * [8] If a backslash (\) appears within a word then backslash substitution
     * occurs. In all cases but those described below the backslash is dropped and
     * the following character is treated as an ordinary character and included in
     * the word. This allows characters such as double quotes, and close brackets
     * [and dollar signs but they aren't special characters in this parser] to be
     * included in words without triggering special processing. The following table
     * lists the backslash sequences that are handled specially, along with the
     * value that replaces each sequence.
     *
     * \a Audible alert (bell) (0x7).
     * \b Backspace (0x8).
     * \f Form feed (0xc).
     * \n Newline (0xa).
     * \r Carriage-return (0xd).
     * \t Tab (0x9).
     * \v Vertical tab (0xb).
     * \<newline>whiteSpace
     *     A single space character replaces the backslash, newline, and all spaces
     *     and tabs after the newline. This backslash sequence is unique in that
     *     [...] it will be replaced even when it occurs between braces, and the
     *     resulting space will be treated as a word separator if it isn't in
     *     braces or quotes.
     * \\ Backslash (``\'').
     * [Not implemented: \ooo The digits ooo (one, two, or three of them) give the
     * octal value of the character.]
     * [Not implemented: \xhh The hexadecimal digits hh give the hexadecimal value
     * of the character. Any number of digits may be present.]
     *
     * Backslash substitution is not performed on words enclosed in braces, except
     * for backslash-newline as described above.
     *
     * [9] Tcl considers # to begin comments only when they are at the
     * beginning of a command. Adhering to this rule makes for unintuitive
     * Soar production syntax. For example, the first closing brace in
     * the follow rule looks commented out, but really isn't because
     * the # is treated as a literal by the CLI:
     *
     *   sp {rule
     *      (state <s> ^superstate nil)
     *   -->
     *      (<s> ^foo bar)
     *     #(<s> ^bar baz)}
     *   }
     *
     * Another gotcha is with commands:
     *
     *   indifferent-selection -g # for RL
     *
     * In Soar, a # anywhere begins a comment, with 3 exceptions:
     *   1. When escaped with a \
     *   2. Between double quotes ("") that are not embedded in brace quotes
     *   3. Between pipes (||) that are embedded in brace quotes
     *
     *
     * [10] [Talks about details regarding substitution and generally does not
     * apply to this parser.]
     *
     * [11] [Talks about details regarding substitution and generally does not
     * apply to this parser.]
     *
     */
    class tokenizer
    {
        private:
            indexed_input_buffer current;   ///< A smart index to the current position in the stream.
            tokenizer_callback* callback;   ///< The current argv callback (only one).
            int command_start_line;         ///< The line that the first word is on.
            const char* error;              ///< Error message.
            Token last_arg;                 ///< Last valid command processed

        public:
            tokenizer()
                : callback(0), error(0)
            {}
            virtual ~tokenizer() {}

            /**
             * Set the current callback handler. There can only be one at a time.
             * To unset, call this with null.
             * @param callback An object to receive the argv callbacks.
             */
            void set_handler(tokenizer_callback* callback)
            {
                this->callback = callback;
            }

            /**
             * Evaluate some input, return true if there were no errors, and issue
             * callbacks at command separators (if a callback is registered).
             * @return true if no errors, parse or errors returned by commands.
             */
            bool evaluate(const char* const input)
            {
                current.set_current(input);
                command_start_line = 1;
                error = 0;

                while (current.good())
                {
                    if (current.eof())
                    {
                        break;
                    }
                    parse_command();
                }
                return current.good();
            }

            /**
             * Returns the line number that the command started on.
             * @return The line number that the command started on.
             */
            int get_command_line_number() const
            {
                return command_start_line;
            }

            /**
             * Returns the current line number, useful when there was a parse
             * error.
             * @return The current line number.
             */
            int get_current_line_number() const
            {
                return current.get_line();
            }

            /**
             * Returns an error string if there was a parse error, or null if the
             * error came from a callback.
             * @return Error message or null if no parse error.
             */
            const char* get_error_string()
            {
                return error;
            }

            /**
             * Returns how many characters of the current line have been read.
             * @return The current offset.
             */
            int get_offset()
            {
                return current.get_offset();
            }

        private:
            /**
             * Parses a command, at least one word. Calls the callback handler.
             */
            void parse_command()
            {
                vector< Token > argv;
                skip_whitespace_and_comments();
                while (parse_word(argv))
                {
                    skip_whitespace();
                }

                if (current.bad())
                {
                    return;
                }

                if (argv.empty())
                {
                    return;
                }

#ifdef PRINT_CALLBACKS
                std::cout << "\n[";
                for (int i = 0; i < argv.size(); i++)
                {
                    std::cout << argv[i] << ",";
                }
                std::cout << "]" << std::endl;
#endif
                if (callback)
                {
                    // TODO: explain this block. What is it for?
                    // Why ignore echoresult?
                    //TODO: need to uncomment this, unless it turns out to be useless
                    // if (((last_arg == "sp") && (argv[0] != "sp")) || ((last_arg == "gp") && (argv[0] != "gp")))
                    // {
                    //     const char* echo_args[] = {"echo", " "};
                    //     vector<string> echo_command(echo_args, echo_args + sizeof(echo_args) / sizeof(echo_args[0]));
                    //     bool echoresult = callback->handle_command(echo_command);
                    // }
                    if (!callback->handle_command(argv))
                    {
                        current.invalidate();
                    }
                    else
                    {
                        last_arg = argv[0];
                    }
                }
            }

            /**
             * Parse the next word, return false when a command separator is
             * encountered. Place the word into argv.
             * @return false if a command separator is encountered.
             */
            bool parse_word(vector< Token >& argv)
            {
                if (current.eof())
                {
                    return false;
                }

                string word;
                if (argv.empty())
                {
                    command_start_line = current.get_line();
                }
                switch (current.get())
                {
                    case ';':
                        break;

                    case '#':
                        skip_to_end_of_line();
                        break;

                    case '"':
                        argv.push_back(read_quoted_string());
                        break;

                    case '{':
                        argv.push_back(read_braces());
                        break;

                    default:
                        argv.push_back(read_normal_word());
                        break;
                }

                return !at_end_of_command();
            }

            /**
             * Store characters for a word that doesn't start
             * with { or " into word.
             */
            Token read_normal_word()
            {
                text_location start = current.get_location();
                text_location last_location;
                string word;
                do
                {
                    char c = current.get();
                    switch (c)
                    {
                        case '\\':
                            c = parse_escape_sequence();
                            if (current.bad())
                            {
                                return Token(start, current.get_location(), word);
                            }
                            break;

                        case ';':
                            return Token(start, last_location, word);

                        case '#':
                            skip_to_end_of_line();
                            return Token(start, last_location, word);;

                        default:
                            last_location = current.get_location();
                            current.increment();
                            break;
                    }
                    word.push_back(c);
                }
                while (!current.eof() && !isspace(current.get()));
                return Token(start, last_location, word);
            }

            /**
             * @return true if at command separator.
             */
            bool at_end_of_command()
            {
                while (current.good())
                {
                    switch (current.get())
                    {
                        case '\n':
                        case ';':
                            current.increment();
                        case 0:
                            return true;

                        default:
                            break;
                    }

                    if (!isspace(current.get()))
                    {
                        return false;
                    }

                    current.increment();
                }
                return true;
            }

            /**
             * Read a word started with a double quote character.
             */
            Token read_quoted_string()
            {
                current.increment(); // consume "
                text_location start = current.get_location();
                string word;
                while (current.get() != '"')
                {
                    switch (current.get())
                    {
                        case 0:
                            error = "unexpected eof";
                            current.invalidate();
                            return Token(start, current.get_location(), word);

                        case '\\':
                        {
                            char c = parse_escape_sequence();
                            if (current.bad())
                            {
                                return Token(start, current.get_location(), word);
                            }
                            word.push_back(c);
                        }
                        break;

                        default:
                            word.push_back(current.get());
                            current.increment();
                            break;
                    }
                }
                Token t = Token(start, current.get_location(), word);
                current.increment(); // consume "
                return t;
            }

            /**
             * The current character is a backslash, return the next character,
             * converting it if necessary.  Special codes become new characters
             * here and are returned as them. Braces and quotes lose special
             * meaning when inside of an escape sequence.
             */
            char parse_escape_sequence()
            {
                current.increment(); // consume backslash

                // TODO: future work? newline, octal, hex, wide hex

                char ret = 0;
                bool increment = true;
                switch (current.get())
                {
                    case 0:
                        error = "unexpected eof";
                        current.invalidate();
                        return 0;
                    case 'a':
                        ret = '\a';
                        break;
                    case 'b':
                        ret = '\b';
                        break;
                    case 'f':
                        ret = '\f';
                        break;
                    case 'n':
                        ret = '\n';
                        break;
                    case 'r':
                        ret = '\r';
                        break;
                    case 't':
                        ret = '\t';
                        break;
                    case 'v':
                        ret = '\v';
                        break;
                    case '\n':
                        skip_whitespace();
                        ret = ' ';
                        increment = false; // skip_whitespace leaves us past where we want to be
                        break;
                    default:
                        ret = current.get();
                        break;
                }
                if (increment)
                {
                    current.increment();
                }
                return ret;
            }

            /**
             * Read a word enclosed in braces. Brace levels must match unless
             * braces are escaped.
             */
            Token read_braces()
            {
                current.increment(); // consume brace;
                text_location start = current.get_location();
                int depth = 1;
                bool literal = false;
                string word;
                text_location last_location;
                bool beginning = true;
                while (depth)
                {
                    char c = current.get();
                    if (c == 0)
                    {
                        error = "unexpected eof, unmatched opening brace";
                        current.invalidate();
                        return Token(start, current.get_location(), word);
                    }
                    last_location = current.get_location();
                    current.increment();
                    if (c == '\\')
                    {
                        // special case for backslash-newline substitution
                        if (current.get() == '\n')
                        {
                            word.push_back(' ');
                            skip_whitespace();
                        }
                        else
                        {
                            // current is escaped but no substitution
                            word.push_back('\\');
                            word.push_back(current.get());
                            current.increment();
                        }
                    }
                    else if (c == '#' && !literal)
                    {
                        skip_to_end_of_line();
                        // if there's a comment at the beginning, then adjust
                        // the start location because we skip comments
                        if (beginning)
                        {
                            start = current.get_location();
                        }
                    }
                    else
                    {
                        if (!literal)
                        {
                            if (c == '{')
                            {
                                ++depth;
                            }
                            else if (c == '}')
                            {
                                if (--depth == 0)  // don't include final }
                                {
                                    return Token(start, last_location, word);
                                }
                            }
                        }
                        word.push_back(c);
                    }
                    if (c == '|')
                    {
                        literal = !literal;
                    }
                    beginning = false;
                }
                return Token(start, last_location, word);
            }

            /**
             * Skip whitespace and end of line comments (which are indicated with a pound sign).
             */
            void skip_whitespace_and_comments()
            {
                skip_whitespace();
                switch (current.get())
                {
                    case '0':
                        return;
                    case '#':
                        skip_to_end_of_line();
                        skip_whitespace_and_comments();
                        return;
                    default:
                        break;
                }
            }

            /**
             * Read until first non-whitespace character.
             */
            void skip_whitespace()
            {
                while (!current.eof() && isspace(current.get()))
                {
                    current.increment();
                }
            }

            /**
             * Read until newline and consume the newline.
             */
            void skip_to_end_of_line()
            {
                while (current.get() != '\n')
                {
                    current.increment();
                    if (current.eof())
                    {
                        break;
                    }
                }
            }
    };
}

#endif // TOKENIZER_H
