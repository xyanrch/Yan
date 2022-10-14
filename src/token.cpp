#include "token.h"
#include <map>
namespace Yan
{

    std::string Token::tostring() const
    {
#define xx(T, Text)    \
    case TokenType::T: \
        return Text;
        switch (type)
        {
            RESERVED(xx)
            KEYWORD(xx)
        case TokenType::T_EOF:
            return "EOF";
        case TokenType::T_IDENT:
        {
            std::string str = "Identi: " + getText();
            return str;
        }
        case TokenType::T_STRLIT:
        {
            std::string str = "strlit: " + getText();
            return str;
        }
        case TokenType::T_INTLIT:
        {
            std::string str = "intlit: " + std::to_string(getValue());
            return str;
        }
        default:
            return "unkown";
        }
#undef xx
    }

    std::string Token::getText()const
    {
        return std::get<std::string>(text);
    }
    int Token::getValue()const
    {
        return std::get<int>(text);
    }

    const location &Token::getLocation()
    {
        return loc;
    }
std::ostream& operator<<(std::ostream& os, const Token& t)
{
    os<<t.loc.fileName<<":"<<t.loc.line<<":"<<t.loc.colum<<" "<<t.tostring();
    return os;
 }
} // namespace Yan