#include "parser.h"
namespace Yan
{

    parser::parser(lexer &s) : scan(s)
    {
        currentScop_ = new Scope;
        currentScop_->setScope(ScopeKind::GLOBAL);
    }
    parser::~parser()
    {
    }
    void parser::enterScope(ScopeKind kind)
    {
        auto sc = new Scope;
        sc->setParent(this->currentScop_);
        sc->setScope(kind);
        sc->depth = currentScop_->depth + 1;

        currentScop_ = sc;
    }
    void parser::leaveScope()
    {

        currentScop_ = currentScop_->getParentScop();
    }
    Expr *parser::primary()
    {
        Expr *node = nullptr;

        if (is(TokenType::T_INTLIT))
        {
            auto t = consume();

            Info(std::to_string(t.getValue()).c_str());
            node = IntegerLiteral::create(t.getValue());
            node->type_ = int_type;
        }
        else if (is(TokenType::T_STRLIT))
        {
            auto t = consume();
            auto len = t.getText().length() + 1;
            node = StringLiteral::create(t.getText());
            auto ty = ArrayType::create(char_type, len);
            node->type_ = ty;
            auto globalSc = currentScop_->getGlobalScope();
            globalSc->addstringLit(t.getText());
        }
        else if (is(TokenType::T_IDENT))
        {
            Identifier *identi = nullptr;
            auto t = consume();
            Info(t.tostring().c_str());
            auto exist = currentScop_->getIdentiInAllScope(t.getText(), &identi);

            if (!exist)
            {

                ExitWithError("undefined variable :%s", t.getText().c_str());
            }
            if (match(TokenType::T_LPAREN))
            {
                return parseFuncCall(identi);
            }
            return identi;
        }
        else if (match(TokenType::T_LPAREN))
        {
            Expr *expr = assign();
            expect(TokenType::T_RPAREN, ")");
            return expr;
        }
        else
        {
            //auto t =peek();
            // std::cout<<t;
            ERROR_EXIT << peek() << " : Invalid Primary expr";
            // ExitWithError("The Primary token must be a number or identifier");
        }

        return node;
    }
    //if the front token doesn't match the specific token exit with error
    void parser::expect(TokenType t, const std::string &what)
    {
        if (!is(t))
        {
            ExitWithError(scan.getLocation(), "matchTed %s,but current token is %s ", what.c_str(), scan.getToken().tostring().c_str());
        }
        else
        {
            consume();
        }
    }
    //Test the front token, if matched consume it,if not put back
    bool parser::match(TokenType t)
    {
        auto token = scan.getToken();
        if (token.type == t)
        {
            //current_token_ = token;
            return true;
        }
        else
        {
            //  current_token_ = token;
            scan.putBack(token);
            return false;
        }
    }
    bool parser::is(TokenType t) const
    {
        auto token = scan.getToken();
        if (token.type == t)
        {
            scan.putBack(token);
            return true;
        }
        else
        {
            scan.putBack(token);
            return false;
        }
    }
    Token parser::consume()
    {
        return scan.getToken();
    }

    // expr = assign(","assign")*
    Expr *parser::expr()
    {
        Expr *left = assign();
        while (match(TokenType::T_COMMA))
        {
            auto right = assign();
            left = BinaryOp::create(OpType::OP_COMMA, left, right);
        }
        return left;
    }
    // assign    = conditional (assign-op assign)?
    // assign-op = "=" | "+=" | "-=" | "*=" | "/=" | "<<=" | ">>="
    //           | "&=" | "|=" | "^="
    Expr *parser::assign()
    {
        auto node = conditional();
        if (match(TokenType::T_ASSIGN)) //=
        {
            return BinaryOp::create(OpType::OP_ASSIGN, node, assign(), node->type_);
        }
        if (match(TokenType::T_ASSLASH)) // /=
        {
            return BinaryOp::create(OpType::OP_ASSMOD, node, assign(), node->type_);
        }
        if (match(TokenType::T_ASSTAR)) //*=
        {
            return BinaryOp::create(OpType::OP_ASSMUL, node, assign(), node->type_);
        }
        if (match(TokenType::T_ASPLUS)) // +=
        {
            return BinaryOp::create(OpType::OP_ASSPLUS, node, assign(), node->type_);
        }
        if (match(TokenType::T_ASMINUS)) // -=
        {
            return BinaryOp::create(OpType::OP_ASSSUB, node, assign(), node->type_);
        }

        if (match(TokenType::T_ASMOD)) // %=
        {
            return BinaryOp::create(OpType::OP_ASSMOD, node, assign(), node->type_);
        }

        //  if (match(TokenType::T_ASSIGN))
        // {
        //     return BinaryOp::create(OpType::OP_ASSIGN,node,assign());
        // }
        //TODO <<= >>= &= ^= |=
        return node;
    }
    // conditional = logor ("?" expr ":" conditional)?
    Expr *parser::conditional()
    {
        Expr *node = logicOr();
        if (match(TokenType::T_QUSTION))
        {
            auto true_expr = expr();
            expect(TokenType::T_COLON, ":");
            auto false_expr = conditional();
            return ConditionExpr::create(node, true_expr, false_expr);
        }
        return node;
    }
    // logor = logand ("||" logand)*
    Expr *parser::logicOr()
    {
        Expr *node = logicAnd();
        while (match(TokenType::T_LOGOR))
        {
            node = BinaryOp::create(OpType::OP_LOGICOR, node, logicAnd());
        }
        return node;
    }
    // logand = bitor ("&&" bitor)*
    Expr *parser::logicAnd()
    {
        Expr *node = bitOr();
        while (match(TokenType::T_LOGAND))
        {
            node = BinaryOp::create(OpType::OP_LOGICAND, node, bitOr());
        }
        return node;
    }

