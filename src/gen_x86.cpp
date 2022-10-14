#include "gen_x86.h"
namespace Yan
{

    const std::vector<std::string> gen::argReg1 = {"%dil", "%sil", "%dl", "%cl", "%r8b", "%r9b"};
    const std::vector<std::string> gen::argReg2 = {"%di", "%si", "%dx", "%cx", "%r8w", "%r9w"};
    const std::vector<std::string> gen::argReg4 = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
    const std::vector<std::string> gen::argReg8 = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    //universal registers
    const std::vector<std::string> gen::regList = {"%r10", "%r11", "%r12", "%r13", "%rbx", "%rcx"};
    const std::vector<std::string> gen::bregList = {"%r10b", "%r11b", "%r12b", "%r13b", "%bl", "%cl"};
    const std::vector<std::string> gen::wregList = {"%r10w", "%r11w", "%r12w", "%r13w", "%bx", "%cx"};
    const std::vector<std::string> gen::dregList = {"%r10d", "%r11d", "%r12d", "%r13d", "%ebx", "%ecx"};

    int gen::labelseq = 1;
    static std::string GetInst(const std::string &inst, int width)
    {

        switch (width)
        {
        case 1:
            return inst + "b";
        case 2:
            return inst + "w";
        case 4:
            return inst + "l";
        case 8:
            return inst + "q";
        default:
            assert(false);

            return inst;
        }
    }

    static std::string GetInst(const std::string &inst, Type *type)
    {
        // assert(type->IsScalar());
        //TODO float
        return GetInst(inst, type->getsize());
    }

    gen::gen(Scope *sc, const std::string &fileName) : globalScope(sc), outfileName(fileName)
    {
        outfstream.open(fileName.c_str(), std::ios::out);
        if (!outfstream.is_open())
        {
            ExitWithError("Open output file:%s fail", fileName.c_str());
        }

        addrGnerator_ = new LvalueAddrGenerator(this);
    }
    gen::~gen()
    {
        if (outfstream.is_open())
        {
            outfstream.close();
            Info("Successfully generated :%s ", outfileName.c_str());
        }
    }

    gen::RegAllocator::RegAllocator()
    {
        RegStatus = {kFreed, kFreed, kFreed, kFreed, kFreed, kFreed};
    }
    int gen::RegAllocator::allocateReg()
    {
        for (int i = 0; i < reg_num; i++)
        {
            if (RegStatus[i] == kFreed)
            {
                RegStatus[i] = kUsed;
                return i;
            }
        }
        ERROR_EXIT << "There is not enough register";
    }

    void gen::RegAllocator::freeReg(int reg)
    {
        RegStatus[reg] = kFreed;
    }
    std::string gen::getReg(int width, int reg, bool argReg)
    {
        if (argReg)
        {
            switch (width)
            {
            case 1:
                return argReg1[reg];
            case 2:
                return argReg2[reg];
            case 4:
                return argReg4[reg];
            case 8:
                return argReg8[reg];
            default:
                assert(false);
            }
        }
        switch (width)
        {
        case 1:
            return bregList[reg];
        case 2:
            return wregList[reg];
        case 4:
            return dregList[reg];
        case 8:
            return regList[reg];
        default:
            assert(false);
        }
        return "";
    }

    void gen::emit(std::string inst)
    {
        outfstream << "\t" << inst << "\n";
    }
    void gen::emit(std::stringstream &inst)
    {
        outfstream << "\t" << inst.str() << std::endl;
    }
    void gen::emitLable(const std::string &lebal)
    {
        outfstream << lebal << ":\n";
    }
    std::string gen::genLabe()
    {
        std::stringstream l;
        l << ".L" << labelseq++;
        return l.str();
    }

