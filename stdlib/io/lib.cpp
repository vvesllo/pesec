#include "lib.hpp"

#include <iostream>


void defineFunction(
    Context& context, 
    std::string name,
    std::vector<std::string> params,
    std::function<Value(Context&)> func
)
{
    context.define(name, std::make_unique<Variable>(FunctionValue(
        params,
        std::make_shared<Context>(&context),
        func
    ),
    false));
}

void init(Context& context)
{
    defineFunction(context, "print", {"value"}, [](Context& context) -> Value {
        std::cout << context.get("value").value.toString();
        return Value();
    });

    defineFunction(context, "println", {"value"}, [](Context& context) -> Value {
        std::cout << context.get("value").value.toString() << std::endl;
        return Value();
    });
    
    defineFunction(context, "input", {"value"}, [](Context& context) -> Value {
        std::string value;
        std::cout << context.get("value").value.toString();
        std::getline(std::cin, value);
        return Value(value);
    });
}