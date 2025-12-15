#include "include/frontend/Value.hpp"
#include "include/frontend/FunctionValue.hpp"

#include <stdexcept>
#include <sstream>


Value::Value() 
    : m_value(std::monostate{}) 
{}

Value::Value(long double value) 
    : m_value(value) 
{}

Value::Value(bool value) 
    : m_value(value) 
{}

Value::Value(const std::string& value) 
    : m_value(value) 
{}

Value::Value(FunctionValue value) 
    : m_value(std::move(value)) 
{}
    
Value::Value(ArrayValue value)
    : m_value(std::move(value)) 
{}

std::string Value::toString() const
{
    if (isNull())
        return "null";
    else if (isBoolean())
        return getBoolean() ? "true" : "false";
    else if (isString())
        return getString();
    else if (isDouble())
    {
        std::ostringstream oss;
        oss.precision(std::numeric_limits<double>::max_digits10);
        oss << getDouble(); 
        return oss.str();
    }
    else if (isArray())
    {
        std::ostringstream oss;
        oss << '[';

        const ArrayValue values = getArray();

        for (size_t i=0; i < values.size(); i++)
        {
            if (i > 0) oss << ", ";
            oss << values[i].toString();
        }

        oss << ']';
        return oss.str();
    }

    return "unstr";
}

bool Value::isNull() const { return std::holds_alternative<std::monostate>(m_value); }

bool Value::isDouble() const { return std::holds_alternative<long double>(m_value); }

bool Value::isBoolean() const { return std::holds_alternative<bool>(m_value); }

bool Value::isString() const { return std::holds_alternative<std::string>(m_value); }

bool Value::isFunction() const { return std::holds_alternative<FunctionValue>(m_value); }

bool Value::isArray() const { return std::holds_alternative<ArrayValue>(m_value); }

long double Value::getDouble() const
{
    if (isDouble()) return std::get<long double>(m_value);

    throw std::runtime_error("Value is not double");
}

bool Value::getBoolean() const
{    
    if (isBoolean()) return std::get<bool>(m_value);

    throw std::runtime_error("Value is not boolean");
}

std::string Value::getString() const
{
    if (isString()) return std::get<std::string>(m_value);

    throw std::runtime_error("Value is not string");
}

FunctionValue& Value::getFunction() const
{
    if (isFunction()) return std::get<FunctionValue>(m_value);

    throw std::runtime_error("Value is not function");
}
 
ArrayValue& Value::getArray() const
{
    if (isArray()) return std::get<ArrayValue>(m_value);

    throw std::runtime_error("Value is not array");
}
 
Value Value::power(const Value& other) const
{
    if (isDouble() && other.isDouble())
        return Value(std::powl(getDouble(), other.getDouble()));
    
    throw std::runtime_error("Can't calculate the power of value");
}

Value Value::operator+(const Value& other) const
{
    if (isDouble() && other.isDouble())
        return Value(getDouble() + other.getDouble());
    else if (isString() && other.isString())
        return Value(getString() + other.getString());
    
    throw std::runtime_error("Values can't be added");
}

Value Value::operator-(const Value& other) const
{
    if (isDouble() && other.isDouble())
        return Value(getDouble() - other.getDouble());
   
    throw std::runtime_error("Values can't be substracted");
}

Value Value::operator*(const Value& other) const
{
    if (isDouble() && other.isDouble())
        return Value(getDouble() * other.getDouble());
    
    throw std::runtime_error("Values can't be multiplied");
}

Value Value::operator/(const Value& other) const
{
    if (other.isDouble() && other.getDouble() == 0)
        throw std::runtime_error("Division by zero");

    if (isDouble() && other.isDouble())
        return Value(getDouble() / other.getDouble());
   
    throw std::runtime_error("Values can't be divided");
}

Value Value::operator%(const Value& other) const
{
    if (other.isDouble() && other.getDouble() == 0)
        throw std::runtime_error("Division by zero");

    if (isDouble() && other.isDouble())
        return Value(std::fmodl(getDouble(), other.getDouble()));
   
    throw std::runtime_error("Can't calcualte reminder of division");
}

Value Value::operator==(const Value& other) const
{
    if (isDouble() && other.isDouble())   return getDouble() == other.getDouble();
    if (isString() && other.isString())   return getString() == other.getString();
    if (isBoolean() && other.isBoolean()) return getBoolean() == other.getBoolean();

    throw std::runtime_error("Values can't be compared");
}

Value Value::operator!=(const Value& other) const
{
    if (isDouble() && other.isDouble())   return getDouble() != other.getDouble();
    if (isString() && other.isString())   return getString() != other.getString();
    if (isBoolean() && other.isBoolean()) return getBoolean() != other.getBoolean();

    throw std::runtime_error("Values can't be compared");
}

Value Value::operator<(const Value& other) const
{
    if (isDouble() && other.isDouble()) return getDouble() < other.getDouble();

    throw std::runtime_error("Values can't be compared");
}

Value Value::operator>(const Value& other) const
{
    if (isDouble() && other.isDouble()) return getDouble() > other.getDouble();

    throw std::runtime_error("Values can't be compared");
}

Value Value::operator<=(const Value& other) const
{
    if (isDouble() && other.isDouble()) return getDouble() <= other.getDouble();

    throw std::runtime_error("Values can't be compared");
}

Value Value::operator>=(const Value& other) const
{
    if (isDouble() && other.isDouble()) return getDouble() >= other.getDouble();

    throw std::runtime_error("Values can't be compared");
}