    // bitor = bitand ("|" bitand)*
    Expr *parser::bitOr()
    {
        Expr *node = bitXor();
        while (match(TokenType::T_BITOR))
        {
            node = BinaryOp::create(OpType::OP_BITOR, node, bitXor());
        }
        return node;
    }
    Expr *parser::bitXor()
    {
        Expr *node = bitAnd();
        while (match(TokenType::T_BITXOR))
        {
            node = BinaryOp::create(OpType::OP_BITXOR, node, bitAnd());
        }
        return node;
    }
    Expr *parser::bitAnd()
    {
        Expr *node = equality();
        while (match(TokenType::T_AMPER))
        {
            node = BinaryOp::create(OpType::OP_BITAND, node, equality());
        }
        return node;
    }
    // equality = relational ("==" relational | "!=" relational)*
    Expr *parser::equality()
    {
        Expr *node = relational();
        while (1)
        {
            if (match(TokenType::T_EQ))
            {
                node = BinaryOp::create(OpType::OP_EQ, node, relational());
            }
            else if (match(TokenType::T_NE))
            {
                node = BinaryOp::create(OpType::OP_NE, node, relational());
            }
            else
                return node;
        }
        return node;
    }
    // relational = shift ("<" shift | "<=" shift | ">" shift | ">=" shift)*
    Expr *parser::relational()
    {
        Expr *node = shift();
        while (1)
        {
            if (match(TokenType::T_LT))
            {
                node = BinaryOp::create(OpType::OP_LT, node, shift());
            }
            else if (match(TokenType::T_LE))
            {
                node = BinaryOp::create(OpType::OP_LE, node, shift());
            }
            else if (match(TokenType::T_GT))
            {
                node = BinaryOp::create(OpType::OP_GT, node, shift());
            }
            else if (match(TokenType::T_GE))
            {
                node = BinaryOp::create(OpType::OP_GE, node, shift());
            }
            else
                return node;
        }
        return node;
    }
    // shift = sum ("<<" sum | ">>" sum)*
    Expr *parser::shift()
    {
        Expr *node = sum();
        while (1)
        {
            if (match(TokenType::T_LSHIFT))
            {
                node = BinaryOp::create(OpType::OP_LSHIFT, node, sum());
            }
            else if (match(TokenType::T_RSHIFT))
            {
                node = BinaryOp::create(OpType::OP_RSHIFT, node, sum());
            }
            else
                return node;
        }
        return node;
    }
    //sum -> mul (('+' mul)|('-' mul))*
    Expr *parser::sum()
    {
        Expr *node = mul();
        for (;;)
        {
            if (match(TokenType::T_ADD))
            {
                node = richadd(node, mul());
            }
            else if (match(TokenType::T_MINUS))
            {
                node = richsub(node, mul());
            }
            else
            {
                return node;
            }
        }
    }
    Expr *parser::richadd(Expr *lhs, Expr *rhs)
    {
        return BinaryOp::create(OpType::OP_ADD, lhs, rhs);
        /*
        if(isInteger(lhs) && isInteger(rhs))        
        {
           auto exp = BinaryOp::create(OpType::OP_ADD, lhs, rhs);
           exp->type_ = lhs->type_;
           return exp;
        }
        if(lhs->type_->castToDeried() && isInteger(rhs))
        {
         //    lhs = mayCasttoPtr(lhs->type_)
            auto exp =  BinaryOp::create(OpType::OP_PTRADD, lhs, rhs);
           
            return exp;           
            
        }
        if(isInteger(lhs) && rhs->type_->castToDeried() )
        {
             //rhs = mayCasttoPtr(rhs->type_)
            auto exp =  BinaryOp::create(OpType::OP_PTRADD, rhs, lhs);
            return exp;
        }

        ERROR_EXIT<<peek()<<" unsupported Type for add/sub operation";
*/
    }
    Expr *parser::richsub(Expr *lhs, Expr *rhs)
    {
        return BinaryOp::create(OpType::OP_SUBTRACT, lhs, rhs);
    }
    Expr *parser::mul()
    {
        Expr *node = cast();
        for (;;)
        {
            if (match(TokenType::T_STAR))
            {
                node = BinaryOp::create(OpType::OP_MULTIPLY, node, cast());
            }
            else if (match(TokenType::T_SLASH))
            {
                node = BinaryOp::create(OpType::OP_DIVIDE, node, cast());
            }
            else if (match(TokenType::T_PERCENT))
            {
                node = BinaryOp::create(OpType::OP_MOD, node, cast());
            }
            else
            {
                return node;
            }
        }
    }
    //cast = (type)cast|unary
    Expr *parser::cast()
    {
        if (is(TokenType::T_RPAREN))
        {
            auto t = consume(); //consume '('
            if (isTypeName())
            {

                auto ty = baseType(nullptr);

                expect(TokenType::T_LPAREN, ")");
                if (!is(TokenType::T_LBRACE))
                {
                    consume();
                    return UnaryOp::create(OpType::OP_CAST, cast(), ty);
                }
            }
            scan.putBack(t); //puback token'('
        }
        return unary();
    }
    Expr *parser::unary()
    {
        if (match(TokenType::T_ADD))
        {
            return cast();
        }
        if (match(TokenType::T_MINUS))
        {
            return BinaryOp::create(OpType::OP_SUBTRACT, IntegerLiteral::create(0), cast());
        }
        if (match(TokenType::T_INC))
        {
            return UnaryOp::create(OpType::OP_PREINC, cast());
        }
        if (match(TokenType::T_AMPER))
        {
            return UnaryOp::create(OpType::OP_ADDR, cast());
        }
        if (match(TokenType::T_STAR))
        {
            auto operand = cast();
            Type *t;
            if (operand->type_->isKindOf(Type::T_PTR))
            {
                t = operand->type_->castToPtr()->getBaseType();
            }
            else
            {
                ExitWithError(operand->type_->tostring().c_str());
            }

            return UnaryOp::create(OpType::OP_DEREF, operand, t);
            //return UnaryOp::create(OpType::OP_DEREF, cast());
        }
        if (match(TokenType::T_LOGNOT))
        {
            return UnaryOp::create(OpType::OP_NOT, cast());
        }
        return postfix();
    }
    // postfix = compound-literal
    //         | primary ("[" expr "]" | "." ident | "->" ident | "++" | "--")*
    Expr *parser::postfix()
    {
        auto node = primary();
        while (1)
        {
            if (match(TokenType::T_INC))
            {
                return UnaryOp::create(OpType::OP_POSTINC, node, node->type_);
            }
            if (match(TokenType::T_LBRACKET))
            {
                //x[i] = *(x+i)
                auto exp = BinaryOp::create(OpType::OP_PTRADD, node, expr(), node->type_);
                expect(TokenType::T_RBRACKET, "]");
                DEBUG_LOG << node->type_->castToDeried()->getBaseType()->tostring();
                node = UnaryOp::create(OpType::OP_DEREF, exp, node->type_->castToDeried()->getBaseType());
                continue;
            }
            if (match(TokenType::T_DOT))
            {
                node = structRef(node);
                continue;
            }
            if (match(TokenType::T_ARROW))
            {                                                            //A->a == (*A).a
                node = UnaryOp::create(OpType::OP_DEREF, node, nullptr); //todo type
                node = structRef(node);
            }
            return node;
        }
    }

