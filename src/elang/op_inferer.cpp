#include <elang/op_inferer.hpp>

#include <elang/type.hpp>

// Binary Op:
// IntAdd DoubleAdd ok
// IntSub DoubleSub ok
// IntMul DoubleMul ok
// IntDiv DoubleDiv ok
// IntMod ok
// IntLess DoubleLess ok
// IntLessEqu DoubleLessEqu ok
// IntGreater DoubleGreater ok
// IntGreaterEqu DoubleGreaterEqu ok
// IntEqu DoubleEqu CharEqu BoolEqu ok
// IntDiff DoubleDiff CharDiff BoolDiff ok
// BoolAnd BoolOr
//
// Unary Op:
// IntPlus IntMinus
// DoublePlus DoubleMinus
// BoolNot
// PtrDeref AddressOf

namespace elang {

OpInferer::OpInferer(TypeManager* tm) : _type_manager(tm) {
}

Type* OpInferer::inferBinaryAddOrMinus(Type* lhs_ty, Type* rhs_ty) {
    auto int_ty = _type_manager->getIntType();
    auto double_ty = _type_manager->getDoubleType();
    if (lhs_ty == int_ty && rhs_ty == int_ty) {
        return int_ty;
    } else if (lhs_ty == double_ty && rhs_ty == double_ty) {
        return double_ty;
    }

    if (lhs_ty->variety == Type::Variety::Pointer && rhs_ty == int_ty) {
        return lhs_ty;
    }
    return nullptr;
}

Type* OpInferer::inferBinaryTimesOrDivide(Type* lhs_ty, Type* rhs_ty) {
    auto int_ty = _type_manager->getIntType();
    auto double_ty = _type_manager->getDoubleType();
    if (lhs_ty == int_ty && rhs_ty == int_ty) {
        return int_ty;
    } else if (lhs_ty == double_ty && rhs_ty == double_ty) {
        return double_ty;
    }
    return nullptr;
}

Type* OpInferer::inferBinaryModulo(Type* lhs_ty, Type* rhs_ty) {
    auto int_ty = _type_manager->getIntType();
    if (lhs_ty == int_ty && rhs_ty == int_ty) {
        return int_ty;
    }
    return nullptr;
}

Type* OpInferer::inferBinaryEqualOrDifferent(Type* lhs_ty, Type* rhs_ty) {
    if (lhs_ty != rhs_ty)
        return nullptr;

    if (lhs_ty == _type_manager->getVoidType())
        return nullptr;

    return _type_manager->getBoolType();
}

Type* OpInferer::inferBinaryComp(Type* lhs_ty, Type* rhs_ty) {
    auto int_ty = _type_manager->getIntType();
    auto double_ty = _type_manager->getDoubleType();
    if ((lhs_ty == int_ty && rhs_ty == int_ty)
        || (lhs_ty == double_ty && rhs_ty == double_ty)) {
        return _type_manager->getBoolType();
    }
    return nullptr;
}

Type* OpInferer::inferBinaryAndOrOr(Type* lhs_ty, Type* rhs_ty) {
    auto bool_ty = _type_manager->getBoolType();
    if (lhs_ty == bool_ty && rhs_ty == bool_ty) {
        return bool_ty;
    }
    return nullptr;
}

Type* OpInferer::inferUnaryPlusOrMinus(Type* ty) {
    auto int_ty = _type_manager->getIntType();
    auto double_ty = _type_manager->getDoubleType();

    if (ty == int_ty || ty == double_ty) {
        return ty;
    }
    return nullptr;
}

Type* OpInferer::inferUnaryNot(Type* ty) {
    if (ty == _type_manager->getBoolType()) {
        return ty;
    }
    return nullptr;
}

Type* OpInferer::inferUnaryPtrDeref(Type* ty) {
    if (ty->variety == Type::Variety::Pointer) {
        return static_cast<PointerType*>(ty)->subtype;
    }
    return nullptr;
}

} // namespace elang
