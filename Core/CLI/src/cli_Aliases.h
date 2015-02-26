#ifndef CLI_ALIASES_H
#define CLI_ALIASES_H

#include <string>
#include <map>
#include <vector>
#include <iterator>
#include "tokenizer.h"
#include "token.h"

namespace cli
{

    class Aliases : public soar::tokenizer_callback
    {
        public:
            Aliases()
            {
                // defaults
                soar::tokenizer t;
                t.set_handler(this);

                t.evaluate("a alias");
                t.evaluate("aw add-wme");
                t.evaluate("chdir cd");
                t.evaluate("ctf command-to-file");
                t.evaluate("set-default-depth default-wme-depth");
                t.evaluate("ex excise");
                t.evaluate("eb explain-backtraces");
                t.evaluate("fc firing-counts");
                t.evaluate("gds_print gds-print");
                t.evaluate("h help");
                t.evaluate("man help");
                t.evaluate("? help");
                t.evaluate("inds indifferent-selection");
                t.evaluate("init init-soar");
                t.evaluate("is init-soar");
                t.evaluate("l learn");
                t.evaluate("dir ls");
                t.evaluate("pr preferences");
                t.evaluate("p print");
                t.evaluate("pc print --chunks");
                t.evaluate("wmes print -i");
                t.evaluate("varprint print -v -d 100");
                t.evaluate("topd pwd");
                t.evaluate("pw pwatch");
                t.evaluate("step run -d");
                t.evaluate("d run -d");
                t.evaluate("e run -e");
                t.evaluate("rw remove-wme");
                t.evaluate("rn rete-net");
                t.evaluate("sn soarnews");
                t.evaluate("st stats");
                t.evaluate("stop stop-soar");
                t.evaluate("ss stop-soar");
                t.evaluate("interrupt stop-soar");
                t.evaluate("un unalias");
                t.evaluate("w watch");
            }

            virtual ~Aliases() {}

            virtual bool handle_command(std::vector< soar::Token >& argv)
            {
                SetAlias(argv);
                return true;
            }

            void SetAlias(const std::vector< soar::Token >& argv)
            {
                if (argv.empty())
                {
                    return;
                }

                // one argument means erase the given alias
                std::vector<soar::Token>::const_iterator i = argv.begin();
                if (argv.size() == 1)
                {
                    aliases.erase(i->get_string());
                }
                else
                // more arguments means clear the given alias and
                // set to the commands following
                {
                    std::vector<std::string>& cmd = aliases[i->get_string()];
                    i++;
                    cmd.clear();
                    for (i; i != argv.end(); i++)
                    {
                        cmd.push_back(i->get_string());
                    }
                }
            }

            bool Expand(std::vector<soar::Token>& argv)
            {
                if (argv.empty())
                {
                    return false;
                }

                std::map< std::string, std::vector< std::string > >::iterator iter =
                    aliases.find(argv.front().get_string());
                // no aliases exist
                if (iter == aliases.end())
                {
                    return false;
                }

                // we replace the aliased word with one or more words; each
                // replaced token has the same start and end locations set as
                // the token that was replaced. This is suboptimal, but TODO.
                std::vector< soar::Token >::iterator insertion = argv.begin();
                soar::Token original_token = *insertion;
                // overwrite first argument in argv
                *insertion = soar::Token(original_token.get_start(), original_token.get_end(), iter->second[0]);
                // insert any remaining args after that one
                for (unsigned int i = 1; i < iter->second.size(); ++i)
                {
                    ++insertion;
                    insertion = argv.insert(insertion, soar::Token(original_token.get_start(), original_token.get_end(), iter->second[i]));
                }

                return true;
            }

            std::map< std::string, std::vector< std::string > >::const_iterator Begin()
            {
                return aliases.begin();
            }

            std::map< std::string, std::vector< std::string > >::const_iterator End()
            {
                return aliases.end();
            }

        private:
            std::map< std::string, std::vector< std::string > > aliases;
    };

} // namespace cli

#endif // CLI_ALIASES_H