    Expr *parser::structRef(Expr *node)
    {
        if (node->type_->isKindOf(Type::T_STRUCT) == false)
        {
            ERROR_EXIT << "Expected a struct type";
        }
        if (is(TokenType::T_IDENT) == false)
        {
            ERROR_EXIT << "Expect identifiler in struct ref";
        }
        auto token = consume();
        auto *mem = node->type_->castToStruct()->findMember(token.getText());

        // auto baseOffset = node->type_->offset;
        if (mem == nullptr)
        {
            ERROR_EXIT << "there is no member :" << token.getText() << " in struct";
        }
        auto newmem = Identifier::clone(mem);
        //update offset struct Test t;
        newmem->offset_ += static_cast<Identifier *>(node)->offset_;
        node = UnaryOp::create(OpType::OP_DOT, newmem, newmem->type_);
        // node =  BinaryOp::create(OpType::OP_DOT, node, mem,mem->type);

        return node;
    }

    IfStmt *parser::parseIfStmt()
    {
        expect(TokenType::T_LPAREN, "(");
        auto condition = expr();
        expect(TokenType::T_RPAREN, ")");
        Stmt *then = nullptr;
        Stmt *els = nullptr;
        if (!is(TokenType::T_LBRACE))
        {
            then = parseSingleStmt();
        }
        else
        {
            selfScope self(*this, ScopeKind::BLOCK);
            then = parseCompoundStmt();
            static_cast<CompousedStmt *>(then)->scope_ = currentScop_;
        }
        if (is(TokenType::T_ELSE))
        {
            consume();

            if (!is(TokenType::T_LBRACE))
            {
                els = parseSingleStmt();
            }
            else
            {
                selfScope self(*this, ScopeKind::BLOCK);
                els = parseCompoundStmt();
                static_cast<CompousedStmt *>(els)->scope_ = currentScop_;
            }
        }

        return IfStmt::create(condition, then, els);
    }
    LoopStmt *parser::parseWhileStmt()
    {
        expect(TokenType::T_LPAREN, "(");
        auto cond = expr();
        expect(TokenType::T_RPAREN, ")");
        Stmt *then = nullptr;
        if (!is(TokenType::T_LBRACE))
        {
            then = parseSingleStmt();
        }
        else
        {
            selfScope self(*this, ScopeKind::BLOCK);
            then = parseCompoundStmt();
            static_cast<CompousedStmt *>(then)->scope_ = currentScop_;
        }

        return LoopStmt::create(cond, then);
    }

    //for(int i =0;i<n;i++)
    // {
    //do something
    //}
    //
    // { int a=0;
    //for(;i<n;i++)
    //{

    //}

