#include <elang/debug_visitor.hpp>

#include <iostream>
#include <ostream>
#include <string>

namespace elang {
namespace ast {

std::ostream& operator<<(std::ostream& out, BinaryOperator::Kind kind) {
    switch (kind) {
    case BinaryOperator::Kind::Assign:
        return out << "=";
    case BinaryOperator::Kind::LessOrEqual:
        return out << "<=";
    case BinaryOperator::Kind::Less:
        return out << "<";
    case BinaryOperator::Kind::Greater:
        return out << ">";
    case BinaryOperator::Kind::GreaterOrEqual:
        return out << ">=";
    case BinaryOperator::Kind::Equal:
        return out << "==";
    case BinaryOperator::Kind::Different:
        return out << "!=";
    case BinaryOperator::Kind::Add:
        return out << "+";
    case BinaryOperator::Kind::Minus:
        return out << "-";
    case BinaryOperator::Kind::Times:
        return out << "*";
    case BinaryOperator::Kind::Divide:
        return out << "/";
    case BinaryOperator::Kind::Modulo:
        return out << "%";
    case BinaryOperator::Kind::LogicalOr:
        return out << "||";
    case BinaryOperator::Kind::LogicalAnd:
        return out << "&&";
    }
}

std::ostream& operator<<(std::ostream& out, UnaryOperator::Kind kind) {
    switch (kind) {
    case UnaryOperator::Kind::Plus:
        return out << "+";
    case UnaryOperator::Kind::Minus:
        return out << "-";
    case UnaryOperator::Kind::LogicalNot:
        return out << "!";
    case UnaryOperator::Kind::PtrDeref:
        return out << "*";
    case UnaryOperator::Kind::AddressOf:
        return out << "&";
    }
}

void DebugVisitor::increaseTab() {
    current_tab += "    ";
}
void DebugVisitor::decreaseTab() {
    current_tab.pop_back();
    current_tab.pop_back();
    current_tab.pop_back();
    current_tab.pop_back();
}

void DebugVisitor::visit(BinaryOperator* node) {
    std::cout << "(";
    node->lhs->accept(this);
    std::cout << " " << node->kind << " ";
    node->rhs->accept(this);
    std::cout << ")";
}

void DebugVisitor::visit(UnaryOperator* node) {
    std::cout << "(" << node->kind;
    node->expr->accept(this);
    std::cout << ")";
}

void DebugVisitor::visit(SubscriptExpression* node) {
    std::cout << "(";
    node->subscripted->accept(this);
    std::cout << ")";
    std::cout << "[";
    node->index->accept(this);
    std::cout << "]";
    return;
}

void DebugVisitor::visit(CallExpression* node) {
    node->func->accept(this);
    std::cout << "(";

    bool first_arg = true;
    for (auto& arg : node->args) {
        if (first_arg) {
            first_arg = false;
        } else {
            std::cout << ", ";
        }
        arg->accept(this);
    }
    std::cout << ")";
}

void DebugVisitor::visit(CastExpression* node) {
    std::cout << "(";
    node->casted->accept(this);
    std::cout << " as " << node->to_type->toString() << ")";
}

void DebugVisitor::visit(LValueToRValueCastExpression* node) {
    std::cout << "(";
    node->lvalue->accept(this);
    std::cout << " to rvalue";
}

void DebugVisitor::visit(IdentifierReference* node) {
    for (auto& str : node->module_path)
        std::cout << str << " :: ";
    std::cout << node->name;
}

void DebugVisitor::visit(IntLiteral* node) {
    std::cout << node->value;
}

void DebugVisitor::visit(DoubleLiteral* node) {
    std::cout << node->value;
}

void DebugVisitor::visit(CharLiteral* node) {
    std::cout << "\'" << node->value << "\'";
}

void DebugVisitor::visit(StringLiteral* node) {
    std::cout << "\"" << node->value << "\"";
}

void DebugVisitor::visit(BoolLiteral* node) {
    std::cout << std::boolalpha << node->value << std::noboolalpha;
}

void DebugVisitor::visit(CompoundStatement* node) {
    std::cout << current_tab << "{\n";
    increaseTab();
    for (auto& stmt : node->stmts) {
        stmt->accept(this);
        std::cout << "\n";
    }
    decreaseTab();
    std::cout << current_tab << "}\n";
}

void DebugVisitor::visit(LetStatement* node) {
    std::cout << current_tab << "let " << node->name << " : "
              << node->type->toString();
    if (node->init_expr) {
        std::cout << " = ";
        node->init_expr->accept(this);
    }
    std::cout << ";";
}

void DebugVisitor::visit(ExpressionStatement* node) {
    std::cout << current_tab;
    if (node->expr) {
        node->expr->accept(this);
    }
    std::cout << ";";
}

void DebugVisitor::visit(SelectionStatement* node) {
    bool first_if = true;
    for (auto& choice : node->choices) {
        if (first_if) {
            std::cout << current_tab << "if ";
            first_if = false;
        } else {
            std::cout << current_tab << "else if";
        }
        choice.first->accept(this);
        std::cout << "\n";
        choice.second->accept(this);
    }

    if (node->else_stmt) {
        std::cout << current_tab << "else\n";
        node->else_stmt->accept(this);
    }
}

void DebugVisitor::visit(IterationStatement* node) {
    std::cout << current_tab << "while ";
    node->condition->accept(this);
    std::cout << "\n";
    node->stmt->accept(this);
}

void DebugVisitor::visit(ReturnStatement* node) {
    std::cout << current_tab << "return";
    if (node->expr) {
        std::cout << " ";
        node->expr->accept(this);
    }
    std::cout << ";";
}

void DebugVisitor::visit(ImportDeclaration* node) {
    std::cout << "import " << node->path << "\n";
}

void DebugVisitor::visit(FunctionDeclaration* node) {
    std::cout << "func " << node->name << " " << node->type->toString();
}

void DebugVisitor::visit(ExternFunctionDeclaration* node) {
    std::cout << "extern func " << node->name << " " << node->type->toString();
}

void DebugVisitor::visit(FunctionDefinition* node) {
    std::cout << "func " << node->name;
    std::cout << "(";

    bool first_param = true;
    for (std::size_t i = 0; i < node->param_names.size(); ++i) {
        if (first_param) {
            first_param = false;
        } else {
            std::cout << ", ";
        }
        std::cout << node->param_names[i] << " : "
                  << node->type->params_types[i]->toString();
    }
    std::cout << ") -> " << node->type->return_type->toString();

    std::cout << "\n";
    node->content_stmt->accept(this);
}

void DebugVisitor::visit(TranslationUnit* node) {
    std::cout << ">>> translation unit";
    for (auto& decl : node->imports) {
        decl->accept(this);
    }

    for (auto& decl : node->func_decls) {
        decl->accept(this);
        std::cout << "\n";
    }
}

void DebugVisitor::visit(Module* node) {
    for (auto& decl : node->imported_modules) {
        decl->accept(this);
    }

    std::cout << ">>> module " << node->name << "\n";
    for (auto& decl : node->functions) {
        decl->accept(this);
        std::cout << "\n";
    }
}

} // namespace ast
} // namespace elang
