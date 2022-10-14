#include <assert.h>
#include "symbol.h"
#include "log.h"

namespace Yan
{

    Scope::Scope()
    {
        parent_ = nullptr;
    }
    Scope::~Scope()
    {
    }
    Scope *Scope::getGlobalScope()
    {
        if (kind_ == ScopeKind::GLOBAL)
        {
            return this;
        }
        auto temp = parent_;
        while (temp)
        {
            if (temp->getScope() == ScopeKind::GLOBAL)
            {
                return temp;
            }
            temp = temp->parent_;
        }
        return temp;
    }
    bool Scope::stringLitExist(const std::string &str)
    {
        assert(kind_ == ScopeKind::GLOBAL);
        auto result = std::find_if(stringLit_.begin(), stringLit_.end(), [&str](std::string s) {
            return s == str;
        });
        return result != stringLit_.end();
    }
    bool Scope::addstringLit(const std::string &str)
    {
        assert(kind_ == ScopeKind::GLOBAL);
        if (stringLitExist(str))
        {
            return false;
        }
        else
        {
            stringLit_.push_back(str);
            return true;
        }
    }
    const std::vector<std::string> &Scope::getStringLitTbale() const
    {
        assert(kind_ == ScopeKind::GLOBAL);
        return stringLit_;
    }

    void Scope::addSymoble(const std::string &name, Identifier *indenti)
    {
        symbols_.push_back({name, indenti});
    }
    bool Scope::getIdentiInCurrentScope(const std::string &name, Identifier **indenti)

    {
        auto ele = std::find_if(symbols_.begin(), symbols_.end(), [&name](Symbol &s) {
            return s.first == name;
        });

        if (ele != symbols_.end())
        {
            *indenti = ele->second;
            return true;
        }
        else
        {
            return false;
        }
    }
    bool Scope::existInCurrentScope(const std::string &name)
    {
        auto ele = std::find_if(symbols_.begin(), symbols_.end(), [&name](Symbol &s) {
            return s.first == name;
        });
        return (ele != symbols_.end());
    }
    bool Scope::getIdentiInAllScope(const std::string &name, Identifier **indenti)
    {
        if (getIdentiInCurrentScope(name, indenti))
        {
            return true;
        }
        if (parent_)
        {
            return parent_->getIdentiInAllScope(name, indenti);
        }
        return false;
    }
    int Scope::caculateParentScopeOffSet(Scope *sc)
    {
        int off = 0;
        if (sc && (sc->getScope() == ScopeKind::BLOCK || (sc->getScope() == ScopeKind::FUNC)))
        {
            off += caculateParentScopeOffSet(sc->getParentScop());
            for (const auto &kv : sc->getlist())
            {
                off += kv.second->type_->getsize();
            }
        }
        else
        {
            return 0;
        }
        return off;
    }

    int Scope::caculateOffset(const std::string &name)
    {
        int off = caculateParentScopeOffSet(parent_);
        for (const auto &kv : symbols_)
        {
            off += kv.second->type_->getsize();

            if (kv.first == name)
            {
                break;
            }
        }
        return off;
    }
    int Scope::getTyepSize()
    {
        int size = 0;
        for (const auto &kv : symbols_)
        {
            Info("key=%s", kv.first.c_str());
            size += kv.second->type_->getsize();
        }
        return size;
    }

    void Scope::dumpSymbol(std::ostream &os)
    {
        os << " Current scope:" << scopeToString(kind_) << " depth: " << depth << std::endl;
        os << std::endl;
        os << std::endl;
        if (kind_ == ScopeKind::GLOBAL)
        {
            for (auto &str : stringLit_)
            {
                os << "StringLit Item  = "
                   << "\"" << str << "\"\n"
                   << std::endl;
            }
        }

        os << "name" << std::setw(20) << "Type" << std::setw(20) << "size" << std::setw(20) << "scope" << std::endl;

        for (const auto &kv : symbols_)
        {

            os << kv.first << std::setw(20) << kv.second->type_->tostring() << std::setw(20) << kv.second->type_->getsize();
            os << std::setw(20) << scopeToString(kind_) << std::endl;
        }
        //    os<<std::setiosflags(std::ios::left)<<std::endl;
    }

    Identifier *Scope::findTagInCurrentScope(const std::string &tagname)
    {
          auto find = std::find_if(tags_.begin(), tags_.end(), [&tagname](Symbol &s) {
            return s.first == tagname;
        });
         if(find != tags_.end())
         {
             return find->second;
         }
         else
         {
             return nullptr;
         }
    }
    Identifier *Scope::findTagInAllScope(const std::string &tagname)
    {
        auto tag = findTagInCurrentScope(tagname);
        if(tag)
        {
            return tag;
        }
        else if(parent_)
        {
           return parent_->findTagInCurrentScope(tagname);
        }
        else
        {
            return nullptr;
        }
    }

    void Scope::addTag(const std::string &tagname, Identifier *tag)
    {
        tags_.emplace_back(tagname,tag);
    }
    std::string scopeToString(ScopeKind s)
    {
        switch (s)
        {
        case ScopeKind::BLOCK:
            return "block";
        case ScopeKind::FUNC:
            return "function";
        case ScopeKind::GLOBAL:
            return "global";
        }
        return "";
    }

} // namespace Yan