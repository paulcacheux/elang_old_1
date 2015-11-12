#ifndef ELANG_SYMBOL_TABLE_H
#define ELANG_SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <map>

#include <elang/type.hpp>

namespace elang {

class LocalTable {
    std::vector<std::map<std::string, Type*>> _scopes;

  public:
    LocalTable();
    ~LocalTable();

    void beginScope();
    void endScope();

    Type* get(const std::string& name);
    bool put(const std::string& name,
             Type* ty); // return false if name is already defined IN LAST
                        // SCOPE else return true
};

class GlobalTable {
  public:
    enum State { Defined, Declared, None };

  private:
    std::map<std::string, std::pair<Type*, State>> _globals;
    std::vector<std::string> _current_module_path;

  public:
    GlobalTable() = default;

    bool beginModule(const std::string& name); // return false if name is
                                               // already in module path else
                                               // return true
    void endModule();

    Type* get(std::vector<std::string>& mod_path, const std::string& name);
    std::pair<Type*, State> getStateInModule(const std::string& name);

    void declare(const std::string& name, Type* ty);
    void define(const std::string& name, Type* ty);
};

} // namespace elang

#endif // ELANG_SYMBOL_TABLE_H
