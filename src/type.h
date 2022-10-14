#ifndef TYPE_H_
#define TYPE_H_
#include <vector>
#include <string>
#include <algorithm>
namespace Yan
{

    struct Identifier;

    class PtrType;
    class ArrayType;
    class StructType;
    class FuncType;
    class DerivedType;

#define BUILD_IN_TYPE(xx, size, align, kind)     \
    class xx : public Type                       \
    {                                            \
    public:                                      \
        static xx *create() { return new xx(); } \
                                                 \
    private:                                     \
        xx() : Type(size, align, kind) {}        \
    };
    enum class storageClass
    {
        TYPE_DEF,
        EXTERN,
        STATIC,
        AUTO,
        REGISTER,
        THREAD,

        UNKNOW = 255
    };

    class Type
    {
    public:
        enum TypeKind
        {
            T_VOID,
            T_BOOL,
            T_CHAR,
            T_SHORT,
            T_INT,
            T_LONG,
            T_ENUM,
            T_PTR,
            T_ARRAY,
            T_STRUCT,//struct or union
            T_FUNC,
        };
        //默认是complete的类型，也就是说知道具体的大小
        Type(int size, int align, TypeKind kind, bool incomplete = false);
        Type() = default;
        TypeKind getKind();
        int getalign() const;
        void setalign(int align){align_ = align;}
        int getsize() const;
        void setSize(int sz){size_ = sz;}
        void setIncomplete(bool incomplete) { isIncomplete_ = incomplete; }
        std::string tostring();        
        bool isIncomplete() { return isIncomplete_; }
        bool isKindOf(TypeKind kind) const { return kind_ == kind; }
        bool isOnekindOf(TypeKind k1, TypeKind k2)
        {
            return isKindOf(k1)||isKindOf(k2);
        }
        template<typename ...TS>
        bool isOnekindOf(TypeKind k1, TypeKind k2, TS ...ts)
        {
            return isKindOf(k1)||isOnekindOf(k2,ts...);
        }

        //cast
        virtual DerivedType *castToDeried() { return nullptr; }
        virtual PtrType *castToPtr() { return nullptr; }
        virtual ArrayType *castToArray() { return nullptr; }
        virtual FuncType *castToFunc() { return nullptr; }
        virtual StructType *castToStruct() { return nullptr; }
        //
        bool isStatic(){return isStatic_;}
        void  setSatic(){isStatic_ = true;}


    private:
        int size_; //sizeo()value

        int align_;
        TypeKind kind_;
        bool isIncomplete_;
        bool isunsigned;
        bool isStatic_  = false;
    };

    class DerivedType : public Type
    {
    public:
        Type *getBaseType() { return baseType_; }
        void setBase(Type *ty) { baseType_ = ty; }
        DerivedType(int size, int align, TypeKind kind, Type *base) : Type(size, align, kind), baseType_(base) {}

        virtual DerivedType *castToDeried() { return this; }

    private:
        Type *baseType_;
    };

    class PtrType : public DerivedType
    {
    public:
        static PtrType *create(Type *base);
        virtual PtrType *castToPtr() { return this; }

    private:
        PtrType(Type *base);
    };

    class ArrayType : public DerivedType
    {
    public:
        static ArrayType *create(Type *base, int len);
        virtual ArrayType *castToArray() { return this; }

    private:
        ArrayType(Type *base, int len);
        int len_;
    };
    class FuncType : public DerivedType
    {
    public:
        void addParam(Identifier *param);
        std::vector<Identifier *> &getParam();
        static FuncType *create(Type *returnType);
        void setVarargsFlag(bool has){varargs = has;}
        bool hasVarargs()const{return varargs;}

        virtual FuncType *castToFunc() { return this; }

    private:
        FuncType(Type *returnType);
        std::vector<Identifier *> paramList_;
        bool varargs;
    };
    class StructType : public Type
    {
    public:
    using Member = Identifier;
     /*   struct Member
        {
            Type *ty_;
            std::string name_;
            int offset_;
        };*/
        static StructType *create();
        void addMember(const Member &memb);
        virtual StructType *castToStruct() { return this; }
        std::vector<Member>& getMembers(){return members_;}
        Member* findMember(const std::string& name);

    private:
        StructType();
        std::vector<Member> members_;
    };


    BUILD_IN_TYPE(IntType, 4, 4, T_INT)
    BUILD_IN_TYPE(VoidType, 1, 1, T_VOID)
    BUILD_IN_TYPE(BoolType, 1, 1, T_BOOL)
    BUILD_IN_TYPE(CharType, 1, 1, T_CHAR)
    BUILD_IN_TYPE(ShortType, 2, 2, T_SHORT)
    BUILD_IN_TYPE(LongType, 8, 8, T_LONG)
    BUILD_IN_TYPE(EnumType, 4, 4, T_ENUM)
#undef BUILD_IN_TYPE
    extern IntType *int_type;
    extern VoidType *void_type;
    extern BoolType *bool_type;
    extern CharType *char_type;
    extern ShortType *short_type;
    extern LongType *long_type;
    extern EnumType* enum_type;
    //function

    extern Type* toPtrType(Type* ty);

} // namespace Yan
#endif