    //store value from register to statck
    void gen::storeValue(Type *ty)
    {
        int reg1 = regAllocator_.getStoredreg();
        int reg2 = regAllocator_.getStoredreg();
        auto inst = GetInst("mov", ty);
        auto reg = getReg(ty->getsize(), reg1);
        auto addr = "(" + regList[reg2] + ")";
        emit(inst, reg, addr);

        regAllocator_.freeReg(reg2);
        regAllocator_.storeReg(reg1);
    }
    //load  value to a register
    void gen::loadValue(Identifier *node)
    {

        std::stringstream fm;
        std::string value = node->isLocal_ ? "-" + std::to_string(node->offset_) + "(%rbp)" : node->name_ + "(%rip)";
        int reg = regAllocator_.allocateReg();
        auto ty = node->type_;
        auto inst = GetInst("mov", ty);
        auto reg_str = getReg(ty->getsize(), reg);
        emit(inst, value, reg_str);

        regAllocator_.storeReg(reg);
    }
    void gen::visit(UnaryOp *node)
    {

        if (node->op_ == OpType::OP_DEREF) //*
        {
            DEBUG_LOG << "* operator";
            node->operand_->accept(this);
            auto reg = regAllocator_.getStoredreg();

            if (!node->type_->isKindOf(Type::T_ARRAY))
            {
                emit("movq", "(" + regList[reg] + ")", regList[reg]);
                regAllocator_.storeReg(reg);
            }
            else
            {
                regAllocator_.storeReg(reg);
            }
        }
        else if (node->op_ == OpType::OP_ADDR) //&
        {
            node->operand_->accept(addrGnerator_);
        }
        else if (node->op_ == OpType::OP_POSTINC)
        {
            genLValueAddr(node->operand_);
            node->operand_->accept(this);
            auto reg1 = regAllocator_.getStoredreg();
            emit("addq", "$1", regList[reg1]);
            regAllocator_.storeReg(reg1);
            storeValue(node->type_);
        }
        else if (node->op_ == OpType::OP_NOT)
        {
            node->operand_->accept(this);
            auto r = regAllocator_.getStoredreg();
            emit("cmp", "$0", regList[r]);
            emit("sete " + bregList[r]);
            emit("movzbq ", bregList[r], regList[r]);
            regAllocator_.storeReg(r);
        }
        else if (node->op_ == OpType::OP_DOT)
        {
            NOTICE_LOG << " 111";
            Info("111");
            node->operand_->accept(this);
            Info("22");
        }
    }
    void gen::visit(StringLiteral *node)
    {
        auto LC = strlitMap_[node->strData_];
        std::stringstream fm;
        auto reg = regAllocator_.allocateReg();
        fm << "leaq " << LC << "(%rip) ," << regList[reg];
        emit(fm);
        regAllocator_.storeReg(reg);
    }
    void gen::genLValueAddr(Expr *node)
    {
        NOTICE_LOG << "lvalue";
        assert(!node->type_->isKindOf(Type::T_ARRAY));
        //addrGenerator will call genAddr according node type
        node->accept(addrGnerator_);
    }
    void gen::visit(BinaryOp *node)
    {
        DEBUG_LOG << "node op = " << static_cast<int>(node->op);
        if (node->op == OpType::OP_ASSIGN)
        {
            genLValueAddr(node->left);
            node->right->accept(this);
            storeValue(node->type_);
        }
        else if (node->op == OpType::OP_PTRADD)
        {
            node->left->accept(this);
            node->right->accept(this);
            auto reg1 = regAllocator_.getStoredreg();
            auto reg2 = regAllocator_.getStoredreg();
            std::stringstream fm;
            fm << "imulq $" << node->left->type_->castToDeried()->getBaseType()->getsize() << "," << regList[reg1];
            emit(fm);
            emit("addq", regList[reg1], regList[reg2]);
            regAllocator_.freeReg(reg1);
            regAllocator_.storeReg(reg2);
        }
        else if (node->op == OpType::OP_ADD)
        {
            node->left->accept(this);
            node->right->accept(this);
            genAdd();
        }
        else if (node->op == OpType::OP_SUBTRACT)
        {
            node->left->accept(this);
            node->right->accept(this);
            genSub();
        }
        else if (node->op == OpType::OP_MULTIPLY)
        {
            node->left->accept(this);
            node->right->accept(this);
            genMul();
        }
        else if (node->op == OpType::OP_DIVIDE)
        {
            node->left->accept(this);
            node->right->accept(this);
            genDiv();
        }
        else if (node->op == OpType::OP_MOD)
        {
        }
        else if (node->op == OpType::OP_GT)
        {
            node->left->accept(this);
            node->right->accept(this);
            //Todo Check node type if long/long
            genCmp("setg");
        }
        else if (node->op == OpType::OP_LT)
        {
            node->left->accept(this);
            node->right->accept(this);
            genCmp("setl");
        }
        else if (node->op == OpType::OP_EQ)
        {
            node->left->accept(this);
            node->right->accept(this);
            genCmp("sete");
        }
        else if (node->op == OpType::OP_NE)
        {
            node->left->accept(this);
            node->right->accept(this);
            genCmp("setne");
        }
        else if (node->op == OpType::OP_GE)
        {
            node->left->accept(this);
            node->right->accept(this);
            genCmp("setge");
        }
        else if (node->op == OpType::OP_LE)
        {
            node->left->accept(this);
            node->right->accept(this);
            genCmp("setle");
        }
        else if (node->op == OpType::OP_LOGICAND)
        {
            node->left->accept(this);
            node->right->accept(this);
            auto reg1 = regAllocator_.getStoredreg();
            auto reg2 = regAllocator_.getStoredreg();
            auto falseLabel = genLabe();
            auto endLabel = genLabe();
            emit("cmp ", "$0", regList[reg1]);

            emit("je " + falseLabel);

            emit("cmp ", "$0", regList[reg2]);
            emit("je " + falseLabel);
            emit("movq", "$1", regList[2]);
            emit("jmp " + endLabel);
            emitLable(falseLabel);
            emit("movq", "$0", regList[reg2]);
            emitLable(endLabel);
            regAllocator_.freeReg(reg1);
            regAllocator_.storeReg(reg2);
        }

        else if (node->op == OpType::OP_LOGICOR)
        {
            node->left->accept(this);
            node->right->accept(this);
            auto reg1 = regAllocator_.getStoredreg();
            auto reg2 = regAllocator_.getStoredreg();
            auto trueLabel = genLabe();
            auto endLabel = genLabe();
            emit("cmp ", "$0", regList[reg1]);

            emit("jne " + trueLabel);

            emit("cmp ", "$0", regList[reg2]);
            emit("jne " + trueLabel);
            emit("movq", "$0", regList[2]);
            emit("jmp " + endLabel);
            emitLable(trueLabel);
            emit("movq", "$1", regList[reg2]);
            emitLable(endLabel);
            regAllocator_.freeReg(reg1);
            regAllocator_.storeReg(reg2);
        }
    }

