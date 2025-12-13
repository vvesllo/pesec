#pragma once

#include <variant>
#include <string>
#include "include/frontend/FunctionValue.hpp"

class Context;
class Value;


using ValueValue = std::variant<
    std::monostate,
    long double,
    bool,
    std::string,
    FunctionValue
>;

class Value final
{
private:
    mutable ValueValue m_value;

public:
    Value();
    Value(long double value);
    Value(bool value);
    Value(const std::string& value);
    Value(FunctionValue value);
    
    std::string toString() const;
    
    bool isNull() const;
    bool isDouble() const;
    bool isBoolean() const;
    bool isString() const;
    bool isFunction() const;
    
    long double getDouble() const;
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