    //}
    //
    Stmt *parser::parseForStmt()
    {
        // Stmt *init = nullptr;
        Expr *cond = nullptr;
        ExprStmt *inc = nullptr;
        CompousedStmt *compstmt = nullptr;

        expect(TokenType::T_LPAREN, "(");
        if (!is(TokenType::T_SEMI))
        {
            enterScope(ScopeKind::BLOCK);
            compstmt = CompousedStmt::create();

            if (isTypeName())
            {
                parseDeclaration(true, compstmt);
            }
            else
            {
                compstmt->addStmt(ExprStmt::create(expr()));
                expect(TokenType::T_SEMI, ";");
            }
        }
        else
        {
            expect(TokenType::T_SEMI, ";");
        }

        if (!is(TokenType::T_SEMI))
        {
            cond = expr();
        }
        expect(TokenType::T_SEMI, ";");

        if (!is(TokenType::T_RPAREN))
        {
            inc = ExprStmt::create(expr());
        }
        expect(TokenType::T_RPAREN, ")");
        Stmt *body = nullptr;
        if (!is(TokenType::T_LBRACE))
        {
            body = parseSingleStmt();
        }
        else
        {
            selfScope self(*this, ScopeKind::BLOCK);
            body = parseCompoundStmt();
            static_cast<CompousedStmt *>(body)->scope_ = currentScop_;
            currentScop_->dumpSymbol(std::cout);
        }

        auto forloop = LoopStmt::create(cond, body);
        forloop->inc_ = inc;
        if (compstmt)
        {
            // auto compstmt = CompousedStmt::create();
            //
            compstmt->addStmt(forloop);
            //           static_cast<CompousedStmt *>(body)->scope_ = currentScop_;
            currentScop_->dumpSymbol(std::cout);
            compstmt->scope_ = currentScop_;
            leaveScope();
            currentScop_->dumpSymbol(std::cout);
            return compstmt;
        }
        currentScop_->dumpSymbol(std::cout);
        return forloop;
    }
    LoopStmt *parser::parseDoWhileStmt()
    {
        Stmt *then = nullptr;
        {
            selfScope self(*this, ScopeKind::BLOCK);
            then = parseCompoundStmt();
            static_cast<CompousedStmt *>(then)->scope_ = currentScop_;
        }
        expect(TokenType::T_WHILE, "while");
        expect(TokenType::T_LPAREN, "(");
        auto cond = expr();
        expect(TokenType::T_RPAREN, ")");
        expect(TokenType::T_SEMI, ";");
        return LoopStmt::create(cond, then, true);
    }
    BreakContinueStmt *parser::parseContinueBreakStmt(BreakContinueStmt::kind type)
    {
        expect(TokenType::T_SEMI, ";");
        return BreakContinueStmt::create(type);
    }
    SwitchStmt *parser::parseSwitchStmt()
    {
        expect(TokenType::T_LPAREN, "(");
        auto exp = expr(); //
        if (exp->type_->isKindOf(Type::T_INT) == false)
        {
            ERROR_EXIT << "expect int type in switch statement";
        }
        expect(TokenType::T_RPAREN, ")");
        expect(TokenType::T_LBRACE, "{");
        auto switchstmt = SwitchStmt::create();
        //TODO
        while (!match(TokenType::T_RBRACE) && !match(TokenType::T_EOF))
        {
            if (match(TokenType::T_CASE))
            {
                switchstmt->caseStmts_.push_back(parseCaseStmt(false, exp));
            }
            else if (match(TokenType::T_DEFALT))
            {
                switchstmt->caseStmts_.push_back(parseCaseStmt(true, exp));
            }
            else
            {
                ERROR_EXIT << "Expect 'case' or 'default' keyword";
            }
        }
        return switchstmt;
    }
    CaseDefaltStmt *parser::parseCaseStmt(bool isdefault, Expr *exp)
    {

        int caseValue = isdefault ? 0 : constExpr<int>();
        expect(TokenType::T_COLON, ":");
        auto caseDefualt = CaseDefaltStmt::create(exp, isdefault, caseValue);
        while(!isOneOf(TokenType::T_CASE,TokenType::T_DEFALT))
        {
            if(isdefault && is(TokenType::T_RBRACE))
            {
                break;
            }
        if(match(TokenType::T_LBRACE))//{}
        {
            selfScope self(*this,ScopeKind::BLOCK);
            auto comstmt = parseCompoundStmt();
            comstmt->scope_ = currentScop_;
            caseDefualt->stmts_.push_back(comstmt);
        }
        else
        {
            caseDefualt->stmts_.push_back(parseSingleStmt());
        }

        }
        return caseDefualt;

    }

