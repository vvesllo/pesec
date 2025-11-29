#pragma once

#include <variant>
#include <string>



enum class ValueType
{
    Integer,
    String
};

class Value
{
private:
    std::variant<long long, std::string> m_value;
    ValueType m_type;

public:
    Value();
    Value(long long value);
    Value(const std::string& value);
    
    bool isInteger() const;
    bool isString() const;

    std::string toString() const;

    long long getInteger() const;
    std::string getString() const;

    Value operator+(const Value& other) const;
    Value operator-(const Value& other) const;
    Value operator*(const Value& other) const;
    Value operator/(const Value& other) const;
};