    void gen::genAdd()
    {
        auto reg1 = regAllocator_.getStoredreg();
        auto reg2 = regAllocator_.getStoredreg();
        emit("addq", regList[reg1], regList[reg2]);
        regAllocator_.freeReg(reg1);
        regAllocator_.storeReg(reg2);
    }
    void gen::genSub()
    {
        auto reg1 = regAllocator_.getStoredreg();
        auto reg2 = regAllocator_.getStoredreg();
        emit("subq", regList[reg1], regList[reg2]);
        regAllocator_.freeReg(reg1);
        regAllocator_.storeReg(reg2);
    }
    void gen::genMul()
    {
        auto reg1 = regAllocator_.getStoredreg();
        auto reg2 = regAllocator_.getStoredreg();
        emit("imulq", regList[reg1], regList[reg2]);
        regAllocator_.freeReg(reg1);
        regAllocator_.storeReg(reg2);
    }
    void gen::genDiv()
    {
        auto reg1 = regAllocator_.getStoredreg();
        auto reg2 = regAllocator_.getStoredreg();
        emit("movq " + regList[reg2] + ", %rax");
        emit("cqto");
        emit("idivq  " + regList[reg1]);
        emit("movq ", "%rax ", regList[reg2]);
        regAllocator_.storeReg(reg2);
        regAllocator_.freeReg(reg1);
    }

    void gen::genCmp(const std::string &how, bool longType)
    {
        auto reg1 = regAllocator_.getStoredreg();
        auto reg2 = regAllocator_.getStoredreg();
        if (longType)
        {
            emit("cmp", regList[reg1], regList[reg2]);
        }
        else
        {
            emit("cmp", dregList[reg1], dregList[reg2]);
        }
        std::string inst = how + " " + bregList[reg2];
        emit(inst);
        emit("movzbq", bregList[reg2], regList[reg2]);
        regAllocator_.freeReg(reg1);
        regAllocator_.storeReg(reg2);
    }

