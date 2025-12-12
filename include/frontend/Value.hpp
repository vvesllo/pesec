#pragma once

#include <functional>
#include <variant>
#include <string>

class Context;
class Value;

using FunctionType = std::function<Value(Context&, const std::vector<Value>&)>;

using ValueValue = std::variant<
    std::monostate,
    double,
    bool,
    std::string,
    FunctionType
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
    ValueValue m_value;
    ValueType m_type;

public:
    Value();
    Value(double value);
    Value(bool value);
    Value(const std::string& value);
    Value(const FunctionType& value);
    
    std::string toString() const;
    
    bool isNull() const;
    bool isDouble() const;
    bool isBoolean() const;
    bool isString() const;
    bool isFunction() const;
    
    
    double getDouble() const;
    bool getBoolean() const;
    std::string getString() const;
    FunctionType getFunction() const;

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