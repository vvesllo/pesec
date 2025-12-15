#pragma once

#include <string>
#include <variant>

namespace TokenType 
{    
    struct Identifier final { const std::string value; };
    enum class Keyword {
        Const,
        Mutab,
        
        Funct,
        Return,
        
        Use,

        While,
        For,
        In,

        Continue,

        If,
        Else,
    };
    

    struct Number final { const long double value; };
    struct String final { const std::string value; };
    struct Boolean final { const bool value; };
    struct Null final {};
    

    struct Plus final {};
    struct Minus final {};
    struct Asterisk final {};
    struct Slash final {};
    struct Percent final {};
    
    struct AsteriskAsterisk final {};

    struct Not final {};

    struct Equals final {};
    struct PlusEquals final {};
    struct MinusEquals final {};
    struct AsteriskEquals final {};
    struct SlashEquals final {};
    struct PercentEquals final {};

    struct EqualsEquals final {};
    struct NotEquals final {};
    struct Less final {};
    struct Greater final {};
    struct LessEquals final {};
    struct GreaterEquals final {};

    struct Semicolon final {};
    struct Comma final {};

    struct Dot final {};
    struct DotDot final {};
    struct DotDotDot final {};
    
    // maybe enum was better option, idc
    struct LeftParen final {}; 
    struct RightParen final {};
    
    struct LeftBracket final {};
    struct RightBracket final {};
    
    struct LeftBrace final {};
    struct RightBrace final {};
}

using TokenAny = std::variant<
    TokenType::Identifier,
    TokenType::Keyword,

    TokenType::Number,
    TokenType::String,
    TokenType::Boolean,
    TokenType::Null,

    TokenType::Plus,
    TokenType::Minus,
    TokenType::Asterisk,
    TokenType::Slash,
    TokenType::Percent,
    
    TokenType::AsteriskAsterisk,
    TokenType::Not,

    TokenType::Equals,
    TokenType::PlusEquals,
    TokenType::MinusEquals,
    TokenType::AsteriskEquals,
    TokenType::SlashEquals,
    TokenType::PercentEquals,

    TokenType::EqualsEquals,
    TokenType::NotEquals,
    TokenType::Less,
    TokenType::Greater,
    TokenType::LessEquals,
    TokenType::GreaterEquals,

    TokenType::Semicolon,
    TokenType::Comma,

    TokenType::Dot,
    TokenType::DotDot,
    TokenType::DotDotDot,

    TokenType::LeftParen,
    TokenType::RightParen,

    TokenType::LeftBracket,
    TokenType::RightBracket,

    TokenType::LeftBrace,
    TokenType::RightBrace
>;
struct Token
{
    TokenAny token;
    size_t line;
};

std::string tokenToString(const Token& token);