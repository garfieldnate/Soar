/**
 * @file indexed_input_buffer.h
 * @author Jonathan Voigt <voigtjr@gmail.com>
 * @date 2010
 *
 * A smart index into an input buffer for use in tokenizing.
 * Encapsulates a lot of code necessary for figuring out
 * where errors happen.
 */
#ifndef INDEXED_INPUT_BUFFER_H
#define INDEXED_INPUT_BUFFER_H
namespace soar {
    class indexed_input_buffer
    {
        private:
            int line;
            int offset;
            const char* current;

        public:
            /**
             * Create with null input buffer, call set_current before using.
             */
            indexed_input_buffer()
            {
                set_current(0);
            }

            /**
             * Create with input buffer, equivalient to using default constructor
             * and calling set_current.
             * @param[in] initial Initial buffer to pass to set_current.
             */
            indexed_input_buffer(const char* initial)
            {
                set_current(initial);
            }

            virtual ~indexed_input_buffer() {}

            /**
             * Set the input buffer, point to the first character in the buffer.
             * Resets line and offset counters.
             * @param initial
             */
            void set_current(const char* initial)
            {
                current = initial;
                if (current && *current)
                {
                    line = 1;
                    offset = 1;
                }
                else
                {
                    line = 0;
                    offset = 0;
                }
            }

            /**
             * Invalidate the current pointer. Used to indicate error state.
             */
            void invalidate()
            {
                current = 0;
            }

            /**
             * Increment the current pointer to the next char in the buffer.  Keeps
             * track of offset in to buffer and newline count as they occur.
             */
            void increment()
            {
                if (*current == '\n')
                {
                    line += 1;
                    offset = 0;
                }
                current += 1;
                offset += 1;
            }

            /**
             * Returns the current line number.
             * @return The current line number.
             */
            int get_line() const
            {
                return line;
            }

            /**
             * Returns how many characters of the current line have been read.
             * @return The current offset.
             */
            int get_offset() const
            {
                return offset;
            }

            /**
             * Returns true if not in an error state.
             * @return true if not in an error state.
             */
            bool good() const
            {
                return current != 0;
            }

            /**
             * Returns true if in an error state.
             * @return true if in an error state.
             */
            bool bad() const
            {
                return !current;
            }

            /**
             * Dereference the pointer and retrieve the current character.
             * This will segfault if this->bad() is true.
             * @return The current character in the stream.
             */
            char get()
            {
                return *current;
            }

            /**
             * Check to see if the current character is the end of input.
             * @return true if at end of input.
             */
            bool eof() const
            {
                return !*current;
            }
    };
}

#endif // INDEXED_INPUT_BUFFER_H
