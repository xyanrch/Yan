#ifndef _AST_H_
#define _AST_H_
#include <memory>
#include <list>
#include <vector>
#include "lexer.h"
#include "type.h"
//#include "symbol.h"

namespace Yan
{
    class Scope;

    struct Expr;
    struct BinaryOp;
    struct Identifier;
    struct IntegerLiteral;
    struct StringLiteral;
    struct FunctionDef;
    struct Declaration;
    struct Program;
    struct LoopStmt;
    struct IfStmt;
    struct CompousedStmt;
    struct FunctionCall;
    struct GotoStmt;
    struct ReturnStmt;
    struct UnaryOp;
    struct ConditionExpr;
    struct BreakContinueStmt;
    struct LabelStmt;
    struct ExprStmt;
    struct Enumerator;
    struct SwitchStmt;
    struct CaseDefaltStmt;

    class Ivisitor
    {
    public:
        virtual void visit(BinaryOp *node) = 0;
        virtual void visit(Identifier *node) = 0;
        virtual void visit(IntegerLiteral *node) = 0;
        virtual void visit(StringLiteral *node) = 0;
        virtual void visit(FunctionDef *node) = 0;
        virtual void visit(Declaration *node) = 0;
        virtual void visit(Program *node) = 0;
        virtual void visit(IfStmt *node) = 0;
        virtual void visit(LoopStmt *node) = 0;
        virtual void visit(CompousedStmt *node) = 0;
        virtual void visit(FunctionCall *node) = 0;
        virtual void visit(GotoStmt *node) = 0;
        virtual void visit(ReturnStmt *node) = 0;
        virtual void visit(UnaryOp *node) = 0;
        virtual void visit(ConditionExpr *node) = 0;
        virtual void visit(BreakContinueStmt *node) = 0;
        virtual void visit(LabelStmt *node) = 0;
        virtual void visit(ExprStmt *node) = 0;
        virtual void visit(Enumerator *node) = 0;
        virtual void visit(SwitchStmt *node) = 0;
        virtual void visit(CaseDefaltStmt *node) = 0;
    };

    struct Node
    {
        Node(){};
        virtual void accept(Ivisitor *v) = 0;
        virtual ~Node(){};
    };
    //
    struct Stmt : public Node
    {
        Stmt(){};
        virtual ~Stmt(){};
    };

    using StmtList = std::list<Stmt *>;
    struct CompousedStmt : public Stmt
    {
        CompousedStmt();
        virtual ~CompousedStmt();

        static CompousedStmt *create();
        void addStmt(Stmt *stmt);
        virtual void accept(Ivisitor *v) override;

        StmtList stmtlist_;
        Scope *scope_ = nullptr;
    };

    struct IfStmt : public Stmt
    {
        IfStmt(Expr *cond, Stmt *then, Stmt *els = nullptr)
            : cond_(cond), then_(then), else_(els) {}
        static IfStmt *create(Expr *cond, Stmt *then, Stmt *els = nullptr)
        {
            return new IfStmt(cond, then, els);
        }
        virtual void accept(Ivisitor *v) override { v->visit(this); }
        Expr *cond_;
        Stmt *then_;
        Stmt *else_;
    };
    // 'while do-while' and 'for' Stmt
    struct LoopStmt : public Stmt
    {
        LoopStmt(Expr *cond, Stmt *then, bool postcheck)
            : cond_(cond), body_(then), postcheck_(postcheck) {}
        static LoopStmt *create(Expr *cond, Stmt *then, bool postcheck = false)
        {
            return new LoopStmt(cond, then, postcheck);
        }
        virtual void accept(Ivisitor *v) override { v->visit(this); }

        ExprStmt *inc_ = nullptr; //only for 'for loop'

        Expr *cond_;
        Stmt *body_;
        // true for do-while
        bool postcheck_;
    };

    struct BreakContinueStmt : public Stmt
    {
        enum kind
        {
            kBreak,
            kcontinue
        };
        BreakContinueStmt(kind k) : kind_(k) {}
        static BreakContinueStmt *create(kind k) { return new BreakContinueStmt(k); }
        virtual void accept(Ivisitor *v) override { v->visit(this); }

        kind kind_;
    };
    struct ReturnStmt : public Stmt
    {
        ReturnStmt(Expr *expr) : expr_(expr) {}
        static ReturnStmt *create(Expr *expr) { return new ReturnStmt(expr); }
        virtual void accept(Ivisitor *v) override { v->visit(this); }
        Expr *expr_;
    };
    struct GotoStmt : public Stmt
    {
        GotoStmt(std::string label) : label_(label) {}
        static GotoStmt *create(std::string label) { return new GotoStmt(label); }
        virtual void accept(Ivisitor *v) { v->visit(this); }

