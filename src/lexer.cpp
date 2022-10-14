#include "lexer.h"
#include <string.h>
namespace Yan
{
    lexer::lexer(std::string &filename) : loc(filename, 1, 0)
    {
        infile.open(filename.c_str(), std::ios_base::in);
        if (!infile.is_open())
        {
            ExitWithError("Fail to open file:%s", filename.c_str());
        }
    }

    int lexer::next()
    {
        int c = infile.get();

        loc.colum++;
        if (c == '\n')
        {
            loc.line++;
            loc.colum = 0;
        }
        return c;
    }
    void lexer::skipcomment()
    {
        char c = next();
        if (c == '*')
        {
            int pre;
            while (1)
            {
                pre = next();
                if (pre == EOF)
                {
                    ExitWithError("Expect end of file");
                }
                if (peek() == '/' && pre == '*')
                {
                    consume('/');
                    break;
                }
            }
        }
        else if (c == '/')
        {
            int ch = peek();
            int i = 0;
            while (ch != '\n' && ch != EOF)
            {
                consume(ch);

                ch = peek();
            }
        }
    }

    void lexer::scan(Token *t)
    {
        //skip the the charecter that we didn't care about
        int c = next();
        while (c == '\t' || c == ' ' || c == '\n' || c == '\r' || c == '\f' || (c == '\\' && peek() == '\n'))
        {
            c = next();
        }
        if (c == EOF)
        {
            t->type = TokenType::T_EOF;
            return;
        }
        if (ispunct(c))
        {
            scanpunct(c, t);
            return;
        }
        if (isdigit(c) || c == '_')
        {
            scanInt(c, t);
            return;
        }
        if (isalpha(c))
        {
            scanIdenti(c, t);
            return;
        }

        ExitWithError(loc, "unexpect charector:%c", c);
    }
    void lexer::scanpunct(char c, Token *t)
    {

        switch (c)
        {
        case '+':
            if ('+' == peek())
            {
                consume('+');
                t->type = TokenType::T_INC;
            }
            else if ('=' == peek())
            {
                consume('=');
                t->type = TokenType::T_ASPLUS;
            }
            else
            {
                t->type = TokenType::T_ADD;
            }
            return;
        case '-':
            if ('-' == peek())
            {
                consume('-');
                t->type = TokenType::T_DEC;
            }
            else if ('=' == peek())
            {
                consume('=');
                t->type = TokenType::T_ASMINUS;
            }
            else if ('>' == peek())
            {
                consume('>');
                t->type = TokenType::T_ARROW;
            }
            else
            {
                t->type = TokenType::T_MINUS;
            }
            return;
        case '*':
            if ('=' == peek())
            {
                consume('=');
                t->type = TokenType::T_ASSTAR;
            }
            else
            {
                t->type = TokenType::T_STAR;
            }
            return;
        case '.':
            if(peek() == '.')
            {
                consume('.');
                consume('.');
                t->type = TokenType::T_ELLIPSIS;
            }
            else
            {
               t->type = TokenType::T_DOT;
            }                
            return;
        case '%':
            if ('=' == peek())
            {
                consume('=');
                t->type = TokenType::T_ASMOD;
            }
            else
            {
                t->type = TokenType::T_PERCENT;
            }
            break;
        case '[':
            t->type = TokenType::T_LBRACKET;
            return;
        case ']':
            t->type = TokenType::T_RBRACKET;
            return;
        case '{':
            t->type = TokenType::T_LBRACE;
            return;
        case '}':
            t->type = TokenType::T_RBRACE;
            return;
        case '(':
            t->type = TokenType::T_LPAREN;
            return;
        case ')':
            t->type = TokenType::T_RPAREN;
            return;
        case ';':
            t->type = TokenType::T_SEMI;
            return;
        case ',':
            t->type = TokenType::T_COMMA;
            return;
        case ':':
            t->type = TokenType::T_COLON;
            return;
        case '\'':
            t->text = scanchar();
            t->type = TokenType::T_INTLIT;
            consume('\'');
            return;
        case '"':

            t->type = TokenType::T_STRLIT;
            t->text = scanstr();
            return;
        case '/':

            if (peek() == '*' || peek() == '/')
            {
                skipcomment();
                t->type = TokenType::T_COMMENTS;
            }
            else if ('=' == peek())
            {
                consume('=');
                t->type = TokenType::T_ASSLASH;
            }
            else
            {
                t->type = TokenType::T_SLASH;
            }
            return;
        case '|':
            if ('|' == peek())
            {
                consume('|');
                t->type = TokenType::T_LOGOR;
            }
            else
            {
                t->type = TokenType::T_BITOR;
            }
            return;
        case '&':
            if ('&' == peek())
            {
                consume('&');
                t->type = TokenType::T_LOGAND;
            }
            else
            {
                t->type = TokenType::T_AMPER;
            }
            return;
        case '~':
            t->type = TokenType::T_INVERT;
            return;
        case '^':
            t->type = TokenType::T_BITXOR;
            return;
        case '=':
            if (peek() == '=')
            {
                consume('=');
                t->type = TokenType::T_EQ;
            }
            else
            {
                t->type = TokenType::T_ASSIGN;
            }
            return;

        case '>':

            if (peek() == '=')
            {
                consume('=');
                t->type = TokenType::T_GE;
            }
            else if (peek() == '>')
            {
                consume('>');
                t->type = TokenType::T_RSHIFT;
            }
            else
            {
                t->type = TokenType::T_GT;
            }

            return;
        case '<':

            if (peek() == '=')
            {
                consume('=');
                t->type = TokenType::T_LE;
            }
            else if (peek() == '<')
            {
                consume('<');
                t->type = TokenType::T_LSHIFT;
            }
            else
            {
                t->type = TokenType::T_LT;
            }

            return;
        case '!':
            if (peek() == '=')
            {

                consume('=');
                t->type = TokenType::T_NE;
            }
            else
            {
                t->type = TokenType::T_LOGNOT;
            }
            return;
        default:
            ExitWithError(loc, "unexpected charecter %c", c);
        }
    }
    std::string lexer::scanstr()
    {
        char str[MAX_STR_LEN];
        char c = next();
        int i = 0;
        while (c != '"')
        {
            str[i++] = c;
            if (i >= MAX_STR_LEN)
            {
                ExitWithError(loc, "string too long");
            }
            c = next();
        }
        return std::string(str, i);
    }
    int lexer::scanchar()
    {
        int c = next();
        int c2, i;
        if (c == '\\')
        {
            switch (c = next())
            {
            case 'a':
                return ('\a');
            case 'b':
                return ('\b');
            case 'f':
                return ('\f');
            case 'n':
                return ('\n');
            case 'r':
                return ('\r');
            case 't':
                return ('\t');
            case 'v':
                return ('\v');
            case '\\':
                return ('\\');
            case '"':
                return ('"');
            case '\'':
                return ('\'');
                // Deal with octal constants by reading in
                // characters until we hit a non-octal digit.
                // Build up the octal value in c2 and count
                // # digits in i. Permit only 3 octal digits.
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
                for (i = c2 = 0; isdigit(c) && c < '8'; c = peek())
                {
                    if (++i > 3)
                        break;
                    consume(c);
                    c2 = c2 * 8 + (c - '0');
                }
                return c2;
            case 'x':
                return hexchar();
                break;
            }
        }
        else
        {
            return c;
        }
    }
    int lexer::hexchar()
    {
        //TODO
        return 0;
    }
    void lexer::scanInt(char c, Token *t)
    {
        int num = 0;
        while (isdigit(c))
        {
            int tempnum = c - '0';
            num = num * 10 + tempnum;
            if (isdigit(peek()))
            {
                c = next();
            }
            else
            {
                break;
            }
        }
        int temp;
        temp = peek();
        if (temp == ' ' || ispunct(temp) || temp == '\n' || temp == '\t' || temp == '\r' || temp == '\f' || temp == EOF)
        {
            t->type = TokenType::T_INTLIT;
            t->text = num;
        }
        else
        {
            //  t->sourceLocation.colum = 122;
            //t->sourceLocation.fileName ="p";
            ExitWithError(loc, "error toekn");
            //std::cout<<"ERROR: unknown Token LINE:"<<lineNum<<" "<<temp<<std::endl;
            // return false;
        }
    }