    ReturnStmt *parser::parseReturnStmt()
    {
        Expr *exp = nullptr;
        if (!is(TokenType::T_SEMI))
        {
            exp = expr();
        }
        expect(TokenType::T_SEMI, ";");
        return ReturnStmt::create(exp);
    }
    Stmt *parser::parseSingleStmt()
    {
        if (match(TokenType::T_IF))
        {
            return parseIfStmt();
        }
        if (match(TokenType::T_WHILE))
        {
            return parseWhileStmt();
        }
        if (match(TokenType::T_DO))
        {
            return parseDoWhileStmt();
        }
        if (match(TokenType::T_SWITCH))
        {
            return parseSwitchStmt();
        }
        if (match(TokenType::T_BREAK))
        {
            return parseContinueBreakStmt(BreakContinueStmt::kBreak);
        }
        if (match(TokenType::T_CONTINUE))
        {
            return parseContinueBreakStmt(BreakContinueStmt::kcontinue);
        }
        if (match(TokenType::T_RETURN))
        {
            return parseReturnStmt();
        }
        if (match(TokenType::T_GOTO))
        {
            auto token = consume();
            if (token.type != TokenType::T_IDENT)
            {
                ExitWithError("expect identifier");
            }
            expect(TokenType::T_SEMI, ";");
            return GotoStmt::create(token.getText());
        }
        if (match(TokenType::T_FOR))
        {
            return parseForStmt();
        }
        if (is(TokenType::T_IDENT))
        {
            Token varToken = consume();
            Info("vartoken:%s", varToken.tostring().c_str());

            if (match(TokenType::T_COLON))
            {

                return LabelStmt::create(varToken.getText());
            }
            else
            {
                scan.putBack(varToken);
                auto assign = expr();
                expect(TokenType::T_SEMI, ";");
                return ExprStmt::create(assign);
            }
        }
        else
        {
            auto exp = expr();
            expect(TokenType::T_SEMI, ";");

            return ExprStmt::create(exp);
        }
        ERROR_EXIT << "Token:" << peek().tostring() << " Unkown statment";
    }
    CompousedStmt *parser::parseCompoundStmt()
    {
        expect(TokenType::T_LBRACE, "{");
        auto compoused = CompousedStmt::create();
        while (!match(TokenType::T_RBRACE))
        {
            if (match(TokenType::T_EOF))
            {
                ExitWithError(__func__);
            }

            if (match(TokenType::T_SEMI))
            {
                //consume empty stmt
                continue;
            }
            else if (isTypeName())
            {
                parseDeclaration(true, compoused);
            }

            else if (is(TokenType::T_LBRACE))
            {
                selfScope self(*this, ScopeKind::BLOCK);
                auto block = parseCompoundStmt();
                block->scope_ = currentScop_;
                compoused->addStmt(block);
            }

            else
            {

                compoused->addStmt(parseSingleStmt());
            }
        }
        return compoused;
    }
    FunctionCall *parser::parseFuncCall(Identifier *identi)
    {

        if (!identi->type_->isKindOf(Type::T_FUNC))
        {
            ExitWithError("Expect function type");
        }
        auto funcCall = FunctionCall::create(identi);

        auto argsNum = identi->type_->castToFunc()->getParam().size();
        Info("num    %d", argsNum);
        for (int i = 0; i < argsNum; i++)
        {
            Expr *exp = assign();
            funcCall->addArg(exp);
            if (i < argsNum - 1)
            {
                expect(TokenType::T_COMMA, ",");
            }
        }
        expect(TokenType::T_RPAREN, ")");
        return funcCall;
    }
    FunctionDef *parser::parseFuncDef()
    {
        storageClass sclass;
        auto type = baseType(&sclass);
        auto pair = declarator(type);
        auto name = pair.second;
        auto new_ty = pair.first;
        auto funcIden = Identifier::create(name, new_ty, false);
        currentScop_->addSymoble(name, funcIden);

        selfScope self(*this, ScopeKind::FUNC);
        for (auto param : new_ty->castToFunc()->getParam())
        {
            currentScop_->addSymoble(param->name_, param);
            param->offset_ = currentScop_->caculateOffset(param->name_);
        }
        auto func = FunctionDef::create(funcIden);
        auto body = parseCompoundStmt();
        body->scope_ = currentScop_;
        func->setBody(body);

        return func;
    }
    //Initializer parser::parseInitializerList(Type*ty)
    ////{

    //}
    Initializer parser::parseDecInitializer(Type *ty)
    {
        if (match(TokenType::T_LBRACE) || isString(ty)) //{}
        {
            //return parseInitializerList(ty);
        }
        else
        {
            auto exp = assign();
            return Initializer(ty, 0, exp);
        }
    }
    // declaration = basetype declarator type-suffix ("=" lvar-initializer)? ";"
    //             | basetype ";"
    void parser::parseDeclaration(bool islocal, Node *node)
    {
        DEBUG_LOG << " islocal = " << islocal;
        storageClass sclass = storageClass::UNKNOW;
        auto type = baseType(&sclass);
        if (match(TokenType::T_SEMI))
        {
            //; struct u{int a;};
            return;
            //don't need create a ast node;
        }

        while (!match(TokenType::T_SEMI) && !match(TokenType::T_EOF))
        {

            auto pair = declarator(type);

            auto identi = Identifier::create(pair.second, pair.first, islocal);
            currentScop_->addSymoble(pair.second, identi);
            if (sclass == storageClass::TYPE_DEF)
            {
                identi->class_ = storageClass::TYPE_DEF;
                continue;
            }
            identi->setoffset(currentScop_->caculateOffset(pair.second));
            auto *decl = Declaration::create(identi);
            //TODO init
            if (match(TokenType::T_ASSIGN))
            {
                auto init = parseDecInitializer(identi->type_);
                decl->inits_.push_back(init);
            }
            //
            //
            if (islocal)
            {
                static_cast<CompousedStmt *>(node)->addStmt(decl);
            }
            else
            {
                static_cast<Program *>(node)->add(decl);
            }
            if (match(TokenType::T_COMMA))
            {
                continue;
            }
        }
    }

    void parser::defineBuildinFunc(std::string name, Type *reType, std::vector<Type *> paramType)
    {
        auto functype = FuncType::create(reType);
        for (const auto &paramT : paramType)
        {
            functype->addParam(Identifier::create("", paramT, true));
        }
        auto identi = Identifier::create(name, functype, false);
        currentScop_->addSymoble(name, identi);
    }
    void parser::addBuildinFunctions()
    {
        //user add buildin function
        defineBuildinFunc("print", void_type, {int_type});
        defineBuildinFunc("printstr", void_type, {PtrType::create(char_type)});
        //system buildin function
        auto void_ptr = PtrType::create(void_type);
        defineBuildinFunc("__builtin_va_arg", void_type, {void_ptr, void_ptr});
        defineBuildinFunc("__builtin_va_start", void_type, {void_ptr});
    }
    Program *parser::parseProgram()
    {
        auto program = Program::create();
        addBuildinFunctions();

        Scope *funcscop = nullptr;
        while (!match(TokenType::T_EOF))
        {
            if (isFuncdef())
            {
                program->add(parseFuncDef());
            }
            else
            {
                parseDeclaration(false, program);
            }
        }

        return program;
    }

