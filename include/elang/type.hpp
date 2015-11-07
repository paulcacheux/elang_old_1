#ifndef ELANG_TYPE_H
#define ELANG_TYPE_H

#include <map>
#include <vector>
#include <utility>
#include <string>
#include <memory>

namespace elang {

class Type {
  public:
    enum class Variety { Builtin, Array, Pointer, Reference, Function };

    explicit Type(Variety var);

    virtual std::string toString() const = 0;

    Variety variety;
};

class BuiltinType : public Type {
  public:
    enum Kind { Void_ty, Int_ty, Double_ty, Char_ty, Bool_ty };

  private:
    explicit BuiltinType(Kind kind);

  public:
    virtual ~BuiltinType() = default;
    virtual std::string toString() const override;

    Kind kind;

    friend class TypeManager;
};

class ArrayType : public Type {
    ArrayType(Type* subtype, std::size_t size);

  public:
    virtual ~ArrayType() = default;
    virtual std::string toString() const override;

    Type* subtype;
    std::size_t size;

    friend class TypeManager;
};

class PointerType : public Type {
    PointerType(Type* subtype);

  public:
    virtual ~PointerType() = default;
    virtual std::string toString() const override;

    Type* subtype;

    friend class TypeManager;
};

class ReferenceType : public Type {
    ReferenceType(Type* subtype);

  public:
    virtual ~ReferenceType() = default;
    virtual std::string toString() const override;

    Type* subtype;

    friend class TypeManager;
};

class FunctionType : public Type {
    FunctionType(Type* return_ty, std::vector<Type*> params_ty);

  public:
    virtual ~FunctionType() = default;
    virtual std::string toString() const override;

    Type* return_type;
    std::vector<Type*> params_types;

    friend class TypeManager;
};

class TypeManager {
    BuiltinType _void_ty;
    BuiltinType _int_ty;
    BuiltinType _double_ty;
    BuiltinType _char_ty;
    BuiltinType _bool_ty;
    std::map<std::pair<Type*, std::size_t>, ArrayType*> _array_types;
    std::map<Type*, PointerType*> _ptr_types;
    std::map<Type*, ReferenceType*> _ref_types;
    std::map<std::pair<Type*, std::vector<Type*>>, FunctionType*> _func_types;

  public:
    TypeManager();
    ~TypeManager();

    BuiltinType* getVoidType();
    BuiltinType* getIntType();
    BuiltinType* getCharType();
    BuiltinType* getBoolType();
    ArrayType* getArrayType(Type* subtype, std::size_t size);
    PointerType* getPointerType(Type* subtype);
    ReferenceType* getReferenceType(Type* subtype);
    FunctionType* getFunctionType(Type* ret_ty, std::vector<Type*> param_ty);
};

} // namespace elang

#endif // ELANG_TYPE_H
