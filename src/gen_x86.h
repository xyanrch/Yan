#ifndef _GEN_X86_H_
#define _GEN_X86_H_
#include "AST.h"
#include "symbol.h"
#include <array>
#include <sstream>
#include <assert.h>
#include <map>
namespace Yan
{

    class LvalueAddrGenerator;
    class gen : public Ivisitor
    {
        using LabelStack = std::stack<std::string>;
        using StringLitLableMap = std::map<std::string, std::string>;

    public:
        explicit gen(Scope *sc, const std::string &fileName = "a.s");
        ~gen();
        std::string genLabe();
        void visitExpr(Expr *node) { node->accept(this); }
        //impliment Ivistor
        virtual void visit(BinaryOp *node) override;
        virtual void visit(Identifier *node) override;
        virtual void visit(IntegerLiteral *node) override;
        virtual void visit(FunctionDef *node) override;
        virtual void visit(Declaration *node) override;
        virtual void visit(Program *node) override;
        virtual void visit(IfStmt *node) override;
        virtual void visit(LoopStmt *node) override;
        virtual void visit(CompousedStmt *node) override;
        virtual void visit(FunctionCall *node) override;
        virtual void visit(GotoStmt *node) override;
        virtual void visit(ReturnStmt *node) override;
        virtual void visit(UnaryOp *node) override;
        virtual void visit(ConditionExpr *node) override {}
        virtual void visit(StringLiteral *node) override;
        virtual void visit(BreakContinueStmt *node) override;
        virtual void visit(LabelStmt *node) override;
        virtual void visit(ExprStmt *node);
        virtual void visit(Enumerator* node);
        virtual void visit(SwitchStmt *node);
        virtual void visit(CaseDefaltStmt *node);

        
        void emitStatic(Declaration*var);
        void emitBss(Declaration*var);
        void emitData(Declaration*var);


        void genAddr(Identifier *node);
        void genLValueAddr(Expr *node);
        void loardArgs(Identifier *node, int index);
        void checkCondition(Expr *node, std::string trueLabel, std::string falsedLabel);
        void checkswitchCondition(Expr* switch_val,int case_va, const std::string& caselb, const std::string&nextlb);

    private:
        void storeValue(Type *ty);
        void loadValue(Identifier *node);
        //operator
        void genAdd();
        void genSub();
        void genMul();
        void genDiv();
        void genCmp(const std::string &how, bool longType = false);

        void genStringLitLables();

        void emit(std::string inst);
        void emit_noindent(std::string label)
        {
            outfstream<<label<<std::endl;
        }
        void emit(std::stringstream &inst);
        void emitLable(const std::string& lebal);
        void emit(std::string inst, std::string reg1, std::string reg2)
        {
            outfstream << "\t" << inst << " " << reg1 << ", " << reg2 << std::endl;
        }
        std::string getReg(int width,int reg, bool argReg = false);

    public:
        class RegAllocator
        {
        public:
            enum Status
            {
                kUsed,
                kFreed
            };
            RegAllocator();
            ~RegAllocator() = default;
            int allocateReg();
            void freeReg(int reg);

            void storeReg(int reg) { allocatedReg.push(reg); }
            int getStoredreg()
            {
                auto reg = allocatedReg.top();
                allocatedReg.pop();
                return reg;
            }

            static constexpr  int reg_num = 6;
            std::array<Status, reg_num> RegStatus;
            std::stack<int> allocatedReg;
        };

    private:
        //1 byte
        static const std::vector<std::string> argReg1;
        //2 byte
        static const std::vector<std::string> argReg2;
        //4 byte
        static const std::vector<std::string> argReg4;
        //8 byte
        static const std::vector<std::string> argReg8;

        static const std::vector<std::string> regList;  //8 byte
        static const std::vector<std::string> bregList; //1 byte
        static const std::vector<std::string> wregList; //2 byte
        static const std::vector<std::string> dregList; //4 byte
        //used to avoid generating duplicated label
        static int labelseq;
        LabelStack breakLabels_;
        LabelStack continueLabels_;
        std::string functionName_;
        //
        StringLitLableMap strlitMap_;
        Scope *globalScope;

        std::string outfileName;
        std::fstream outfstream;

        LvalueAddrGenerator *addrGnerator_;
        RegAllocator regAllocator_;

        //disable copy and assgin
        gen(const gen &) = delete;
        gen &operator=(const gen &) = delete;
    };

    class LvalueAddrGenerator : public Ivisitor
    {
    public:
        LvalueAddrGenerator(gen *instance) : generator_(instance) {}

    private:
        virtual void visit(BinaryOp *node) override { assert(0); }
        virtual void visit(Identifier *node) override { generator_->genAddr(node); }
        virtual void visit(IntegerLiteral *node) override { assert(0); }
        virtual void visit(FunctionDef *node) override { assert(0); }
        virtual void visit(Declaration *node) override { assert(0); }
        virtual void visit(Program *node) override { assert(0); }
        virtual void visit(IfStmt *node) override { assert(0); }
        virtual void visit(CompousedStmt *node) override { assert(0); }
        virtual void visit(FunctionCall *node) override { assert(0); }
        virtual void visit(GotoStmt *node) override { assert(0); }
        virtual void visit(ReturnStmt *node) override { assert(0); }
        virtual void visit(StringLiteral *node) override { assert(0); }
        virtual void visit(UnaryOp *node) override
        {
           // assert(node->op_ == OpType::OP_DEREF);
           NOTICE_LOG<<"LvalueAddrGenerator";
           if(node->op_ == OpType::OP_DEREF)
            {
                generator_->visitExpr(node->operand_);
            }
            else if(node->op_ == OpType::OP_DOT)
            {
                node->operand_->accept(this);
            }
            else
            {
                ERROR_EXIT<<" not support gen addr";
            }
        }
        virtual void visit(ConditionExpr *node) override { assert(0); }
        virtual void visit(LoopStmt *node) override { assert(0); }
        virtual void visit(BreakContinueStmt *node) override { assert(0); }
        virtual void visit(LabelStmt *node) override { assert(0); }
        virtual void visit(ExprStmt *node) override { assert(0); }
        virtual void visit(Enumerator* node)override{ assert(0);}
        virtual void visit(SwitchStmt *node)override{assert(0);}
        virtual void visit(CaseDefaltStmt *node)override{assert(0);}
  

    private:
        gen *generator_;
    };
} // namespace Yan
#endif