    bool parser::isFuncdef()
    {
        std::stack<Token> cache;
        bool isfuncdef = false;
        while (!is(TokenType::T_SEMI) && !is(TokenType::T_LBRACE))
        {
            if (is(TokenType::T_EOF))
            {
                ExitWithError("unexpected Eof");
            }

            cache.push(consume());
        }
        if (is(TokenType::T_SEMI))
        {

            isfuncdef = false;
        }
        //end with pattern .* ) {.*
        else if (is(TokenType::T_LBRACE) && cache.top().type == TokenType::T_RPAREN)
        {
            isfuncdef = true;
        }
        while (!cache.empty())
        {
            putBack(cache.top());
            cache.pop();
        }

        return isfuncdef;
    }

    bool parser::isTypeName()
    {
        return isOneOf(TokenType::T_INT,
                       TokenType::T_CHAR,
                       TokenType::T_BOOL,
                       TokenType::T_ENUM,
                       TokenType::T_SHORT,
                       TokenType::T_SIGNED,
                       TokenType::T_UNSIGNED,
                       TokenType::T_STATIC,
                       TokenType::T_STRUCT,
                       TokenType::T_UNION,
                       TokenType::T_VOID,
                       TokenType::T_TYPDEF,
                       TokenType::T_EXTERN,
                       TokenType::T_LONG) ||
               (is(TokenType::T_IDENT) && findtypedef(peek().getText()));
    }
    bool parser::findtypedef(const std::string &name)
    {
        Identifier *identi = nullptr;
        currentScop_->getIdentiInAllScope(name, &identi);
        if (identi && identi->class_ == storageClass::TYPE_DEF)
        {
            return true;
        }
        return false;
    }

    // basetype = builtin-type | struct-decl | typedef-name | enum-specifier
    //
    // builtin-type = "void" | "_Bool" | "char" | "short" | "int"
    //              | "long" | "long" "long"
    Type *parser::baseType(storageClass *sclass)
    {
        if (!isTypeName())
        {
            ExitWithError("Type name expected");
        }
        enum
        {
            kkindnone = 0,
            kvoid = 1,
            kbool,
            kchar,
            kint
            // kfloat,
            //kdouble
        } kind = kkindnone;
        enum
        {
            ksizenone = 0,
            kshort = 1,
            klong,
            kllong
        } size = ksizenone;

        enum
        {
            ksignnone = 0,
            ksigned = 1,
            kunsigned
        } sign = ksignnone;

        Type *ty = nullptr;
        storageClass storage_class = storageClass::UNKNOW;
        while (isTypeName())
        {
            if (isOneOf(TokenType::T_STATIC, TokenType::T_EXTERN, TokenType::T_TYPDEF))
            {
                //handle storage class
                if (storage_class != storageClass::UNKNOW)
                {
                    //storage alreadly have a value;
                    ExitWithError("Type mismatch");
                }
                if (match(TokenType::T_STATIC))
                {
                    storage_class = storageClass::STATIC;
                }
                else if (match((TokenType::T_EXTERN)))
                {
                    storage_class = storageClass::EXTERN;
                }
                else if (match(TokenType::T_TYPDEF))
                {
                    storage_class = storageClass::TYPE_DEF;
                }
            }
            //handle user defined type
            else if (is(TokenType::T_STRUCT))
            {
                ty = parseStructDecl();
            }
            else if (is(TokenType::T_ENUM))
            {
                ty = parseEnumSpecifier();
                //  expect(TokenType::T_SEMI,";");
            }
            else if (is(TokenType::T_UNION))
            {
                ty = parseUnionSpecifier();
            }
            //Handle build-in type

            else if (isOneOf(TokenType::T_SHORT, TokenType::T_LONG))
            {
                //handle size;
                if (size != ksizenone && size != klong)
                {
                    ExitWithError("Type mismatch");
                }
                if (match(TokenType::T_SHORT))
                {
                    size = kshort;
                }
                else if (match(TokenType::T_LONG))
                {
                    size = size ? kllong : klong;
                }
            }
            //build-in type
            else if (isOneOf(TokenType::T_INT, TokenType::T_CHAR, TokenType::T_VOID, TokenType::T_BOOL))
            {
                if (kind != kkindnone)
                {
                    ExitWithError("Type mismatch");
                }
                if (match(TokenType::T_INT))
                {
                    kind = kint;
                }
                else if (match(TokenType::T_CHAR))
                {
                    kind = kvoid;
                }
                else if (match(TokenType::T_VOID))
                {
                    kind = kvoid;
                }
                else if (match(TokenType::T_BOOL))
                {
                    kind = kbool;
                }
            }

            //sign

            else if (isOneOf(TokenType::T_UNSIGNED, TokenType::T_SIGNED))
            {
                if (sign != ksignnone)
                {
                    ExitWithError("mismatch type");
                }
                if (match(TokenType::T_UNSIGNED))
                {
                    sign = kunsigned;
                }
                else if (match(TokenType::T_SIGNED))
                {
                    sign = ksigned;
                }
            }
            //handle user defined type;
            else if (peek().type == TokenType::T_IDENT)
            {
                Identifier *identi = nullptr;
                if (currentScop_->getIdentiInAllScope(peek().getText(), &identi))
                {
                    consume();
                    return identi->type_;
                }
            }
        }
        if (sclass)
        {
            *sclass = storage_class;
        }

        switch (kind)
        {
        case kvoid:
            ty = void_type;
            break;
        case kchar:
            ty = char_type;
            break;
        case kbool:
            ty = bool_type;
            break;
        default:
            break;
        }
        if (ty == nullptr)
        {
            switch (size)
            {
            case kshort:
                ty = short_type;
                break;
            case klong:
                ty = long_type;
                break;
            case kllong:
                ty = long_type;
                break; //todo
            default:
                ty = int_type;
                break;
            }
        }
        return ty;
    }
    Type *parser::modifyBase(Type *type, Type *base, Type *new_base)
    {
        if (type == base)
        {
            return new_base;
        }
        auto ty = base->castToDeried();
        ty->setBase(modifyBase(type, ty->getBaseType(), new_base));
        return ty;
    }
    // declarator = "*"* ("(" declarator ")" | ident) type-suffix
    //int *p();
    Declarator parser::declarator(Type *type)
    {
        Type *ty = type;
        Info(peek().tostring().c_str());
        while (match(TokenType::T_STAR))
        {
            ty = PtrType::create(ty);
        }

        if (match(TokenType::T_LPAREN))
        {

            auto pair = declarator(ty);
            Type *base = pair.first;
            expect(TokenType::T_RPAREN, ")");

            auto new_base = type_suffix(ty);
            auto return_type = modifyBase(ty, base, new_base);

            return std::make_pair(return_type, pair.second);
        }
        else if (is(TokenType::T_IDENT))
        {
            auto name = consume().getText();
            ty = type_suffix(ty);
            return std::make_pair(ty, name);
        }
        else
        {
            ty = type_suffix(ty);
            //abstract declarator int*();
            return std::make_pair(ty, "");
        }
    }
    Type *parser::type_suffix(Type *type)
    {
        if (is(TokenType::T_LBRACKET))
        {
            return declarator_array(type);
        }
        if (is(TokenType::T_LPAREN))
        {
            return declarator_func(type);
        }
        return type;
    }

