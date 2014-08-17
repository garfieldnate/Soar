#include "lexer.h"
#include "lexeme.h"

#include "unittest.h"

#include <fstream>
#include <string>
#include <sstream>
#include <list>

using std::string;
using std::stringstream;
using std::list;
using soar::Lexer;
using soar::Lexeme;

class LexerTest : public CPPUNIT_NS::TestCase
{
  CPPUNIT_TEST_SUITE( LexerTest );
  runAllTests();
  CPPUNIT_TEST_SUITE_END();
public:
  void setUp(){}
  void tearDown() {}
protected:
  /**
   * Reads the lexer test data file and runs each specified test
   */
  void runAllTests();
  /**
   * Runs a single lexer test with the specified data.
   * @param testName       Name of the test
   * @param lexerInput     String to pass to the lexer
   * @param expectedTokens Set of expected Lexeme types; each should be a string representing a Lexeme type, as given by Lexeme::GetTypeName.
   */
  void lexerTest(string* testName, string* lexerInput, list<string>* expectedTokens);
  string listToString(list<string>* tokens);
};

CPPUNIT_TEST_SUITE_REGISTRATION( LexerTest ); // Registers the test so it will be used

void LexerTest::runAllTests(){
  const string blockStarter = "=== ";
  const string inputStarter = "--- input";
  const string tokenStarter = "--- tokens";

  std::ifstream dataFile ("test_agents/lexer_test.txt");
  if(!dataFile.is_open())
  {
    CPPUNIT_ASSERT_MESSAGE("could not open lexer_test.txt", false);
    return;
  }

  //the test file is formatted like this:
  //=== test name
  //--- input
  //strings to tokenize
  //with the lexer
  //--- tokens
  //TOKEN_NAME_1
  //TOKEN_NAME_2
  //=== next test name
  
  //currently reading input block
  bool inInput = false;
  //currently reading tokens block
  bool inTokens = false;
  //current line of the test data file
  string line;

  string testName;
  string lexerInput;
  list<string> tokens;
  while ( getline (dataFile, line) ){
    // cout << line << '\n';
    if(line.compare(0, blockStarter.length(), blockStarter) == 0){
      // run previous test before reading next one
      if(testName.length() != 0)
        lexerTest(&testName, &lexerInput, &tokens);
      // name of the test follows the block starter
      testName = line.substr(4);
      //re-initialize other loop variable
      inInput = false;
      inTokens = false;
      lexerInput = "";
      tokens = list<string>();
    }else if(line.compare(0, inputStarter.length(), inputStarter) == 0){
      inInput = true;
    }else if(line.compare(0, tokenStarter.length(), tokenStarter) == 0){
      inInput = false;
      inTokens = true;
    }else if(inInput){
      lexerInput += line;
    }else if(inTokens){
      tokens.push_back(line);
    }
  }
  //for the last test in the input file
  if(testName.length() != 0)
    lexerTest(&testName, &lexerInput, &tokens);
}

void LexerTest::lexerTest (string* testName, string* lexerInput, list<string>* expectedTokens ){
  Lexer lexer(NULL, lexerInput->c_str());
  
  //create the list of actual tokens found in the input
  list<string> tokens;
  lexer.get_lexeme();
  while(lexer.current_lexeme.type != EOF_LEXEME){
    tokens.push_back(string(Lexeme::GetTypeName(lexer.current_lexeme.type)));
  }
  
  //compare them with the expected tokens
  CPPUNIT_ASSERT_MESSAGE(
    "expected: " + listToString(expectedTokens) +
      ", found: " + listToString(&tokens),
    tokens == *expectedTokens
  );
}

string LexerTest::listToString(list<string>* tokens){
  list<string>::const_iterator i;
  stringstream s;
  for( i = tokens->begin(); i != tokens->end(); ++i)
    s << *i << " ";
  return s.str();
}