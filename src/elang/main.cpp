#include <iostream>
#include <string>

#include <elang/source_manager.hpp>
#include <elang/lexer.hpp>
#include <elang/parser.hpp>
#include <elang/debug_visitor.hpp>
#include <elang/ast.hpp>

void usage(char* exec) {
    std::cerr << "Usage: " << exec << " input-file.c";
}

std::unique_ptr<elang::ast::Module>
loadModule(elang::SourceManager* sm, std::string path, std::string name) {
    unsigned index;
    if (path == "-")
        index = sm->registerStdin();
    else
        index = sm->registerFile(path);

    elang::Lexer lexer{sm, index};
    elang::Parser parser{&lexer, sm};
    auto mod = parser.parseTranslationUnit();

    std::vector<std::unique_ptr<elang::ast::Module>> imported_modules;
    for (auto& import : mod->imports) {
        std::string import_path = import->path + ".el";
        imported_modules.push_back(
            std::move(loadModule(sm, import_path, import->path)));
    }
    return std::make_unique<elang::ast::Module>(
        name, std::move(imported_modules), std::move(mod->func_decls));
}

int main(int argc, char** argv) {
    std::string path
        = (argc <= 1 || std::string{argv[1]} == "-") ? "-" : argv[1];

    elang::SourceManager source_manager;
    elang::ast::DebugVisitor debug_visitor;
    auto main_module = loadModule(&source_manager, path, "main");
    main_module->accept(&debug_visitor);
}