    void gen::visit(IntegerLiteral *node)
    {
        auto reg = regAllocator_.allocateReg();
        emit("movq $" + std::to_string(node->value_) + ", " + regList[reg]);
        regAllocator_.storeReg(reg);
    }
    void gen::visit(Enumerator *node)
    {
        auto reg = regAllocator_.allocateReg();
        emit("movq $" + std::to_string(node->value) + ", " + regList[reg]);
        regAllocator_.storeReg(reg);
    }
    void gen::visit(FunctionDef *node)
    {
        DEBUG_LOG << "FunctionDef function name = " << node->identi_->name_;
        DEBUG_LOG << node->identi_->name_ << " funcdef";

        functionName_ = node->identi_->name_;
        emit(".text");
        if(!node->identi_->type_->isStatic())
        {
            emit_noindent(".global " + node->identi_->name_);
        }
        //name:
        emitLable(node->identi_->name_);
        emit("nop");
        emit("pushq %rbp");
        emit("movq %rsp, %rbp");
        int index = 0;
        for (auto &arg : static_cast<FuncType *>(node->identi_->type_)->getParam())
        {
            loardArgs(arg, index++);
        }
        node->body_->accept(this);
        if (node->identi_->type_->castToFunc()->getBaseType()->isKindOf(Type::T_VOID))
        {
            emit("leave");
            emit("ret");
        }
    }
    //store function args from register to stack

    void gen::loardArgs(Identifier *node, int index)
    {
        auto offset = node->offset_;
        auto ty = node->type_;
        auto inst = GetInst("mov", ty);
        auto reg = getReg(ty->getsize(), index, true);
        auto addr = "-" + std::to_string(offset) + "(%rbp)";
        emit(inst, reg, addr);
    }
    void gen::visit(Declaration *node)
    {
        if(node->obj_->type_->isStatic())
        {
            return emitStatic(node);
        }
        if (node->obj_->isLocal_)
        {
            if (node->inits_.empty())
            {
                return;
            }
            node->inits_.front().expr_->accept(this);

            auto reg = regAllocator_.getStoredreg();

            auto obj = node->obj_;
            auto ty = obj->type_;

            auto inst = GetInst("mov", ty);
            auto regstr = getReg(ty->getsize(), reg);
            auto addr = "-" + std::to_string(obj->offset_) + "(%rbp)";
            emit(inst, regstr, addr);

            regAllocator_.freeReg(reg);
            return;
        }
        //TODO if not init
        emitBss(node);
        //TODO else gen data

    }
    void gen::emitStatic(Declaration*var)
    {
        if(var->inits_.empty())
        {
            emitBss(var);
        }
        else
        {
            emitData(var);

        }
    }
    void gen::emitData(Declaration*var)
    {
        //TODO
    }
    void gen::emitBss(Declaration*var)
    {
         Info("gen BSS:");
        auto &name = var->obj_->name_;
        auto align = (var->obj_->type_)->getalign();
        auto size = var->obj_->type_->getsize();
        //
        emit(".data");
        if(!var->obj_->type_->isStatic())
        {
            emit(".global " + name);
        }
        emit(".lcomm "+name+" ,"+std::to_string(size));

    }
    void gen::visit(Identifier *node)
    {
        DEBUG_LOG << "name = " << node->name_ << " node->isLocal_ = " << node->isLocal_;
        std::stringstream fm;
        if (node->type_->isKindOf(Type::T_ARRAY))
        {
            DEBUG_LOG << "identifier is array type";
            genAddr(node);
            return;
        }
        loadValue(node);
    }

