#include <iostream>
#include <string>

#include <elang/source_manager.hpp>
#include <elang/lexer.hpp>
#include <elang/parser.hpp>
#include <elang/debug_visitor.hpp>
#include <elang/sema_visitor.hpp>
#include <elang/ast.hpp>

int main(int argc, char** argv) {
    std::cout.sync_with_stdio(false);
    std::string path
        = (argc <= 1 || std::string{argv[1]} == "-") ? "-" : argv[1];

    elang::SourceManager source_manager;

    unsigned index;
    if (path == "-")
        index = source_manager.registerStdin();
    else
        index = source_manager.registerFile(path);

    elang::Lexer lexer{&source_manager, index};
    elang::Parser parser{&lexer, &source_manager};

    auto main_mod = parser.parseMainModule();
    elang::ast::DebugVisitor debug_visitor;
    main_mod->accept(&debug_visitor);

    elang::ast::SemaVisitor sema_visitor{&source_manager};
    main_mod->accept(&sema_visitor);

    std::cout << "sema done" << std::endl;

    main_mod->accept(&debug_visitor);
}
