#include "portability.h"

#include "unittest.h"

#include "cli_Aliases.h"
#include "cli_Parser.h"
#include "token.h"

class AliasTest : public CPPUNIT_NS::TestCase
{
        CPPUNIT_TEST_SUITE(AliasTest);      // The name of this class

#ifdef DO_ALIAS_TESTS
        CPPUNIT_TEST(testOne);
        CPPUNIT_TEST(testTwo);
        CPPUNIT_TEST(testThree);
        CPPUNIT_TEST(testRemove);
        CPPUNIT_TEST(testDefaults);

        CPPUNIT_TEST(testSimpleCommand);
#endif
        CPPUNIT_TEST_SUITE_END();

    public:
        AliasTest()
        {}
        virtual ~AliasTest() {}

        void setUp();        // Called before each function outlined by CPPUNIT_TEST
        void tearDown();    // Called after each function outlined by CPPUNIT_TEST

    protected:
        cli::Aliases* aliases;

        void testOne();
        void testTwo();
        void testThree();
        void testRemove();
        void testDefaults();

        void testSimpleCommand();
};

CPPUNIT_TEST_SUITE_REGISTRATION(AliasTest);   // Registers the test so it will be used

void AliasTest::setUp()
{
    aliases = new cli::Aliases();
}

void AliasTest::tearDown()
{
    delete aliases;
}

void AliasTest::testOne()
{
    std::vector< soar::Token > a;
    a.push_back(soar::Token(text_location(), text_location(), "alias"));
    a.push_back(soar::Token(text_location(), text_location(), "1"));
    aliases->SetAlias(a);
    std::vector< soar::Token > b;
    b.push_back(soar::Token(text_location(), text_location(), "alias"));
    CPPUNIT_ASSERT(aliases->Expand(b));
    CPPUNIT_ASSERT(b.size() == 1);
    CPPUNIT_ASSERT(b[0].get_string() == "1");
}

void AliasTest::testTwo()
{
    std::vector< soar::Token > a;
    a.push_back(soar::Token(text_location(), text_location(), "alias"));
    a.push_back(soar::Token(text_location(), text_location(), "1"));
    a.push_back(soar::Token(text_location(), text_location(), "2"));
    aliases->SetAlias(a);
    std::vector< soar::Token > b;
    b.push_back(soar::Token(text_location(), text_location(), "alias"));
    CPPUNIT_ASSERT(aliases->Expand(b));
    CPPUNIT_ASSERT(b.size() == 2);
    CPPUNIT_ASSERT(b[0].get_string() == "1");
    CPPUNIT_ASSERT(b[1].get_string() == "2");
}

void AliasTest::testThree()
{
    std::vector< soar::Token > a;
    a.push_back(soar::Token(text_location(), text_location(), "alias"));
    a.push_back(soar::Token(text_location(), text_location(), "1"));
    a.push_back(soar::Token(text_location(), text_location(), "2"));
    a.push_back(soar::Token(text_location(), text_location(), "3"));
    aliases->SetAlias(a);
    std::vector< soar::Token > b;
    b.push_back(soar::Token(text_location(), text_location(), "alias"));
    CPPUNIT_ASSERT(aliases->Expand(b));
    CPPUNIT_ASSERT(b.size() == 3);
    CPPUNIT_ASSERT(b[0].get_string() == "1");
    CPPUNIT_ASSERT(b[1].get_string() == "2");
    CPPUNIT_ASSERT(b[2].get_string() == "3");
}

void AliasTest::testRemove()
{
    std::vector< soar::Token > a;
    a.push_back(soar::Token(text_location(), text_location(), "p"));
    aliases->SetAlias(a);
    std::vector< soar::Token > b;
    b.push_back(soar::Token(text_location(), text_location(), "p"));
    CPPUNIT_ASSERT(!aliases->Expand(b));
    CPPUNIT_ASSERT(b.size() == 1);
    CPPUNIT_ASSERT(b[0].get_string() == "p");

}

