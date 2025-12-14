#include "lib.hpp"

#include <stdexcept>

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
    defineFunction(context, "tostring", {"value"}, [](Context& context) -> Value {
        return Value(context.get("value").value.toString());
    });
    
    defineFunction(context, "tonumber", {"value"}, [](Context& context) -> Value {
        Value& value = context.get("value").value;
        if (value.isString())
            return Value(std::stold(value.getString()));
        
        throw std::runtime_error("Invalid value");
    });

    defineFunction(context, "length", {"value"}, [](Context& context) -> Value {
        Value& value = context.get("value").value;
        if (value.isString())
            return Value(static_cast<long double>(value.getString().size()));
        
        throw std::runtime_error("Invalid value to get length");
    });
}