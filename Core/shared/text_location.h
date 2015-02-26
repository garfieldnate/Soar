/**
 * @file text_location.h
 * @author Nathan Glenn <garfieldnate@gmail.com>
 * @date 2015
 *
 * This is a simple, immutable class for indicating a location within
 * text, consisting of a line and column number, as well as an optional
 * file name.
 */
#ifndef TEXT_LOCATION_H
#define TEXT_LOCATION_H
#include <string>
class text_location
{
    public:
        /**
         * Initializes the object with zero/empty values.
         */
        text_location() : line(0), column(0) {}

        /**
         * Initialize the file location with a line and column number.
         * The file name is set to the empty string, indicating that the
         * text is not from a file (STDIN, for example).
         */
        text_location(int line, int column) :
            line(line), column(column) {}

        /**
         * Initialize the object with both line/column number and
         * a file name.
         */
        text_location(int line, int column, std::string file_name) :
            line(line), column(column), file_name(file_name) {}

        /**
         * Return the name of the file indicated by this text
         * location.
         * @return the file name
         */
        std::string get_file_name() const
        {
            return file_name;
        }

        /**
         * Return the line number of the text location.
         * @return line number
         */
        int get_line() const
        {
            return line;
        }

        /**
         * Return the column number of the text location.
         * @return column number
         */
        int get_column() const
        {
            return column;
        }
        text_location& operator=(const text_location& loc)
        {
            line = loc.line;
            column = loc.column;
            file_name = loc.file_name;
            return *this;
        }

    private:
        std::string file_name;
        int line;
        int column;
};

#endif // TEXT_LOCATION_H
