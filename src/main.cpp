#include <print>
#include <fstream>

#include "include/frontend/Lexer.hpp"
#include "include/frontend/Parser.hpp"


std::string readfile(std::string_view filepath);

int main()
{
    // Lexer lexer(readfile("examples/hello.pesec"));
    
    try
    {
        Lexer lexer("2 + 2 * 2");
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        auto result = parser.parse();

        std::println("result: {}", result->evaluate().toString());
    }
    catch (const std::runtime_error& e)
    {
        std::println("{}", e.what());
    }
    catch (const std::exception& e)
    {
        std::println("compilation error: {}", e.what());
    }
}

std::string readfile(std::string_view filepath) 
{
    std::ifstream file(filepath.data());

    return std::string{
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };
}