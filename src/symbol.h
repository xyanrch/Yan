#ifndef _SYMBOL_H_
#define _SYMBOL_H_
#include <array>
#include <algorithm>
#include <iomanip>
#include "token.h"
#include "AST.h"
#include "type.h"

namespace Yan
{

    //class Idetifier;

    enum class ScopeKind
    {
        GLOBAL,
        FUNC,
        BLOCK
    };
    extern std::string scopeToString(ScopeKind s);
    class Scope
    {
    public:
        int depth = 1;

    public:
        using Symbol = std::pair<std::string, Identifier *>;
        using SymbolTable = std::vector<Symbol>;
        using StringLitTable = std::vector<std::string>;
        Scope();
        ~Scope();
        Scope *getGlobalScope();
        int getdepth() { return depth; }
        //stringlit
        bool stringLitExist(const std::string &str);
        bool addstringLit(const std::string &str);
        const StringLitTable &getStringLitTbale() const;
        //new
        void addSymoble(const std::string &name, Identifier *indenti);
        bool getIdentiInCurrentScope(const std::string &name, Identifier **indenti);
        bool getIdentiInAllScope(const std::string &name, Identifier **indenti);
        
        void setParent(Scope *parent) { parent_ = parent; }
        void setScope(const ScopeKind s) { kind_ = s; }
        ScopeKind getScope() { return kind_; }
        SymbolTable &getlist() { return symbols_; }
        bool existInCurrentScope(const std::string &name);
        Scope *getParentScop() { return parent_; }
        SymbolTable::iterator begin() { return symbols_.begin(); }
        SymbolTable::iterator end() { return symbols_.end(); }

        Identifier *findTagInCurrentScope(const std::string &tagname);
        Identifier *findTagInAllScope(const std::string &tagname);
        void addTag(const std::string &tagname, Identifier *tag);

        int caculateOffset(const std::string &name);
        int getTyepSize();
        void dumpSymbol(std::ostream &os);

    private:
        int caculateParentScopeOffSet(Scope *sc);
        ScopeKind kind_;
        SymbolTable symbols_;// TODO ower the  identifier
        SymbolTable tags_;         //store defined struct union TODO ower the tags
        StringLitTable stringLit_; //only valid in global scope

        Scope *parent_;

        //disable copy and assign
        Scope(const Scope &) = delete;
        Scope &operator=(const Scope &) = delete;
    };
} // namespace Yan
#endif