#include <print>
#include <fstream>

#include "include/frontend/Lexer.hpp"
#include "include/frontend/Parser.hpp"


std::string readfile(std::string_view filepath);

int main()
{
    Lexer lexer(readfile("examples/hello.pesec"));
    
    try
    {
        lexer.tokenize();
    }
    catch (const std::runtime_error& e)
    {
        std::println("{}", e.what());
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