    void lexer::scanIdenti(char c, Token *t)
    {
        char identi[MAX_STR_LEN];
        int len = 0;
        identi[len++] = c;
        char ch;
        while (1)
        {
            if (isalpha(peek()) || isdigit(peek()) || peek() == '_')
            {
                ch = next();

                identi[len++] = ch;
            }

            else
            {
                break;
            }
        }
        identi[len++] = '\0';
        if (len > MAX_STR_LEN)
        {
            ExitWithError(loc, "Identifier too long");
        }
        //    char end = peek();
        //    if(end !=' '|| end !='('|| end ! = ')' || end != ';' || end != ',')
        // if(strcmp(identi,"print")==0)
        // {
        //     t->type = TokenType::T_PRINT;
        // }

        if (keyword(identi, t))
        {
            return;
        }

        t->type = TokenType::T_IDENT;
        t->text = identi;
        // ExitWithError(loc,"unkown toekn:%s")
        //std::cout<<"unkown identi :"<<identi<<std::endl;
        //exit(1);
    }
    bool lexer::keyword(char *s, Token *t)
    {
#define xx(ty, str)              \
    if (strcmp(s, str) == 0)     \
    {                            \
        t->type = TokenType::ty; \
        return true;             \
    }
        KEYWORD(xx)
#undef xx
        return false;
    }

    bool lexer::isdigit(char c)
    {
        if (c >= '0' && c <= '9')
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    bool lexer::isalpha(char c)
    {
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    //single letter punctuation
    bool lexer::ispunct(char c)
    {
        if (c=='|' || c == '+' || c == '-' || c == '*' || c == '/' || c == '<' || c == '>' || c == '!' || c == '=' ||
            c == ',' || c == ';' || c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' ||
            c == '%' || c == ':' || c == '?' || c == '\'' || c == '"' || c == '.' || c == '^' || c == ':' || c == '&')
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    int lexer::peek()
    {
        return infile.peek();
    }
    //consume the match char
    void lexer::consume(char c)
    {
        if (peek() == c)
        {
            next();
        }
        else
        {
            ExitWithError("expected charecter :%c", c);
        }
    }

    void lexer::putBack(const Token &t)
    {
        tokenCache_.push(t);
    }
    Token lexer::getToken()
    {
        if (tokenCache_.empty())
        {
            Token t(loc);
            scan(&t);
            while (t.type == TokenType::T_COMMENTS)
            {
                scan(&t);
            }
            return t;
        }
        else
        {
            auto t = tokenCache_.top();
            tokenCache_.pop();
            return t;
        }
    }
    Token lexer::peektoken()
    {
        if (tokenCache_.empty())
        {
            Token t(loc);
            scan(&t);
            while (t.type == TokenType::T_COMMENTS)
            {
                scan(&t);
            }
            tokenCache_.push(t);
            return t;
        }
        else
        {
            return tokenCache_.top();
        }
    }
    lexer::~lexer()
    {
        if (infile.is_open())
        {
            infile.close();
        }
    }

}; // namespace Yan
