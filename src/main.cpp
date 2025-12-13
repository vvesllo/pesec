#include <print>
#include <iostream>
#include <fstream>

#include "include/frontend/Lexer.hpp"
#include "include/frontend/Parser.hpp"
#include "include/frontend/Value.hpp"


std::string readfile(std::string_view filepath)
{
    std::fstream file(filepath.data());

    if (!file)
    {
        std::println("File '{}' not exists!", filepath);
        return "";
    }

    return std::string{
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };
}

int main()
{
    try 
    {
        Lexer lexer(readfile("examples/test.pesec"));
        Parser parser(lexer.process());
        
        Context context;
        
        for (auto& statement : parser.parse())
            statement->evaluate(context);
    } 
    catch (const std::runtime_error& e) 
    {
        std::println("{}", e.what());
    }
    catch (const std::exception& e) 
    {
        std::println("exception: {}", e.what());
    }
}
