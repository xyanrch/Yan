#include"gtest/gtest.h"
#include<fstream>
#include "../src/lexer.h"
#include<iostream>

 static std::string inputfile = "test.c";
class lexerTest :public testing::Test {

};
TEST(lexerTest, testToken)
{
  using namespace Yan;
  Token t;
  t.text = "hhhhh";
  t.type = TokenType::T_IDENT;
  std::cout<<t;
}
TEST(lexerTest, test_token_cache)
{
  std::ofstream f(inputfile);
  f<<"int main; long a; a += 1; a /=2; -=,*= b->x";
  f.flush();
  f.close();
  using namespace Yan;
  lexer l(inputfile);
  Token t;
  auto t1 = l.getToken();
  ASSERT_TRUE(t1.tostring() == "int");
  l.putBack(t1);
  auto t2 = l.getToken();
  ASSERT_TRUE(t2.tostring() == "int");
  auto t3 = l.getToken();
  ASSERT_TRUE(t3.tostring() == "Identi: main");
  l.putBack(t3);
  ASSERT_TRUE(l.getToken().tostring() == "Identi: main");
  auto t11 = l.getToken();
  auto t12 = l.getToken();
  auto t13 = l.getToken();

  l.putBack(t13);
  l.putBack(t12);
  l.putBack(t11);
  ASSERT_TRUE(l.getToken().tostring() == ";");
  ASSERT_TRUE(l.getToken().tostring() == "long");
  ASSERT_TRUE(l.getToken().tostring() == "Identi: a");
}
TEST(lexerTest, test_scan_program)
{
  std::string yy = "yy.c";
  std::ofstream f("yy.c");
  f<<"int main () {\n"
  " int i = 10, b = 20, c= 30;\n"
   "i = b, c;\n"
   "i%=8;b = b^c;\n"
   "if(!a)\n"
   "printf(\"%i\\n\", i);\n"

  " i = (b, c);\n"
   "printf(\"%i\\n\", i);\n"
  "}\n";

  f.flush();
  f.close();
  using namespace Yan;
  lexer l(yy);
  Token t;
  t = l.getToken();
  std::vector<Token> tlist;
  std::vector<std::string> expect_token{
" int"  , 
" Identi: main"  , " ("  , " )"  , " {"  , " int"  , 
" Identi: i"  , " ="  , " intlit: 10"  , " ,"  , " Identi: b"  , 
" ="  , " intlit: 20"  , " ,"  , " Identi: c"  , " ="  , 
" intlit: 30"  , " ;"  , " Identi: i"  , " ="  , " Identi: b"  , 
" ,"  , " Identi: c"  , " ;"  , " Identi: i"  , " %="  , 
" intlit: 8"  , " ;"  , " Identi: b"  , " ="  , " Identi: b"  , 
" ^"  , " Identi: c"  , " ;"  , " if"  , " ("  , 
" !"  , " Identi: a"  , " )"  , " Identi: printf"  , " ("  , 
" strlit: %i\\n"  , " ,"  , " Identi: i"  , " )"  , " ;"  , 
" Identi: i"  , " ="  , " ("  , " Identi: b"  , " ,"  , 
" Identi: c"  , " )"  , " ;"  , " Identi: printf"  , " ("  , 
" strlit: %i\\n"  , " ,"  , " Identi: i"  , " )"  , " ;"  , 
" }"
  };
 
  while(t.type !=TokenType::T_EOF)
  {
    tlist.push_back(t); 
  //  std::cout<<t.getLocation().fileName<<":"<<t.getLocation().line<<",  " <<std::endl;  
    t = l.getToken();
  }

  ASSERT_TRUE(tlist.size()== expect_token.size());

  for (int i= 0;i<tlist.size();i++)
  {
    std::string wrapper = " "+tlist[i].tostring();
    ASSERT_TRUE(wrapper == expect_token[i]);
  }



}
TEST(lexerTest, test_basic)
{
  std::ofstream f(inputfile);
  f<<"int main; long a; a += 1; a /=2; -=,*= b->x";
  f.flush();
  f.close();
  using namespace Yan;
  lexer l(inputfile);
  Token t;
  l.scan(&t);
  std::vector<Token> tlist;
  std::vector<TokenType>expect {TokenType::T_INT,TokenType::T_IDENT,TokenType::T_SEMI,
  TokenType::T_LONG,TokenType::T_IDENT,TokenType::T_SEMI,TokenType::T_IDENT,
  TokenType::T_ASPLUS,TokenType::T_INTLIT,TokenType::T_SEMI,TokenType::T_IDENT,TokenType::T_ASSLASH,
  TokenType::T_INTLIT,TokenType::T_SEMI, TokenType::T_ASMINUS, TokenType::T_COMMA,TokenType::T_ASSTAR,
  TokenType::T_IDENT,TokenType::T_ARROW,TokenType::T_IDENT};
  while(t.type !=TokenType::T_EOF)
  {
    tlist.push_back(t);
    Info(t.tostring().c_str());
    
    l.scan(&t);
  }
  ASSERT_TRUE(tlist.size()== expect.size());

  for (int i= 0;i<tlist.size();i++)
  {
    ASSERT_TRUE(static_cast<uint32_t>(tlist[i].type) == static_cast<uint32_t>(expect[i]));
  }



}
TEST(lexerTest, test_scanchar)
{
  std::ofstream f(inputfile);
  f<<"char c,ff; c = '\b';  ff = 'y'; a ='\101'; char*s = \"hello world\";";
  f.flush();
  f.close();
  using namespace Yan;
  lexer l(inputfile);
  Token t;
  l.scan(&t);
  std::vector<Token> tlist;
  std::vector<TokenType>expect {TokenType::T_CHAR,TokenType::T_IDENT,TokenType::T_COMMA,
  TokenType::T_IDENT,TokenType::T_SEMI,TokenType::T_IDENT,TokenType::T_ASSIGN,TokenType::T_INTLIT,TokenType::T_SEMI,
  TokenType::T_IDENT,TokenType::T_ASSIGN,TokenType::T_INTLIT,TokenType::T_SEMI,TokenType::T_IDENT,
  TokenType::T_ASSIGN,TokenType::T_INTLIT,TokenType::T_SEMI,TokenType::T_CHAR,TokenType::T_STAR,
  TokenType::T_IDENT,  TokenType::T_ASSIGN,TokenType::T_STRLIT,TokenType::T_SEMI};
  while(t.type !=TokenType::T_EOF)
  {
    tlist.push_back(t);
    
    l.scan(&t);
  }
  ASSERT_TRUE(tlist.size()== expect.size());

  for (int i= 0;i<tlist.size();i++)
  {
    
    ASSERT_TRUE(static_cast<uint32_t>(tlist[i].type) == static_cast<uint32_t>(expect[i]));
  }
  ASSERT_TRUE(tlist[7].getValue() == '\b');
  ASSERT_TRUE(tlist[11].getValue() == 'y');  
  ASSERT_TRUE(tlist[15].getValue() == 65); 
  ASSERT_TRUE(tlist[21].getText() == "hello world");


}
TEST(lexerTest, test_skipcomments)
{
  std::ofstream f(inputfile);
  f<<"//int main \n int a;";
  f.flush();
  f.close();
  using namespace Yan;
  lexer l(inputfile);
  Token t;
  l.scan(&t);
  std::vector<Token> tlist;
  std::vector<TokenType>expect {TokenType::T_COMMENTS,TokenType::T_INT,TokenType::T_IDENT,TokenType::T_SEMI};
  while(t.type !=TokenType::T_EOF)
  {
    tlist.push_back(t);
    
    l.scan(&t);
  }
  ASSERT_TRUE(tlist.size()== expect.size());

  for (int i= 0;i<tlist.size();i++)
  {
    ASSERT_TRUE(static_cast<uint32_t>(tlist[i].type) == static_cast<uint32_t>(expect[i]));
  }



}