        std::string label_;
    };
    struct LabelStmt : public Stmt
    {
        LabelStmt(std::string label) : label_(label) {}
        static LabelStmt *create(std::string label) { return new LabelStmt(label); }
        virtual void accept(Ivisitor *v) override { v->visit(this); }
        std::string label_;
    };
    struct SwitchStmt : public Stmt
    {
        static SwitchStmt *create() { return new SwitchStmt; }
        virtual void accept(Ivisitor *v) override { v->visit(this); }
        std::vector<CaseDefaltStmt *> caseStmts_;
    };
    struct CaseDefaltStmt : public Stmt
    {
        static int seq;
        static std::string genCaseLabel()
        {
            return "case_lable_"+std::to_string(seq++);
        }
        static CaseDefaltStmt *create(Expr *exp, bool isdefault, int caseValue)
        {
            return new CaseDefaltStmt(exp, isdefault, caseValue);
        }
        CaseDefaltStmt(Expr *exp, bool isdefault, int caseValue) : switchExp_(exp), isdefaultLable_(isdefault), caseValue_(caseValue)
        {
            lable_ = CaseDefaltStmt::genCaseLabel();
        }
        virtual void accept(Ivisitor *v) override
        {
            v->visit(this);
        }
        Expr *switchExp_ = nullptr;
        bool isdefaultLable_ = false;
        int caseValue_ = 0;
        std::vector<Stmt *> stmts_;
        std::string lable_;
    };
   
    struct ExprStmt : public Stmt
    {
        ExprStmt(Expr *exp) : expr_(exp) {}
        static ExprStmt *create(Expr *exp) { return new ExprStmt(exp); }
        virtual ~ExprStmt(){};
        virtual void accept(Ivisitor *v) { v->visit(this); }
        Expr *expr_;
    };
    struct Expr : public Node
    {
        Expr(Type *ty) : type_(ty) {}
        Expr() : type_(nullptr) {}
        virtual bool isLvalue() { return false; }
        virtual ~Expr(){};
        Type *type_; //Todo ower the type
    };
    enum class OpType
    {
        //BinaryOP
        OP_ADD,
        OP_PTRADD,
        OP_SUBTRACT,
        OP_PTRSUB,
        OP_MULTIPLY,
        OP_DIVIDE,
        OP_MOD,
        OP_ASSIGN,
        OP_ASSPLUS,   //+=
        OP_ASSSUB,    // -=
        OP_ASSMUL,    //*=
        OP_ASSDIV,    // /=
        OP_ASSMOD,    // %=
        OP_SHLASSIGN, // <<=
        OP_SHRASSIGN,
        OP_ANDASSIGN,
        OP_ORASSIGN,
        OP_XORASSIGN,
        OP_LSHIFT, //<<
        OP_RSHIFT, //>>

        OP_COMMA, // ,

        OP_EQ,
        OP_NE,
        OP_GT,
        OP_LT,
        OP_GE,
        OP_LE,

        OP_LOGICAND,
        OP_LOGICOR,
        OP_BITOR,
        OP_BITXOR,
        OP_BITAND,
        OP_DOT, //.

        //UnaryOP
        OP_CAST,
        OP_ADDR,
        OP_DEREF,
        OP_POSTINC,
        OP_PREINC,
        OP_POSTDEC,
        OP_PREDEC,
        OP_NOT,

        OP_UNKOWN
    };
    struct BinaryOp : public Expr
    {

        //svirtual void accept(Ivisitor*v);
        BinaryOp(OpType op, Expr *left, Expr *right, Type *ty);
        virtual ~BinaryOp();
        virtual void accept(Ivisitor *v) override;
        static BinaryOp *create(OpType op, Expr *left, Expr *right, Type *ty = nullptr);

        OpType op;
        Expr *left;
        Expr *right;
    };
    /*
// unary = ("+" | "-" | "*" | "&" | "!" | "~"|".")? cast

//       | ("++" | "--") unary

//       | postfix
*/
    struct UnaryOp : public Expr
    {
        UnaryOp(OpType op, Expr *operand, Type *ty) : Expr(ty), op_(op), operand_(operand) {}
        static UnaryOp *create(OpType op, Expr *operand, Type *ty = nullptr) { return new UnaryOp(op, operand, ty); }
        virtual void accept(Ivisitor *v) { v->visit(this); }
        virtual bool isLvalue() { return true; }