    Type *parser::declarator_array(Type *type)
    {
        expect(TokenType::T_LBRACKET, "[");
        bool incomplete = true;
        int len = -1;
        if (!is(TokenType::T_RBRACKET))
        {
            len = constExpr<int>();
            expect(TokenType::T_RBRACKET, "]");
            incomplete = false;
        }

        if (is(TokenType::T_LBRACKET))
        {
            type = declarator_array(type);
            if (type->isIncomplete())
            {
                ExitWithError("Array type incompleye");
            }
        }
        assert(len > 0);
        Type *array = ArrayType::create(type, len);
        array->setIncomplete(incomplete);

        return array;
    }

    Type *parser::declarator_func(Type *type)
    {
        if (type->isKindOf(Type::T_ARRAY) ||
            type->isKindOf(Type::T_FUNC))
        {
            ExitWithError("function return type can't be:%s", type->tostring().c_str());
        }
        auto functionType = FuncType::create(type);
        expect(TokenType::T_LPAREN, "(");
        while (1)
        {
            if (isOneOf(TokenType::T_EOF, TokenType::T_RPAREN))
            {
                break;
            }
            if (match(TokenType::T_ELLIPSIS))
            {
                functionType->setVarargsFlag(true);
                break;
            }
            auto pair = parse_func_param();
            auto param = Identifier::create(pair.second, pair.first, true);
            functionType->addParam(param);
            if (isOneOf(TokenType::T_EOF, TokenType::T_RPAREN))
            {
                break;
            }

            expect(TokenType::T_COMMA, ",");
        }
        expect(TokenType::T_RPAREN, ")");

        return functionType;
    }

