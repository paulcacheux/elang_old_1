#include <elang/sema_visitor.hpp>

std::unique_ptr<elang::ast::Expression>
addL2RCast(std::unique_ptr<elang::ast::Expression>&& expr);

namespace elang {
namespace ast {

SemaVisitor::SemaVisitor(SourceManager* sm)
    : _type_manager(sm->getTypeManager()),
      _diag_engine(sm->getDiagnosticEngine()) {
}

void SemaVisitor::visit(BinaryOperator* node) {
    node->lhs->accept(this);
    node->rhs->accept(this);
    if (node->kind == BinaryOperator::Kind::Assign) {
        if (node->lhs->type->variety != Type::Variety::LValue) {
            _diag_engine->report(node->location, 3001);
            node->type = node->lhs->type;
            return;
        }
        auto lhs_ty = static_cast<LValueType*>(node->lhs->type)->subtype;
        node->rhs = addL2RCast(std::move(node->rhs));
        if (lhs_ty != node->rhs->type) {
            _diag_engine->report(node->location, 3002, lhs_ty->toString(),
                                 node->rhs->type->toString());
        }
        node->type = lhs_ty;
    } else if (node->kind == BinaryOperator::Kind::LessOrEqual
               || node->kind == BinaryOperator::Kind::Less
               || node->kind == BinaryOperator::Kind::GreaterOrEqual
               || node->kind == BinaryOperator::Kind::Greater
               || node->kind == BinaryOperator::Kind::Equal
               || node->kind == BinaryOperator::Kind::Different) {
        node->lhs = addL2RCast(std::move(node->lhs));
        node->rhs = addL2RCast(std::move(node->rhs));

        if (node->lhs->type != node->rhs->type) {
            _diag_engine->report(node->location, 3003);
        }
        node->type = _type_manager->getBoolType();
    } else if (node->kind == BinaryOperator::Kind::Add
               || node->kind == BinaryOperator::Kind::Minus
               || node->kind == BinaryOperator::Kind::Times
               || node->kind == BinaryOperator::Kind::Divide
               || node->kind == BinaryOperator::Kind::Modulo) {
        node->lhs = addL2RCast(std::move(node->lhs));
        node->rhs = addL2RCast(std::move(node->rhs));

        if (node->lhs->type != node->rhs->type) {
            _diag_engine->report(node->location, 3004);
        }
        node->type = node->lhs->type;
    } else if (node->kind == BinaryOperator::Kind::LogicalAnd
               || node->kind == BinaryOperator::Kind::LogicalOr) {
        node->lhs = addL2RCast(std::move(node->lhs));
        node->rhs = addL2RCast(std::move(node->rhs));

        auto bool_ty = _type_manager->getBoolType();
        if (node->lhs->type != bool_ty || node->rhs->type != bool_ty) {
            _diag_engine->report(node->location, 3005);
        }
        node->type = bool_ty;
    }
}

void SemaVisitor::visit(UnaryOperator* node) {
    node->expr->accept(this);
    if (node->kind == UnaryOperator::Kind::Plus
        || node->kind == UnaryOperator::Kind::Minus) {
        node->expr = addL2RCast(std::move(node->expr));
        if (node->expr->type != _type_manager->getIntType()
            || node->expr->type != _type_manager->getDoubleType()) {
            _diag_engine->report(node->location, 3006);
        }
        node->type = node->expr->type;
    } else if (node->kind == UnaryOperator::Kind::LogicalNot) {
        node->expr = addL2RCast(std::move(node->expr));
        if (node->expr->type != _type_manager->getBoolType()) {
            _diag_engine->report(node->location, 3007);
        }
        node->type = node->expr->type;
    } else if (node->kind == UnaryOperator::Kind::PtrDeref) {
        node->expr = addL2RCast(std::move(node->expr));
        if (node->expr->type->variety != Type::Variety::Pointer) {
            _diag_engine->report(node->location, 3008);
            node->type = node->expr->type;
            return;
        }
        auto ptr_subty = static_cast<PointerType*>(node->expr->type)->subtype;
        node->type = _type_manager->getLValueType(ptr_subty);
    } else if (node->kind == UnaryOperator::Kind::AddressOf) {
        if (node->expr->type->variety != Type::Variety::LValue) {
            _diag_engine->report(node->location, 3009);
            node->type = node->expr->type;
            return;
        }
        node->type = _type_manager->getPointerType(
            static_cast<LValueType*>(node->expr->type)->subtype);
    }
}

void SemaVisitor::visit(SubscriptExpression* node) {
    node->subscripted->accept(this);
    node->index->accept(this);
    node->subscripted = addL2RCast(std::move(node->subscripted));
    node->index = addL2RCast(std::move(node->index));

    if (node->index->type != _type_manager->getIntType()) {
        _diag_engine->report(node->index->location, 3010);
    }

    if (node->subscripted->type->variety == Type::Variety::Pointer) {
        node->type = _type_manager->getLValueType(
            static_cast<PointerType*>(node->subscripted->type)->subtype);
    } else if (node->subscripted->type->variety == Type::Variety::Array) {
        node->type = _type_manager->getLValueType(
            static_cast<ArrayType*>(node->subscripted->type)->subtype);
    } else {
        _diag_engine->report(node->subscripted->location, 3011,
                             node->subscripted->type->toString());
        node->type = node->subscripted->type;
    }
}

void SemaVisitor::visit(CallExpression* node) {
    node->func->accept(this);
    if (node->func->type->variety != Type::Variety::Function) {
        _diag_engine->report(node->location, 3012,
                             node->func->type->toString());
        node->type = _type_manager->getIntType();
        return;
    }
    auto func_ty = static_cast<FunctionType*>(node->func->type);

    std::vector<Type*> args_ty;
    args_ty.reserve(node->args.size());
    for (auto& arg : node->args) {
        arg->accept(this);
        arg = addL2RCast(std::move(arg));
        args_ty.push_back(arg->type);
    }
    if (args_ty != func_ty->params_types) {
        _diag_engine->report(node->location, 3013);
        node->type = _type_manager->getIntType();
        return;
    }
    node->type = func_ty->return_type;
}

void SemaVisitor::visit(CastExpression* node) {
    // TODO: check if this cast is possible
    node->type = node->to_type;
}

void SemaVisitor::visit(LValueToRValueCastExpression* node) {
    node->lvalue->accept(this);
    auto ty = node->lvalue->type;
    if (ty->variety != Type::Variety::LValue) {
        _diag_engine->report(node->location, 0);
        std::exit(-1);
    }
    node->type = static_cast<LValueType*>(ty)->subtype;
}

void SemaVisitor::visit(IdentifierReference* node) {
    Type* ty = nullptr;
    if (node->module_path.empty()) {
        ty = _local_table->get(node->name);
    }

    if (!ty) {
        ty = _global_table.get(node->module_path, node->name);
    }

    if (!ty) {
        _diag_engine->report(node->location, 3014, node->name);
        node->type = _type_manager->getIntType();
        return;
    }

    if (ty->variety == Type::Variety::Function) {
        node->type = ty;
    } else {
        node->type = _type_manager->getLValueType(ty);
    }
}

void SemaVisitor::visit(IntLiteral* node) {
    node->type = _type_manager->getIntType();
}

void SemaVisitor::visit(DoubleLiteral* node) {
    node->type = _type_manager->getDoubleType();
}

void SemaVisitor::visit(CharLiteral* node) {
    node->type = _type_manager->getCharType();
}

void SemaVisitor::visit(StringLiteral* node) {
    node->type = _type_manager->getPointerType(_type_manager->getCharType());
}

void SemaVisitor::visit(BoolLiteral* node) {
    node->type = _type_manager->getBoolType();
}

void SemaVisitor::visit(CompoundStatement* node) {
    _local_table->beginScope();
    for (auto& stmt : node->stmts) {
        stmt->accept(this);
    }
    _local_table->endScope();
}

void SemaVisitor::visit(LetStatement* node) {

    if (node->init_expr) {
        node->init_expr->accept(this);
        node->init_expr = addL2RCast(std::move(node->init_expr));
        if (!node->type) {
            node->type = node->init_expr->type;
        } else if (node->init_expr->type != node->type) {
            _diag_engine->report(node->location, 3015, node->name,
                                 node->init_expr->type->toString(),
                                 node->type->toString());
            return;
        }
    }

    if (!_local_table->put(node->name, node->type)) {
        _diag_engine->report(node->location, 3016, node->name);
        return;
    }
}

void SemaVisitor::visit(ExpressionStatement* node) {
    if (node->expr) {
        node->expr->accept(this);
    }
}

void SemaVisitor::visit(SelectionStatement* node) {
    for (auto& choice : node->choices) {
        choice.first->accept(this);
        choice.first = addL2RCast(std::move(choice.first));
        if (choice.first->type != _type_manager->getBoolType()) {
            _diag_engine->report(node->location, 3021);
        }
        choice.second->accept(this);
    }
    if (node->else_stmt) {
        node->else_stmt->accept(this);
    }
}

void SemaVisitor::visit(IterationStatement* node) {
    node->condition->accept(this);
    node->condition = addL2RCast(std::move(node->condition));
    if (node->condition->type != _type_manager->getBoolType()) {
        _diag_engine->report(node->location, 3022);
    }
    node->stmt->accept(this);
}

void SemaVisitor::visit(ReturnStatement* node) {
    if (node->expr) {
        node->expr->accept(this);
        node->expr = addL2RCast(std::move(node->expr));
        if (node->expr->type != _current_return_ty) {
            _diag_engine->report(node->location, 3023,
                                 node->expr->type->toString(),
                                 _current_return_ty->toString());
        }
    } else {
        if (_current_return_ty != _type_manager->getVoidType()) {
            _diag_engine->report(node->location, 3023,
                                 _type_manager->getVoidType()->toString(),
                                 _current_return_ty->toString());
        }
    }
}

void SemaVisitor::visit(FunctionDeclaration* node) {
    auto current_state = _global_table.getStateInModule(node->name);
    if (current_state.second == GlobalTable::State::Defined) {
        _diag_engine->report(node->location, 3018, node->name);
    } else if (current_state.second == GlobalTable::State::Declared
               && current_state.first != node->type) {
        _diag_engine->report(node->location, 3019, node->name);
    } else {
        _global_table.declare(node->name, node->type);
    }
}

void SemaVisitor::visit(FunctionDefinition* node) {
    auto current_state = _global_table.getStateInModule(node->name);
    if (current_state.second == GlobalTable::State::Defined) {
        _diag_engine->report(node->location, 3018, node->name);
    } else if (current_state.second == GlobalTable::State::Declared
               && current_state.first != node->type) {
        _diag_engine->report(node->location, 3019, node->name);
    } else {
        _global_table.define(node->name, node->type);
        _local_table = std::make_unique<LocalTable>();
        for (std::size_t i = 0; i < node->param_names.size(); ++i) {
            if (!_local_table->put(node->param_names[i],
                                   node->type->params_types[i])) {
                _diag_engine->report(node->location, 3017,
                                     node->param_names[i]);
            }
        }
        _current_return_ty = node->type->return_type;
        node->content_stmt->accept(this);
        _local_table.reset();
    }
}

void SemaVisitor::visit(Module* node) {
    if (!_global_table.beginModule(node->name)) {
        _diag_engine->report(node->location, 3020, node->name);
        return;
    }

    for (auto& decl : node->declarations) {
        decl->accept(this);
    }

    _global_table.endModule();
}

} // namespace ast
} // namespace elang

// utils
std::unique_ptr<elang::ast::Expression>
addL2RCast(std::unique_ptr<elang::ast::Expression>&& expr) {
    if (expr->type->variety == elang::Type::Variety::LValue) {
        auto loc = expr->location;
        auto ty = static_cast<elang::LValueType*>(expr->type)->subtype;
        auto l2r_cast
            = std::make_unique<elang::ast::LValueToRValueCastExpression>(
                std::move(expr), loc);
        expr = std::move(l2r_cast);
        expr->type = ty;
    }
    return std::move(expr);
}