void AliasTest::testDefaults()
{
    // Test for some really common defaults that should never go away
    std::vector< soar::Token > p;
    p.push_back(soar::Token(text_location(), text_location(), "p"));
    CPPUNIT_ASSERT(aliases->Expand(p));
    CPPUNIT_ASSERT(p.size() == 1);
    CPPUNIT_ASSERT(p.front().get_string() == "print");

    std::vector< soar::Token > q;
    q.push_back(soar::Token(text_location(), text_location(), "?"));
    CPPUNIT_ASSERT(aliases->Expand(q));
    CPPUNIT_ASSERT(q.size() == 1);
    CPPUNIT_ASSERT(q.front().get_string() == "help");

    std::vector< soar::Token > init;
    init.push_back(soar::Token(text_location(), text_location(), "init"));
    CPPUNIT_ASSERT(aliases->Expand(init));
    CPPUNIT_ASSERT(init.size() == 1);
    CPPUNIT_ASSERT(init.front().get_string() == "init-soar");

    std::vector< soar::Token > varprint;
    varprint.push_back(soar::Token(text_location(), text_location(), "varprint"));
    CPPUNIT_ASSERT(aliases->Expand(varprint));
    CPPUNIT_ASSERT(varprint.size() == 4);
    CPPUNIT_ASSERT(varprint[0].get_string() == "print");
    CPPUNIT_ASSERT(varprint[1].get_string() == "-v");
    CPPUNIT_ASSERT(varprint[2].get_string() == "-d");
    CPPUNIT_ASSERT(varprint[3].get_string() == "100");

    std::vector< soar::Token > step;
    step.push_back(soar::Token(text_location(), text_location(), "step"));
    CPPUNIT_ASSERT(aliases->Expand(step));
    CPPUNIT_ASSERT(step.size() == 2);
    CPPUNIT_ASSERT(step[0].get_string() == "run");
    CPPUNIT_ASSERT(step[1].get_string() == "-d");

    std::vector< soar::Token > d;
    d.push_back(soar::Token(text_location(), text_location(), "d"));
    CPPUNIT_ASSERT(aliases->Expand(d));
    CPPUNIT_ASSERT(d.size() == 2);
    CPPUNIT_ASSERT(d[0].get_string() == "run");
    CPPUNIT_ASSERT(d[1].get_string() == "-d");

    std::vector< soar::Token > e;
    e.push_back(soar::Token(text_location(), text_location(), "e"));
    CPPUNIT_ASSERT(aliases->Expand(e));
    CPPUNIT_ASSERT(e.size() == 2);
    CPPUNIT_ASSERT(e[0].get_string() == "run");
    CPPUNIT_ASSERT(e[1].get_string() == "-e");

    std::vector< soar::Token > stop;
    stop.push_back(soar::Token(text_location(), text_location(), "stop"));
    CPPUNIT_ASSERT(aliases->Expand(stop));
    CPPUNIT_ASSERT(stop.size() == 1);
    CPPUNIT_ASSERT(stop.front().get_string() == "stop-soar");

    std::vector< soar::Token > interrupt;
    interrupt.push_back(soar::Token(text_location(), text_location(), "interrupt"));
    CPPUNIT_ASSERT(aliases->Expand(interrupt));
    CPPUNIT_ASSERT(interrupt.size() == 1);
    CPPUNIT_ASSERT(interrupt.front().get_string() == "stop-soar");

    std::vector< soar::Token > w;
    w.push_back(soar::Token(text_location(), text_location(), "w"));
    CPPUNIT_ASSERT(aliases->Expand(w));
    CPPUNIT_ASSERT(w.size() == 1);
    CPPUNIT_ASSERT(w.front().get_string() == "watch");
}

void AliasTest::testSimpleCommand()
{
    cli::Parser parser;
    soar::tokenizer tok;
    tok.set_handler(&parser);

    class TestCommand : public cli::ParserCommand
    {
        public:
            virtual ~TestCommand() {}

            virtual const char* GetString() const
            {
                return "test";
            }
            virtual const char* GetSyntax() const
            {
                return "Syntax";
            }

            virtual bool Parse(std::vector<soar::Token>& argv)
            {
                CPPUNIT_ASSERT(argv.size() == 3);
                CPPUNIT_ASSERT(argv[0].get_string() == "test");
                CPPUNIT_ASSERT(argv[1].get_string() == "one");
                CPPUNIT_ASSERT(argv[2].get_string() == "two");
                return true;
            }
    };
    parser.AddCommand(new TestCommand());

    tok.evaluate("test one two");
}
