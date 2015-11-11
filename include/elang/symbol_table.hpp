#ifndef ELANG_SYMBOL_TABLE_H
#define ELANG_SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <map>

#include <elang/type.hpp>

namespace elang {

class SymbolTable {
    std::vector<std::map<std::string, Type*>> _scopes;
    std::map<std::string, Type*> _globals;

    std::vector<std::string> _current_module_path;

  public:
    SymbolTable();

    void beginScope();
    void endScope();

    void beginModule(const std::string& name);
    void endModule();

    Type* getSymbol(std::vector<std::string>& mod_path,
                    const std::string& name);

    void putSymbol(const std::string& name, Type* ty);
    void putGlobal(const std::string& name, Type* ty);

    bool canAddSymbol(const std::string& name); // in last scope
    bool canAddGlobal(const std::string& name); // in current module
};

} // namespace elang

#endif // ELANG_SYMBOL_TABLE_H
