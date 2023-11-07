#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "type_checker.hpp"

#include <fstream>

int main(int argc, const char* argv[]) {
    std::ifstream file;
    std::string path = argv[1];

    file.open(path, std::ios::binary);

    koala::lexer lexer(file, path);
    koala::parser parser(lexer);
    koala::type_checker type_checker(parser);

    lexer.lex();
    parser.parse();
    type_checker.check();

    parser.execute_function("main");
}