#ifndef _PARSER_H_
#define _PARSER_H_
#include <map>
#include "lexer.h"
#include "AST.h"
#include "log.h"
#include "symbol.h"
#include "evaluator.h"
#include "util.h"
namespace Yan
{

    // + - * / intlitr EOF
    using Declarator = std::pair<Type *, std::string>;

    class parser
    {

    public:
        parser(lexer &s);
        ~parser();
        //Begin a block scope;
        void enterScope(ScopeKind kind);
        void leaveScope();
        Scope *getScope() { return currentScop_; }

        Program *parseProgram();

        //Statments
        Stmt *parseSingleStmt();
        CompousedStmt *parseCompoundStmt();
        FunctionDef *parseFuncDef();
        FunctionCall *parseFuncCall(Identifier* identi);
        IfStmt *parseIfStmt();
        LoopStmt *parseWhileStmt();
        Stmt *parseForStmt();
        LoopStmt *parseDoWhileStmt();
        BreakContinueStmt *parseContinueBreakStmt(BreakContinueStmt::kind type);
        ReturnStmt* parseReturnStmt();
        SwitchStmt* parseSwitchStmt();
        CaseDefaltStmt* parseCaseStmt(bool isdefault,Expr* exp);


        //EXPR
        Expr *expr();
        Expr *assign();
        Expr *conditional();
        Expr *logicOr();
        Expr *logicAnd();
        Expr *bitOr();
        Expr *bitXor();
        Expr *bitAnd();
        Expr *equality();
        Expr *relational();
        Expr *shift();
        Expr *sum();
        Expr *richadd(Expr *lhs, Expr* rhs);
        Expr *richsub(Expr *lhs, Expr* rhs);
        Expr *mul();
        Expr *cast();
        Expr *unary();
        Expr *postfix();
        Expr *structRef(Expr* node);
        Expr *primary();

        //declaration
        void parseDeclaration(bool islocal, Node* node);
        Initializer parseDecInitializer(Type* ty);
        Type *baseType(storageClass *sclass);
        Declarator declarator(Type *type);
        Type *declarator_array(Type *type);
        Type *declarator_func(Type *type);
        Type *type_suffix(Type *type);
        Type *modifyBase(Type *type, Type *base, Type *new_base);
        Type* parseStructDecl();
        Type* parseEnumSpecifier();
        Type* parseUnionSpecifier();
        Type* structUnionDecl();

         StructType::Member  parseStructMember();

        Declarator parse_func_param();

        template <typename T>
        T constExpr()
        {
            evaluator<T> eval;
            return eval.eval(assign());
        }

    private:
        bool isInteger(Expr* node);
        bool isString(Type*ty);
        bool isTypeName();
        bool findtypedef(const std::string &name);
        bool isFuncdef();
        void defineBuildinFunc(std::string name, Type* reType, std::vector<Type*>paramType);
        void addBuildinFunctions();

        //Token ralated function
        bool match(TokenType t);
        bool is(TokenType t) const;//test if the token is t

        bool isOneOf(TokenType K1, TokenType K2) const
        {
            return is(K1) || is(K2);
        }
        template <typename... Ts>
        bool isOneOf(TokenType K1, TokenType K2, Ts... Ks) const
        {
            return is(K1) || isOneOf(K2, Ks...);
        }
        Token consume();
        void putBack(const Token &t) { scan.putBack(t); }
        Token peek() { return scan.peektoken(); }
        void expect(TokenType t, const std::string &what);

    private:
        lexer &scan;
        //Token currenttoken_;//used for error output
        

        Scope *currentScop_;

        //disable copy and assign
        parser(const parser &) = delete;
        parser &operator=(const parser &) = delete;
    };
    //RAII scope
    class selfScope
    {
    public:
        selfScope(parser &p, ScopeKind kind) : self(p)
        {
            self.enterScope(kind);
        }
        ~selfScope()
        {
            self.leaveScope();
        }

    private:
        parser &self;
    };

} // namespace Yan
#endif