    //load a identifier's address to register
    void gen::genAddr(Identifier *node)
    {
        NOTICE_LOG << "genaddr";
        std::stringstream fm;
        auto reg = regAllocator_.allocateReg();
        if (node->isLocal_)
        {
            Info("name:%s offset :%d", node->name_.c_str(), node->offset_);

            fm << "leaq  -" << node->offset_ << "(%rbp) , "
               << regList[reg];
        }
        else
        {

            fm << "leaq " << node->name_ << "(%rip), " << regList[reg];
        }
        emit(fm);
        regAllocator_.storeReg(reg);
    }
    void gen::visit(IfStmt *node)
    {
        node->cond_->accept(this);
        auto reg = regAllocator_.getStoredreg();
        emit("cmp", "$0", regList[reg]);
        auto elsLabel = genLabe();
        auto elsEndLabel = genLabe();
        emit("jz " + elsLabel);
        regAllocator_.freeReg(reg);
        node->then_->accept(this);
        emit("jmp " + elsEndLabel);
        emitLable(elsLabel);
        if (node->else_)
            node->else_->accept(this);
        emitLable(elsEndLabel);
    }
    void gen::visit(SwitchStmt *node)
    {
        auto endLabel = genLabe();
        breakLabels_.push(endLabel);
        for (int i = 0; i < node->caseStmts_.size(); i++)
        {
            auto caseStmt = node->caseStmts_[i];
            if (i + 1 == node->caseStmts_.size())
            {

                checkswitchCondition(caseStmt->switchExp_, caseStmt->caseValue_, caseStmt->lable_, endLabel);
            }
            else
            {
                auto nextCase = node->caseStmts_[i + 1];
                checkswitchCondition(caseStmt->switchExp_, caseStmt->caseValue_, caseStmt->lable_, nextCase->lable_);
            }

            caseStmt->accept(this);
        }
        emitLable(endLabel);
        breakLabels_.pop();
    }
    void gen::visit(CaseDefaltStmt *node)
    {
        emitLable(node->lable_);
        for (auto& stmt: node-> stmts_)
        {
            stmt->accept(this);
        }

//assert(0);
    }

    void gen::visit(LoopStmt *node)
    {
        DEBUG_LOG << " Gen LoopStmtmet";

        auto startLabel = genLabe();
        auto endLabel = genLabe();
        breakLabels_.push(endLabel);
        continueLabels_.push(startLabel);

        if (node->postcheck_)
        {
            emit("jmp " + startLabel);
        }
        else
        {
            checkCondition(node->cond_, startLabel, endLabel);
        }
        emitLable(startLabel);
        node->body_->accept(this);
        if (node->inc_)
        {
            node->inc_->accept(this);
        }
        checkCondition(node->cond_, startLabel, endLabel);
        emitLable(endLabel);
        breakLabels_.pop();
        continueLabels_.pop();
    }
    void gen::visit(BreakContinueStmt *node)
    {
        if (node->kind_ == BreakContinueStmt::kBreak)
        {
            if (breakLabels_.empty())
            {
                ExitWithError("break doesn't in a loop or switch statment");
            }
            emit("jmp " + breakLabels_.top());
        }
        else
        {
            if (continueLabels_.empty())
            {
                ExitWithError("contine doesn't in a loop or switch statment");
            }
            emit("jmp " + continueLabels_.top());
        }
    }
    void gen::checkCondition(Expr *node, std::string trueLabel, std::string falsedLabel)
    {
        if (node == nullptr)
        {
            //for(;;)
            emit("jmp " + trueLabel);
            return;
        }
        node->accept(this);
        auto reg = regAllocator_.getStoredreg();
        emit("cmp $0," + regList[reg]);
        emit("jz " + falsedLabel);
        emit("jmp " + trueLabel);
        regAllocator_.freeReg(reg);
    }
    void gen:: checkswitchCondition(Expr* switch_val,int case_va, const std::string& caselb, const std::string&nextlb)
    {
        switch_val->accept(this);
        auto reg = regAllocator_.getStoredreg();
        emit("cmp", "$"+std::to_string(case_va), regList[reg]);
        emit("jz " + caselb);
        emit("jmp " + nextlb);
        regAllocator_.freeReg(reg);

    }

    void gen::visit(GotoStmt *node)
    {
        std::stringstream s;
        s << "jmp .L." << functionName_ << "." << node->label_;
        emit(s);
    }

    void gen::visit(LabelStmt *node)
    {
        std::stringstream s;
        s << ".L." << functionName_ << "." << node->label_ << ":";
        emit(s);
    }
    void gen::visit(ReturnStmt *node)
    {
        if (node->expr_)
        {
            node->expr_->accept(this);
            auto r = regAllocator_.getStoredreg();
            emit("movq", regList[r], "%rax");
            regAllocator_.freeReg(r);
        }
        else
        {
            emit("movq $0,%rax");
        }

        emit("leave");
        emit("ret");
    }

