#ifndef _EVALUATOR_H_
#define _EVALUATOR_H_
#include "AST.h"
#include<assert.h>
namespace Yan
{
template<typename T>
class evaluator: public Ivisitor
{
public:
   virtual void visit(BinaryOp* node)override;
   virtual void visit(Identifier* node)override{ assert(0);}
   virtual void visit(IntegerLiteral* node)override ;
   virtual void visit(FunctionDef* node)override{ assert(0);}
   virtual void visit(Declaration* node)override{ assert(0);}
   virtual void visit(Program* node)override{ assert(0);}
   virtual void visit(IfStmt* node)override{ assert(0);}
   virtual void visit(CompousedStmt* node)override{ assert(0);}
   virtual void visit(FunctionCall* node)override{ assert(0);}
   virtual void visit(GotoStmt* node)override { assert(0);}
   virtual void visit(ReturnStmt* node)override{ assert(0);}
   virtual void visit(StringLiteral* node)override{assert(0);}
   virtual void visit(UnaryOp*node)override;
   virtual void visit(ConditionExpr* node)override;
   virtual void visit(LoopStmt* node)override{assert(0);}
   virtual void visit(BreakContinueStmt* node)override{assert(0);}
   virtual void visit(LabelStmt* node)override{assert(0);}
    virtual void visit(ExprStmt* node)override{assert(0);}
    virtual void visit(Enumerator * node)override{}//todo};
    virtual void visit(CaseDefaltStmt *node){}
    virtual void visit(SwitchStmt *node){}


   T eval(Expr* node)
   {
       node->accept(this);
       return result;
   }

   T getResult(){ return result;}

private:
    T result;

};

}
#include "evaluator.cpp"
#endif