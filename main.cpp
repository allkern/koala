#include "lexer/lexer.hpp"
#include "parser/parser.hpp"

#include <fstream>

int main(int argc, const char* argv[]) {
    std::istream* input;
    std::ostream* output;

    std::ifstream file;
    std::string path = argv[1];

    file.open(path, std::ios::binary);

    koala::lexer lexer(file, path);
    koala::parser parser(lexer);

    lexer.lex();
    parser.parse();
}