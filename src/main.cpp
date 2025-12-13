#include <print>
#include <iostream>
#include <fstream>

#include "include/frontend/Lexer.hpp"
#include "include/frontend/Parser.hpp"
#include "include/frontend/Value.hpp"
#include "include/frontend/FunctionValue.hpp"


void init_stdio(Context& context)
{
    context.define("println", std::make_unique<Variable>(
        FunctionValue(
            std::vector<std::string>{"string"}, // params
            std::make_shared<Context>(&context),
            [](Context& context, std::vector<Value>& values) -> Value
            {
                std::cout << values[0].toString() << std::endl;
                return Value();
            }
        ),
        false
    ));
    context.define("input", std::make_unique<Variable>(
        FunctionValue(
            std::vector<std::string>{"string"}, // params
            std::make_shared<Context>(&context),
            [](Context& context, std::vector<Value>& values) -> Value
            {
                std::string value;
                std::cout << values[0].toString();
                std::getline(std::cin, value);
                return Value(value);
            }
        ),
        false
    ));
}


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
        init_stdio(context);
        
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
