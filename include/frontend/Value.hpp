#pragma once

#include <variant>
#include <string>
#include "include/frontend/FunctionValue.hpp"

class Context;
class Value;


using ValueValue = std::variant<
    std::monostate,
    double,
    bool,
    std::string,
    FunctionValue
>;


enum class ValueType
{
    Null,
    Double,
    Boolean,
    String,
    Function,
};

class Value final
{
private:
    mutable ValueValue m_value;
    ValueType m_type;

public:
    Value();
    Value(double value);
    Value(bool value);
    Value(const std::string& value);
    Value(FunctionValue value);
    
    std::string toString() const;
    
    bool isNull() const;
    bool isDouble() const;
    bool isBoolean() const;
    bool isString() const;
    bool isFunction() const;
    
    
    double getDouble() const;
    bool getBoolean() const;
    std::string getString() const;
    FunctionValue& getFunction() const;
    
    Value operator+(const Value& other) const;
    Value operator-(const Value& other) const;
    Value operator*(const Value& other) const;
    Value operator/(const Value& other) const;
    
    Value operator==(const Value& other) const;
    Value operator!=(const Value& other) const;
    Value operator<(const  Value& other) const;
    Value operator>(const  Value& other) const;
    Value operator<=(const Value& other) const;
    Value operator>=(const Value& other) const;
};