#include <print>
#include <iostream>
#include <fstream>

#include "include/frontend/Lexer.hpp"
#include "include/frontend/Parser.hpp"
#include "include/frontend/Value.hpp"


void init_stdio(Context& context)
{
    context.define("println", Variable{
        (FunctionType)[](Context& context, const std::vector<Value>& values) {
            for (size_t i=0; i < values.size(); i++)
            {
                if (i > 0) std::cout << ' ';
                std::cout << values[i].toString();
            }
            std::cout << std::endl;
            return Value();
        },
        false
    });
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
        std::string source = readfile("examples/test.pesec");

        Lexer lexer(source);
        
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
