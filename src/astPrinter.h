
#ifndef _AST_PRINTER_H_
#define  _AST_PRINTER_H_
#include "AST.h"
namespace Yan
{


class astPrinter: public Ivisitor
{
public:
    astPrinter(std::ostream& out);
    ~astPrinter();

   virtual void visit(BinaryOp* node)override;
   virtual void visit(Identifier* node)override;
   virtual void visit(IntegerLiteral* node)override ;
   virtual void visit(FunctionDef* node)override;
   virtual void visit(Declaration* node)override;
   virtual void visit(Program* node)override;
   virtual void visit(IfStmt* node)override;
   virtual void visit(CompousedStmt* node)override;
   virtual void visit(FunctionCall* node)override;
   virtual void visit(GotoStmt* node)override;
   virtual void visit(ReturnStmt* node)override;
   virtual void visit(StringLiteral* node)override;
   virtual void visit(UnaryOp*node)override;
   virtual void visit(ConditionExpr* node)override;
   virtual void visit(LoopStmt* node)override;
   virtual void visit(BreakContinueStmt* node)override;
   virtual void visit(LabelStmt* node)override;
   virtual void visit(Enumerator * node) override;
 private:
 std::ostream& out_;
    /* data */
};



}
#endif