#pragma once

#include <variant>
#include <string>


enum class TokenType
{
    Identifier,
    Keyword,
    
    String,
    Integer,
    Float,

    Semicolon,

    LeftParen,
    RightParen,

    Eof
};

struct Token
{
    TokenType type;
    std::variant<
        long long,
        double,
        std::string
    > value;
};