#include <elang/symbol_table.hpp>

inline std::string modulePathToString(const std::vector<std::string>& path) {
    std::string result;
    for (auto& elem : path) {
        result += elem + "::";
    }
    return result;
}

namespace elang {

SymbolTable::SymbolTable() {
    _scopes.reserve(5);
}

void SymbolTable::beginScope() {
    _scopes.emplace_back();
}

void SymbolTable::endScope() {
    _scopes.pop_back();
}

void SymbolTable::beginModule(const std::string& name) {
    _current_module_path.push_back(name);
}

void SymbolTable::endModule() {
    _current_module_path.pop_back();
}

Type* SymbolTable::getSymbol(std::vector<std::string>& mod_path,
                             const std::string& name) {
    for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return (*it)[name];
        }
    }

    auto added_module_path = _current_module_path;
    while (!added_module_path.empty()) {
        auto pathed_name = modulePathToString(added_module_path)
                           + modulePathToString(mod_path) + name;
        if (_globals.find(pathed_name) != _globals.end()) {
            mod_path.insert(mod_path.begin(), added_module_path.begin(),
                            added_module_path.end());
            return _globals[pathed_name];
        }
        added_module_path.pop_back();
    }

    return nullptr;
}

void SymbolTable::putSymbol(const std::string& name, Type* ty) {
    _scopes.back()[name] = ty;
}

void SymbolTable::putGlobal(const std::string& name, Type* ty) {
    _globals[modulePathToString(_current_module_path) + name] = ty;
}

bool SymbolTable::canAddSymbol(const std::string& name) {
    return _scopes.back().find(name) == _scopes.back().end();
}

bool SymbolTable::canAddGlobal(const std::string& name) {
    return _globals.find(modulePathToString(_current_module_path) + name)
           == _globals.end();
}

} // namespace elang
