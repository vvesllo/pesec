#include "lib.hpp"
#include <iostream>

void init(Context& context)
{
    context.define("pow", std::make_unique<Variable>(FunctionValue(
        std::vector<std::string>{"value", "power"}, // param names
        std::make_shared<Context>(&context),
        [](Context& context) -> Value
        {
            return Value(std::pow(
                context.get("value").value.getDouble(),
                context.get("power").value.getDouble()
            ));
        }
    ),
    false));
}