#include <elang/symbol_table.hpp>

inline std::string modulePathToString(const std::vector<std::string>& path) {
    std::string result;
    for (auto& elem : path) {
        result += elem + "::";
    }
    return result;
}

namespace elang {

LocalTable::LocalTable() {
    _scopes.reserve(5);
    beginScope();
}

LocalTable::~LocalTable() {
    endScope();
}

void LocalTable::beginScope() {
    _scopes.emplace_back();
}

void LocalTable::endScope() {
    _scopes.pop_back();
}

Type* LocalTable::get(const std::string& name) {
    for (auto it = _scopes.rbegin(); it != _scopes.rend(); ++it) {
        if (it->find(name) != it->end()) {
            return (*it)[name];
        }
    }
    return nullptr;
}

bool LocalTable::put(const std::string& name, Type* ty) {
    if (_scopes.back().find(name) != _scopes.back().end()) {
        return false;
    }

    _scopes.back()[name] = ty;
    return true;
}

bool GlobalTable::beginModule(const std::string& name) {
    if (std::find(_current_module_path.begin(), _current_module_path.end(),
                  name)
        != _current_module_path.end()) {
        return false;
    }
    _current_module_path.push_back(name);
    return true;
}

void GlobalTable::endModule() {
    _current_module_path.pop_back();
}

Type* GlobalTable::get(std::vector<std::string>& mod_path,
                       const std::string& name) {
    auto added_module_path = _current_module_path;
    while (!added_module_path.empty()) {
        auto pathed_name = modulePathToString(added_module_path)
                           + modulePathToString(mod_path) + name;
        if (_globals.find(pathed_name) != _globals.end()) {
            mod_path.insert(mod_path.begin(), added_module_path.begin(),
                            added_module_path.end());
            return _globals[pathed_name].first;
        }
        added_module_path.pop_back();
    }

    return nullptr;
}

std::pair<Type*, GlobalTable::State>
GlobalTable::getStateInModule(const std::string& name) {
    auto pathed_name = modulePathToString(_current_module_path) + name;
    if (_globals.find(pathed_name) != _globals.end()) {
        return _globals[pathed_name];
    }
    return std::make_pair<Type*, State>(nullptr, State::None);
}

void GlobalTable::declare(const std::string& name, Type* ty) {
    _globals[modulePathToString(_current_module_path) + name]
        = std::make_pair(ty, State::Declared);
}

void GlobalTable::define(const std::string& name, Type* ty) {
    _globals[modulePathToString(_current_module_path) + name]
        = std::make_pair(ty, State::Defined);
}

} // namespace elang
