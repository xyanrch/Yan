#ifndef _TOKEN_H_
#define _TOKEN_H_
#include<string>
#include<variant>
#include<iostream>
namespace Yan{

//max str len for one token

constexpr int MAX_STR_LEN = 100;
class Token;
#define KEYWORD(xx)         \
    xx(T_VOID , "void") \
    xx(T_CHAR,     "char") \
     xx(T_INT,      "int" ) \
     xx(T_LONG,     "long") \
     xx(T_SHORT,    "short")\
     xx(T_ENUM,     "enum") \
     xx(T_STRUCT,   "struct")\
     xx(T_UNION,    "union")\
     xx(T_BOOL,     "bool")\
     xx(T_EXTERN,   "extern")\
     xx(T_STATIC,   "static")\
     xx(T_TYPDEF,   "typedef")\
     xx(T_SIGNED,   "signed")\
     xx(T_UNSIGNED,  "unsigned")\
     xx(T_IF,       "if")\
     xx(T_ELSE,     "else")\
     xx(T_RETURN,   "return")\
     xx(T_SWITCH,   "switch")\
     xx(T_CASE,     "case")\
     xx(T_BREAK,    "break")\
     xx(T_CONTINUE, "continue")\
     xx(T_DEFALT,   "default")\
     xx(T_WHILE,    "while")\
     xx(T_FOR,      "for")\
     xx(T_DO,       "do")\
     xx(T_GOTO,      "goto")
    

#define RESERVED(xx)        \
    xx(T_ASSIGN,        "=") \
    xx(T_ASPLUS,        "+=") \
    xx(T_ASMINUS,       "-=")\
    xx(T_ASSTAR,        "*=")\
    xx(T_ASSLASH,       "/=")\
    xx(T_ASMOD,         "%=")\
    xx(T_ANDASSIGN,      "&=")\
    xx(T_ORASSIGN,       "|=" )\
    xx(T_XORASSIGN,     "^=")\
    xx(T_LSHIFASSIGN,    "<<=")\
    xx(T_RSHIFTASSIGN,  ">>=")\
    xx(T_QUSTION,       "?")\
    xx(T_ADD,            "+")\
    xx(T_MINUS,         "-")\
    xx(T_STAR,          "*")\
    xx(T_SLASH,          "/")\
    xx(T_PERCENT,        "%")\
    xx(T_INC,           "++")\
    xx(T_DEC,           "--")\
    xx(T_EQ,            "==")\
    xx(T_GT,            ">")\
    xx(T_LT,            "<")\
    xx(T_LE,            "<=")\
    xx(T_GE,            ">=")\
    xx(T_NE,            "!=")\
    xx(T_LSHIFT,         "<<")\
    xx(T_RSHIFT,         ">>")\
    xx(T_BITOR,         "|")\
    xx(T_AMPER,         "&")\
    xx(T_BITXOR,        "^")\
    xx(T_INVERT,        "~")\
    xx(T_ARROW,         "->")\
    xx(T_DOT,           ".")\
    xx(T_LOGAND,        "&&")\
    xx(T_LOGOR,         "||")\
    xx(T_LOGNOT,        "!")\
    xx(T_LBRACE,        "{")\
    xx(T_RBRACE,        "}")\
    xx(T_LPAREN,        "(")\
    xx(T_RPAREN,        ")")\
    xx(T_LBRACKET,      "[")\
    xx(T_RBRACKET,      "]")\
    xx(T_COMMA,         ",")\
    xx(T_SEMI,          ";")\
    xx(T_COLON,         ":")\
    xx(T_ELLIPSIS,      "...")





enum class TokenType 
{ 
#define xx(type, text) type,
    KEYWORD(xx)
    RESERVED(xx)
#undef xx

    T_EOF,
    T_INTLIT,
    T_STRLIT,
    T_PRINT,
    
    //other
    T_COMMENTS,
    T_IDENT,
 
    T_UNKOWN_TOKEN
};

struct location
{
    std::string fileName;
    int line;
    int colum;
    location():fileName(""),line(0),colum(0){}
    location(const std::string& file, int line, int colum)
    {
        this->fileName = file;
        this->line = line;
        this->colum = colum;
    }
    
};
 class Token 
{   

public:
    Token() = default;
    Token(const location& l):loc(l){}
    TokenType type;
       // store identifier name or num litri
    std::variant<int,std::string> text;
    std::string  tostring()const;
    std::string getText()const ;
    int getValue()const;
    const location& getLocation();
    friend std::ostream& operator << (std::ostream& s, const Token& t);
   
private:
     
    location loc;

};
extern std::ostream& operator << (std::ostream& s, const Token& t);


}

#endif
