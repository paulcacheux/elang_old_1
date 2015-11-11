#include <elang/type.hpp>

namespace elang {

Type::Type(Type::Variety var) : variety(var) {
}

BuiltinType::BuiltinType(BuiltinType::Kind kind)
    : Type(Type::Variety::Builtin), kind(kind) {
}

std::string BuiltinType::toString() const {
    switch (kind) {
    case BuiltinType::Kind::Void_ty:
        return "void";
    case BuiltinType::Kind::Int_ty:
        return "int";
    case BuiltinType::Kind::Double_ty:
        return "double";
    case BuiltinType::Kind::Char_ty:
        return "char";
    case BuiltinType::Kind::Bool_ty:
        return "bool";
    }
}

ArrayType::ArrayType(Type* subtype, std::size_t size)
    : Type(Type::Variety::Array), subtype(subtype), size(size) {
}

std::string ArrayType::toString() const {
    return "[" + subtype->toString() + " ; " + std::to_string(size) + "]";
}

PointerType::PointerType(Type* subtype)
    : Type(Type::Variety::Pointer), subtype(subtype) {
}

std::string PointerType::toString() const {
    return "*" + subtype->toString();
}

LValueType::LValueType(Type* subtype)
    : Type(Type::Variety::LValue), subtype(subtype) {
}

std::string LValueType::toString() const {
    return subtype->toString() + "<lval>";
}

FunctionType::FunctionType(Type* return_ty, std::vector<Type*> params_ty)
    : Type(Type::Variety::Function), return_type(return_ty),
      params_types(std::move(params_ty)) {
}

std::string FunctionType::toString() const {
    std::string str = "(";

    bool first_param = true;
    for (auto ty : params_types) {
        if (first_param) {
            first_param = false;
        } else {
            str += ", ";
        }
        str += ty->toString();
    }
    str += ") -> " + return_type->toString();
    return str;
}

TypeManager::TypeManager()
    : _void_ty(BuiltinType::Kind::Void_ty), _int_ty(BuiltinType::Kind::Int_ty),
      _double_ty(BuiltinType::Double_ty), _char_ty(BuiltinType::Kind::Char_ty),
      _bool_ty(BuiltinType::Kind::Bool_ty) {
}

TypeManager::~TypeManager() {
    for (auto array_ty : _array_types)
        delete array_ty.second;

    for (auto ptr_ty : _ptr_types)
        delete ptr_ty.second;
}

BuiltinType* TypeManager::getVoidType() {
    return &_void_ty;
}

BuiltinType* TypeManager::getIntType() {
    return &_int_ty;
}

BuiltinType* TypeManager::getDoubleType() {
    return &_double_ty;
}

BuiltinType* TypeManager::getCharType() {
    return &_char_ty;
}

BuiltinType* TypeManager::getBoolType() {
    return &_bool_ty;
}

ArrayType* TypeManager::getArrayType(Type* subtype, std::size_t size) {
    auto id = std::make_pair(subtype, size);
    if (_array_types.find(id) != _array_types.end()) {
        return _array_types[id];
    }
    _array_types[id] = new ArrayType(subtype, size);
    return _array_types[id];
}

PointerType* TypeManager::getPointerType(Type* subtype) {
    if (_ptr_types.find(subtype) != _ptr_types.end()) {
        return _ptr_types[subtype];
    }
    _ptr_types[subtype] = new PointerType(subtype);
    return _ptr_types[subtype];
}

LValueType* TypeManager::getLValueType(Type* subtype) {
    if (_lval_types.find(subtype) != _lval_types.end()) {
        return _lval_types[subtype];
    }
    _lval_types[subtype] = new LValueType(subtype);
    return _lval_types[subtype];
}

FunctionType* TypeManager::getFunctionType(Type* ret_ty,
                                           std::vector<Type*> param_ty) {
    auto id = std::make_pair(ret_ty, param_ty);
    if (_func_types.find(id) != _func_types.end()) {
        return _func_types[id];
    }
    _func_types[id] = new FunctionType(ret_ty, param_ty);
    return _func_types[id];
}

} // namespace elang
