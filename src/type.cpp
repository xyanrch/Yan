#include "type.h"
#include"AST.h"
namespace Yan
{
  
 IntType*     int_type = IntType::create();
 VoidType*    void_type = VoidType::create();
 BoolType*    bool_type = BoolType::create();
 CharType*    char_type = CharType::create();
 ShortType*   short_type = ShortType::create();
 LongType*    long_type = LongType::create();
 EnumType*    enum_type  = EnumType::create();
Type::Type(int size, int align, TypeKind kind, bool incomplete):size_(size),
align_(align), kind_(kind),isIncomplete_(incomplete)
{

}

Type::TypeKind Type::getKind()
{
    return kind_;
}
int Type::getalign()const
{
    return align_;
}
int Type:: getsize()const 
{
    return size_;
}
std::string Type::tostring()
{
    switch(kind_)
    {
        case T_VOID: return "void";

        case T_BOOL: return "bool";
        case T_CHAR: return "char";
        case T_SHORT:return "short";
        case T_INT: return "int";
        case T_LONG: return "long";

        case T_ENUM: return "enum";
        case T_PTR: return "ptr";
        case T_ARRAY:return "array";
        case T_STRUCT:return "struct";
        case T_FUNC: return "func";
    }
}


PtrType* PtrType::create(Type* base)
{ 
    return new PtrType(base);
}

PtrType::PtrType(Type* base):DerivedType(8,8,T_PTR,base)
{

}

ArrayType* ArrayType::create(Type*base, int len)
{ 
    return new ArrayType(base,len);
}

 ArrayType::ArrayType(Type*base, int len):DerivedType(base->getsize()*len,base->getalign(),T_ARRAY, base),len_(len)
 {

 }

void FuncType::addParam(Identifier* param)
{
    paramList_.push_back(param);
}
std::vector<Identifier*>& FuncType::getParam()
{
    return paramList_;
}
FuncType* FuncType::create(Type* returnType)
{ 
    return new FuncType(returnType); 
}

FuncType::FuncType(Type* returnType):DerivedType(4,4,T_FUNC, returnType) 
{

}

StructType* StructType::create()
{
     return new StructType();
}
void StructType::addMember(const StructType::Member& memb)
{
    members_.push_back(memb);
}
StructType::StructType():Type(0,4,T_STRUCT,true)
{

}
 Identifier* StructType::findMember(const std::string& name)
        {
            auto find = std::find_if(members_.begin(),members_.end(),[&name](Member& m)
            {
                return name == m.name_;
            });

            if(find != members_.end())
            {
                return find.base();
            }
            else
            {
                return nullptr;
            }
        }


 Type* mayCasttoPtr(Type* ty)
 {
     if(ty->castToDeried() == nullptr || ty->isKindOf(Type::T_PTR))
     {
         return ty;
     }
     return PtrType::create(ty->castToDeried()->getBaseType());
 }
}