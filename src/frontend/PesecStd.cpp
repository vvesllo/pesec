#include "include/frontend/PesecStd.hpp"

#include "include/frontend/FunctionValue.hpp"

#include <iostream>

void PesecStd::init_io(Context& context)
{
    context.define("print", std::make_unique<Variable>(FunctionValue(
        std::vector<std::string>{"value"}, // params
        std::make_shared<Context>(&context),
        [](Context& context, std::vector<Value>& values) -> Value
        {
            std::cout << values[0].toString();
            return Value();
        }
    ),
    false));
    context.define("println", std::make_unique<Variable>(FunctionValue(
        std::vector<std::string>{"value"}, // params
        std::make_shared<Context>(&context),
        [](Context& context, std::vector<Value>& values) -> Value
        {
            std::cout << values[0].toString() << std::endl;
            return Value();
        }
    ),
    false));
    context.define("input", std::make_unique<Variable>(FunctionValue(
        std::vector<std::string>{"value"}, // params
        std::make_shared<Context>(&context),
        [](Context& context, std::vector<Value>& values) -> Value
        {
            std::string value;
            std::cout << values[0].toString();
            std::getline(std::cin, value);
            return Value(value);
        }
    ),
    false));
}

void PesecStd::init_types(Context& context)
{
    context.define("tonum", std::make_unique<Variable>(FunctionValue(
        std::vector<std::string>{"string"}, // params
        std::make_shared<Context>(&context),
        [](Context& context, std::vector<Value>& values) -> Value
        {
            Value value = values[0];
            if (value.isString())
            {
                return Value(std::stold(value.getString()));
            }
            
            throw std::runtime_error("Variable is not a number string");
        }
    ),
    false));
    context.define("tostr", std::make_unique<Variable>(FunctionValue(
        std::vector<std::string>{"value"}, // params
        std::make_shared<Context>(&context),
        [](Context& context, std::vector<Value>& values) -> Value
        {
            return Value(values[0].toString());
        }
    ),
    false));
}

void PesecStd::init_math(Context& context)
{

}
