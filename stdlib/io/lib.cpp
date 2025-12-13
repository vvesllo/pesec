#include "lib.hpp"
#include <iostream>

void init(Context& context)
{
    context.define("print", std::make_unique<Variable>(FunctionValue(
        std::vector<std::string>{"value"}, // param names
        std::make_shared<Context>(&context),
        [](Context& context) -> Value
        {
            std::cout << context.get("value").value.toString();
            return Value();
        }
    ),
    false));
    context.define("println", std::make_unique<Variable>(FunctionValue(
        std::vector<std::string>{"value"}, // param names
        std::make_shared<Context>(&context),
        [](Context& context) -> Value
        {
            std::cout << context.get("value").value.toString();
            return Value();
        }
    ),
    false));
    context.define("input", std::make_unique<Variable>(FunctionValue(
        std::vector<std::string>{"value"}, // param names
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