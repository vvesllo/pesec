#pragma once

#include <functional>
#include <vector>
#include <string>
#include <memory>

class ASTNode;
class Context;
class Value;


using FunctionType = std::function<Value(Context&, std::vector<Value>&)>;

class FunctionValue final
{
private:
    const std::vector<std::string> m_parameters;
    std::shared_ptr<Context> m_scope;
    
    std::shared_ptr<ASTNode> m_body;
    FunctionType m_function;

public:
    FunctionValue(
        const std::vector<std::string> parameters,
        std::shared_ptr<Context> scope,
        std::shared_ptr<ASTNode> body
    );
    FunctionValue(
        const std::vector<std::string> parameters,
        std::shared_ptr<Context> scope,
        FunctionType function
    );

    Value operator()(Context& context, std::vector<Value>& arguments);
};