        OpType op_;
        Expr *operand_;
    };
    struct ConditionExpr : public Expr
    {
        ConditionExpr(Expr *cond, Expr *then, Expr *els)
            : cond_(cond), trueExpr_(then), falseExpr_(els) {}
        virtual void accept(Ivisitor *v) override { v->visit(this); }
        static ConditionExpr *create(Expr *cond, Expr *then, Expr *els);
        Expr *cond_;
        Expr *trueExpr_;
        Expr *falseExpr_;
    };
    //type longlong int short
    struct IntegerLiteral : public Expr
    {
        IntegerLiteral(int value) : value_(value) {}
        static IntegerLiteral *create(int value) { return new IntegerLiteral(value); }
        virtual void accept(Ivisitor *v) override { v->visit(this); }
        int value_;
    };
    struct StringLiteral : public Expr
    {

        StringLiteral(const std::string &data) : strData_(data) {}
        static StringLiteral *create(const std::string &data) { return new StringLiteral(data); }
        virtual void accept(Ivisitor *v) { v->visit(this); }
        // const char *strData_;
        //int byteLen_;
        std::string strData_;
    };

    struct FunctionCall : public Expr
    {
        FunctionCall(Identifier *designator) : designator_(designator) {}
        static FunctionCall *create(Identifier *designator) { return new FunctionCall(designator); }
        void addArg(Expr *arg) { argList_.push_back(arg); }
        virtual void accept(Ivisitor *v) override { v->visit(this); }
        Identifier *designator_;
        std::vector<Expr *> argList_;
    };
    //represent a varaible
    struct Identifier : public Expr
    {

        std::string name_;
        storageClass class_ = storageClass::UNKNOW;
        bool isLocal_;
        //only for local varibale
        int offset_ = 0;
        virtual void accept(Ivisitor *v) override { v->visit(this); }
        void setoffset(int offset) { offset_ = offset; }
        Identifier(const std::string &name, Type *type, bool islocal) : Expr(type), name_(name), isLocal_(islocal) {}
        static Identifier *clone(Identifier *old)
        {
            auto newnode = new Identifier(old->name_, old->type_, old->isLocal_);
            newnode->offset_ = old->offset_;
            newnode->class_ = old->class_;
            return newnode;
        }
        static Identifier *create(const std::string &name, Type *type, bool islocal) { return new Identifier(name, type, islocal); }
        virtual bool isLvalue() { return true; }
    };
    struct Enumerator : public Identifier
    {
        static Enumerator *create(const std::string &name, int val)
        {
            return new Enumerator(name, val);
        }
        virtual void accept(Ivisitor *v) override { v->visit(this); }
        Enumerator(const std::string &name, int val) : Identifier(name, int_type, false)
        {
            value = val;
        }
        int value;
    };

    //using ExtDecl = Node;
    struct FunctionDef : Node
    {
        FunctionDef(Identifier *identi) : identi_(identi) {}
        virtual ~FunctionDef() override {}
        virtual void accept(Ivisitor *v) override { v->visit(this); }
        static FunctionDef *create(Identifier *identi) { return new FunctionDef(identi); }
        void setBody(CompousedStmt *body) { body_ = body; }
        int getStackSize();
        Identifier *identi_;
        //LabelStmt* returnLebl_;
        CompousedStmt *body_;
    };
    struct Initializer
    {
        Initializer(Type *ty, int offset, Expr *exp) : type_(ty),
                                                       offset_(offset),
                                                       expr_(exp)
        {
        }

        Type *type_;
        int offset_;
        Expr *expr_;
    };
    //
    struct Declaration : public Stmt
    {
        using InitList = std::list<Initializer>;
        Declaration(Identifier *identi) : obj_(identi) {}
        virtual void accept(Ivisitor *v) override { v->visit(this); }
        static Declaration *create(Identifier *identi) { return new Declaration(identi); }
        Identifier *obj_;
        //a {1,2,4}
        InitList inits_;
    };

    using ExtDeclList = std::vector<Node *>;
    struct Program : Node
    {
        Program() {}
        virtual ~Program() {}
        void accept(Ivisitor *v) override { v->visit(this); }
        static Program *create() { return new Program(); }
        void add(Node *decl) { decls_.push_back(decl); }

        ExtDeclList decls_;
    };

}; // namespace Yan
#endif
