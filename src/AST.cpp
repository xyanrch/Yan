#include "AST.h"
#include "symbol.h"
namespace Yan
{

    // void BinaryOp::accept(Ivisitor* v)
    // {
    //     v->visit(this);
    // }
     int CaseDefaltStmt::seq = 0;
    CompousedStmt::CompousedStmt()
    {
    }
    CompousedStmt *CompousedStmt::create()
    {
        return new CompousedStmt();
    }
    void CompousedStmt::addStmt(Stmt *stmt)
    {
        stmtlist_.push_back(stmt);
    }
    void CompousedStmt::accept(Ivisitor *v)
    {
        v->visit(this);
    }

    CompousedStmt::~CompousedStmt()
    {
    }

    BinaryOp::BinaryOp(OpType op, Expr *left, Expr *right, Type *ty) : Expr(ty)
    {
        this->op = op;
        this->left = left;
        this->right = right;
    }
    BinaryOp::~BinaryOp()
    {
    }
    void BinaryOp::accept(Ivisitor *v)
    {
        v->visit(this);
    }
    BinaryOp *BinaryOp::create(OpType op, Expr *left, Expr *right, Type *ty)
    {
        return new BinaryOp(op, left, right, ty);
    }

    ConditionExpr *ConditionExpr::create(Expr *cond, Expr *then, Expr *els)
    {
        return new ConditionExpr(cond, then, els);
    }
    int FunctionDef::getStackSize()
    {
        auto sc = this->body_->scope_;
        if (sc)
        {
            return sc->getTyepSize();
        }
        return 0;
    }

} // namespace Yan