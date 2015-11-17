#ifndef ELANG_OP_INFERER_H
#define ELANG_OP_INFERER_H

namespace elang {

class TypeManager;
class Type;

class OpInferer {
    TypeManager* _type_manager;

  public:
    OpInferer(TypeManager* tm);
    Type* inferBinaryAddOrMinus(Type* lhs_ty, Type* rhs_ty);
    Type* inferBinaryTimesOrDivide(Type* lhs_ty, Type* rhs_ty);
    Type* inferBinaryModulo(Type* lhs_ty, Type* rhs_ty);
    Type* inferBinaryEqualOrDifferent(Type* lhs_ty, Type* rhs_ty);
    Type* inferBinaryComp(Type* lhs_ty, Type* rhs_ty);
    Type* inferBinaryAndOrOr(Type* lhs_ty, Type* rhs_ty);

    Type* inferUnaryPlusOrMinus(Type* ty);
    Type* inferUnaryNot(Type* ty);
    Type* inferUnaryPtrDeref(Type* ty);
};

} // namespace elang

#endif // ELANG_OP_INFERER_H
