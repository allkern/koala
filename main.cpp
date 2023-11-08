#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "type_checker.hpp"
#include "interpreter.hpp"

#include <fstream>

int main(int argc, const char* argv[]) {
    std::ifstream file;
    std::string path = argv[1];

    file.open(path, std::ios::binary);

    koala::lexer lexer(file, path);
    koala::parser parser(lexer);
    koala::type_checker type_checker(parser);
    koala::interpreter interpreter(parser);

    lexer.lex();
    parser.parse();
    type_checker.check();
    interpreter.init();

    koala::interpreter::value* return_value = interpreter.call_function(
        "main",
        {
            interpreter.make_value(100),
            interpreter.make_value("Hello, world!")
        }
    );

    if (return_value)
        printf("return_value=%s\n", return_value->str().c_str());
}