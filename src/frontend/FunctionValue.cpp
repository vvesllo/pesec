#include "include/frontend/FunctionValue.hpp"

#include "include/frontend/ast/ASTNode.hpp"
#include "include/frontend/ast/ReturnValueException.hpp"
#include "include/frontend/Context.hpp"

#include <format>
#include <stdexcept>


FunctionValue::FunctionValue(
    const std::vector<std::string> parameters,
    std::shared_ptr<Context> scope,
    std::shared_ptr<ASTNode> body
)
    : m_parameters(parameters)
    , m_scope(std::move(scope))
    , m_body(std::move(body))
    , m_function(nullptr)
{
    
}
    
FunctionValue::FunctionValue(
    const std::vector<std::string> parameters,
    std::shared_ptr<Context> scope,
    FunctionType function
)
    : m_parameters(parameters)
    , m_scope(std::move(scope))
    , m_body(nullptr)
    , m_function(std::move(function))
{

}

Value FunctionValue::operator()(Context& context, std::vector<Value>& arguments)
{
    Context local_scope(m_scope.get());

    if (arguments.size() != m_parameters.size())
        throw std::runtime_error(std::format(
            "Inappropriate number of arguments ({}, but {} given)",
            m_parameters.size(), arguments.size()
        ));

    for (size_t i=0; i < m_parameters.size(); i++)
        local_scope.define(
            m_parameters[i], 
            std::make_unique<Variable>(arguments[i], true)
        );
    
    try 
    {
        if (m_body)
            return m_body->evaluate(local_scope);
        else if (m_function)
            return m_function(local_scope, arguments);
        throw std::runtime_error("Invalid function");
    } 
    catch (const ReturnValueException& return_value) 
    {
        return return_value.value();
    }


    return Value();
}