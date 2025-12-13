#include "include/frontend/PesecStd.hpp"

#include "include/frontend/FunctionValue.hpp"

#include <iostream>



//std::unordered_map<std::string, Value>&

void PesecStd::init_io(Context& context)
{
    context.define("print", std::make_unique<Variable>(FunctionValue(
        std::vector<std::string>{"value"}, // params
        std::make_shared<Context>(&context),
        [](Context& context) -> Value
        {
            std::cout << context.get("value").value.toString();
            return Value();
        }
    ),
    false));
    context.define("println", std::make_unique<Variable>(FunctionValue(
        std::vector<std::string>{"value"}, // params
        std::make_shared<Context>(&context),
        [](Context& context) -> Value
        {
            std::cout << context.get("value").value.toString();
            return Value();
        }
    ),
    false));
    context.define("input", std::make_unique<Variable>(FunctionValue(
        std::vector<std::string>{"value"}, // params
        std::make_shared<Context>(&context),
        [](Context& context) -> Value
        {
            std::string value;
            std::cout << context.get("value").value.toString();
            std::getline(std::cin, value);
            return Value(value);
        }
    ),
    false));
}

void PesecStd::init_types(Context& context)
{
    context.define("tonum", std::make_unique<Variable>(FunctionValue(
        std::vector<std::string>{"value"}, // params
        std::make_shared<Context>(&context),
        [](Context& context) -> Value
        {
            Value value = context.get("value").value;
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
        [](Context& context) -> Value
        {
            return Value(context.get("value").value.toString());
        }
    ),
    false));
}

void PesecStd::init_math(Context& context)
{

}
