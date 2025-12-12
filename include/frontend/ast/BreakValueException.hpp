#pragma once

#include "include/frontend/Value.hpp"
#include <stdexcept>

class BreakValueException final : public std::runtime_error
{
private:
    Value m_value;

public:
    BreakValueException(Value value);

    Value value() const;
};