    void gen::visit(ExprStmt *node)
    {
        Info("ExprStmt");
        if (node->expr_)
        {
            Info("null");
        }
        node->expr_->accept(this);
        Info("ExprStmt22");
        auto reg = regAllocator_.getStoredreg();
        regAllocator_.freeReg(reg);
    }
    void gen::visit(FunctionCall *node)
    {
        static int seq = 0;
        Info("functionCall");

        auto &args = node->argList_;
        for (int i = 0; i < args.size(); i++)
        {
            args[i]->accept(this);
            auto reg = regAllocator_.getStoredreg();

            if (args[i]->type_ && args[i]->type_->getsize() == 1)
            {
                emit("movzbq", bregList[reg], argReg8[i]);
            }
            else
            {
                emit("movq", regList[reg], argReg8[i]);
            }

            regAllocator_.freeReg(reg);
        }

        /* auto reg = regAllocator_.allocateReg();
        Info("reg =%d", reg);
        emit("movq", "%rsp", regList[reg]);
    
        emit("andq", "$15", regList[reg]);
        auto labe = genLabe();
        auto labe1 = genLabe();
        emit("jnz " + labe);
        regAllocator_.freeReg(reg);
        // emit("mov $0,%rax");
        emit("call " + node->designator_->name_);
        emit("jmp " + labe1);
        emitLable(labe);
        emit("subq $8,%rsp");
        //emit("movq $0,%rax");
        emit("   call " + node->designator_->name_);
        emit(" addq $8,%rsp");
        emitLable(labe1);
        */
        emit("movq $0,%rax");
        emit("   call " + node->designator_->name_);

        // if(!node->designator_->type_->castToFunc()->getBaseType()->isKindOf(Type::T_VOID))
        {
            auto r = regAllocator_.allocateReg();
            emit("movq", " %rax", regList[r]); //store return value
            regAllocator_.storeReg(r);
        }
    }

    void gen::visit(CompousedStmt *node)
    {
        Info("gen Compoused statment");
        if (node == nullptr)
        {
            Info("compoused statmets is null");
        }
        int offset = 0;
        if (node->scope_)
        { //extend stack size for block statments

            offset = node->scope_->getTyepSize();
            std::stringstream f;
            f << "subq $" << offset << ", %rsp";
            emit(f);
        }
        for (auto &stmt : node->stmtlist_)
        {
            stmt->accept(this);
        }

        if (offset > 0)
        {
            //restore
            std::stringstream f;
            f << "addq $" << offset << ", %rsp";
            emit(f);
        }
    }
    void gen::genStringLitLables()
    {
        auto table = globalScope->getStringLitTbale();
        int i = 0;
        for (auto &str : table)
        {
            std::stringstream label, str2;
            label << ".LC" << i++;
            strlitMap_.insert({str, label.str()});
            label << ":";
            emit(label);
            str2 << "\t.string \"" << str << "\"";
            emit(str2);
        }
    }
    void gen::visit(Program *node)
    {
        Info("Begin generate assemble code...");
        genStringLitLables();
        //buildin print
        emit(".LC10:");
        emit("\t.string \"%d\\n\"");
        emit(".text");
        emit(".global print");
        emit("print:");
        emit("pushq %rbp");
        emit("movq %rsp, %rbp");
        emit("subq $16, %rsp");
        emit("movq %rdi, -8(%rbp)");
        emit("movq -8(%rbp), %rax");
        emit("movq %rax, %rsi");
        emit("leaq .LC10(%rip), %rdi");
        emit("movq $0,%rax");
        emit("call printf@PLT");
        emit("nop");
        emit("leave");
        emit("ret");
        //build in printstr
        emit(".LC11:");
        emit("\t.string \"%s\"");
        emit(".text");
        emit(".global printstr");
        emit("printstr:");
        emit("pushq %rbp");
        emit("movq %rsp, %rbp");
        emit("subq $16, %rsp");
        emit("movq %rdi, -8(%rbp)");
        emit("movq -8(%rbp), %rax");
        emit("movq %rax, %rsi");
        emit("leaq .LC11(%rip), %rdi");
        emit("movq $0,%rax");
        emit("call printf@PLT");
        emit("nop");
        emit("leave");
        emit("ret");

        for (auto &decl : node->decls_)
        {
            decl->accept(this);
        }
    }
} // namespace Yan