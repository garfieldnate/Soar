#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include "text_location.h"

namespace soar
{
    class Token
    {
        public:
            Token() {}
            Token(text_location start, text_location end,
                  std::string string): start(start), end(end), string(string) {}
            text_location get_start() const
            {
                return start;
            }
            text_location get_end() const
            {
                return end;
            }
            std::string get_string() const
            {
                return string;
            }
            Token& operator=(const Token& token)
            {
                if (this != &token)
                {
                    start = token.start;
                    end = token.end;
                    string = token.string;
                }
                return *this;
            }
        private:
            text_location start;
            text_location end;
            std::string string;
    };
}

#endif // TOKEN_H