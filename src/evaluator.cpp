
namespace Yan
{
    template <typename T>
    void evaluator<T>::visit(BinaryOp *node)
    {
        T Left = evaluator<T>::eval(node->left);
        T Right = evaluator<T>::eval(node->right);
        switch (node->op)
        {
        case OpType::OP_ADD:
            result = Left + Right;
            break;
        case OpType::OP_SUBTRACT:
            result = Left - Right;
            break;
        case OpType::OP_MULTIPLY:
            result = Left * Right;
            break;
        case OpType::OP_DIVIDE:
            result = Left / Right;
            break;
        case OpType::OP_MOD:
            result = Left % Right;
            break;
        case OpType::OP_EQ:
            result = Left == Right;
            break;
        default:
            break;
        }
    }
    template <typename T>
    void evaluator<T>::visit(IntegerLiteral *node)
    {

        if (node->type_->isKindOf(Type::T_INT))
        {

            result = static_cast<int>(node->value_);
        }
        else
        {
            assert(0);
        }
    }
    template <typename T>
    void evaluator<T>::visit(UnaryOp *node)
    {
    }
    template <typename T>
    void evaluator<T>::visit(ConditionExpr *node)
    {
    }
} // namespace Yan
