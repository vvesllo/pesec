#pragma once

#include "include/frontend/Value.hpp"
#include <stdexcept>

class ReturnValueException final : public std::runtime_error
{
private:
    Value m_value;

public:
    ReturnValueException(Value value);

    Value value() const; 
};