    Declarator parser::parse_func_param()
    {
        Type *ty;
        if (isTypeName())
        {
            ty = baseType(nullptr);
        }
        else
        {
            ExitWithError("Expect type");
        }

        auto pair = declarator(ty);
        ty = pair.first;
        if (ty->isKindOf(Type::T_ARRAY))
        {
            ty = PtrType::create(ty->castToArray()->getBaseType());
        }
        else if (ty->isKindOf(Type::T_FUNC))
        {
            ty = PtrType::create(ty);
        }
        return std::make_pair(ty, pair.second);
    }
    // struct-union-decl = attribute? ident? ("{" struct-members)?
    Type *parser::structUnionDecl()
    {
        if (is(TokenType::T_IDENT)) //identi
        {
            auto t = consume();
            if (!is(TokenType::T_LBRACE))
            {
                //struct Name bb;
                auto find = currentScop_->findTagInAllScope(t.getText());
                if (!find)
                {
                    //没有找到，是前置声明，创建tag
                    auto ty = StructType::create();

                    currentScop_->addTag(t.getText(), Identifier::create(t.getText(), ty, false));
                    return ty;
                }
                if (!find->type_->isKindOf(Type::T_STRUCT))
                {
                    //找到了，类型不匹配，错误退出。
                    ERROR_EXIT << t.getText() << " not a struct type";
                }
                //找到了，类型也匹配，返回类型
                //struct IPADDR　ａｄｄｒ
                return find->type_;
            }
            //struct point {

            //}
            putBack(t);
        }

        ///handle "struct *ss" ss is a pointer to unnamed incomplete struct type;
        if (is(TokenType::T_STAR))
        {
            return StructType::create();
        }
        Type *ty;
        if (is(TokenType::T_IDENT))
        {
            auto t = consume();
            if (auto *identi = currentScop_->findTagInCurrentScope(t.getText()))
            {
                if (identi->type_->isKindOf(Type::T_STRUCT) == false)
                {
                    ERROR_EXIT << "Not a struct tag:" << t.getText();
                }
                ty = identi->type_;
            }
            else
            {
                ty = StructType::create();
                currentScop_->addTag(t.getText(), Identifier::create(t.getText(), ty, false));
            }
        }
        else
        {
            ERROR_EXIT << "Wrong struct define";
        }

        //struct members
        expect(TokenType::T_LBRACE, "{");
        auto struct_ty = ty->castToStruct();
        while (!match(TokenType::T_RBRACE))
        {
            struct_ty->addMember(parseStructMember());
        }
        return struct_ty;
    }
    // struct-decl = "struct" ident? ("{" struct-member "}")?
    Type *parser::parseStructDecl()
    {
        expect(TokenType::T_STRUCT, "struct");
        auto *struct_ty = structUnionDecl()->castToStruct();
        //Assign offset
        int offset = 0;
        for (auto &mem : struct_ty->getMembers())
        {
            if (mem.type_->isIncomplete())
            {
                ERROR_EXIT << "incomplete struct member " << mem.name_;
            }
            offset = align_to(offset, mem.type_->getalign());
            mem.offset_ = offset;
            offset += mem.type_->getsize();
            if (struct_ty->getalign() < mem.type_->getalign())
            {
                struct_ty->setalign(mem.type_->getalign());
            }
        }
        struct_ty->setSize(align_to(offset, struct_ty->getalign()));
        struct_ty->setIncomplete(false);
        return struct_ty;
    }
    // struct-member = basetype declarator type-suffix ";"
    StructType::Member parser::parseStructMember()
    {
        Type *ty = baseType(nullptr);
        auto pair = declarator(ty);
        expect(TokenType::T_SEMI, ";");
        StructType::Member mem(pair.second, pair.first, true);

        return mem;
    }
    // enum-specifier = ident? "{" enum-list? "}"
    //                | ident ("{" enum-list? "}")?
    //
    // enum-list      = ident ("=" num)? ("," ident ("=" num)?)* ","?
    Type *parser::parseEnumSpecifier()
    {
        expect(TokenType::T_ENUM, "enum"); //期待关键字enum
        Type *ty = enum_type;
        ;
        bool anmous = true;
        Token t;
        if (is(TokenType::T_IDENT))
        {
            //enum identi;
            t = consume();
            anmous = false;
        }

        if (!anmous && !is(TokenType::T_LBRACE))
        {
            //enum test;
            //auto s = consume();
            //  ERROR_EXIT<<s.tostring()<<"   kkk";
            auto identi = currentScop_->findTagInAllScope(t.getText());
            if (identi == nullptr)
            {
                //没有找到enum的定义
                ERROR_EXIT << " unknow enum type";
            }
            if (identi->type_->isKindOf(Type::T_ENUM) == false)
            {
                //不是enum 类型
                ERROR_EXIT << "not an enum tag:" << t.getText();
            }
            return identi->type_;
        }
        expect(TokenType::T_LBRACE, "{");
        int value = 0;
        int i = 0;
        while (!match(TokenType::T_RBRACE)) //"遇到}则结束"
        {
            //enum RGB
            //{
            //                   R = 1,
            //                    G = 2,
            //                   B = 3
            //}
            if (i++ > 0)
            {
                expect(TokenType::T_COMMA, ",");
            }

            auto str = consume().getText();
            if (match(TokenType::T_ASSIGN))
            {
                value = constExpr<int>();
            }

            auto enumerator = Enumerator::create(str, value++);
            if (currentScop_->findTagInCurrentScope(str))
            {
                ERROR_EXIT << "redefined the enumerator :" << str;
            }
            currentScop_->addSymoble(str, enumerator);
        }
        if (!anmous)
        {

            currentScop_->addTag(t.getText(), Identifier::create(t.getText(), ty, false));
        }
        return ty;
    }
    Type *parser::parseUnionSpecifier()
    {

        expect(TokenType::T_UNION, "union");
        auto *struct_ty = structUnionDecl()->castToStruct();

        for (auto &mem : struct_ty->getMembers())
        {
            if (mem.type_->isIncomplete())
            {
                ERROR_EXIT << "incomplete struct member " << mem.name_;
            }

            if (struct_ty->getalign() < mem.type_->getalign())
            {
                struct_ty->setalign(mem.type_->getalign());
            }
            if (struct_ty->getsize() < mem.type_->getsize())
            {
                struct_ty->setSize(mem.type_->getsize());
            }
        }
        struct_ty->setSize(align_to(struct_ty->getsize(), struct_ty->getalign())); //对齐
        struct_ty->setIncomplete(false);
        return struct_ty;
    }
    bool parser::isInteger(Expr *node)
    {

        return node->type_->isOnekindOf(Type::T_INT, Type::T_CHAR, Type::T_LONG, Type::T_SHORT);
    }
    bool parser::isString(Type *ty)
    {

        return ty->isKindOf(Type::T_ARRAY) && ty->castToArray()->getBaseType()->isKindOf(Type::T_CHAR);
    }

} // namespace Yan
