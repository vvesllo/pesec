#include "include/frontend/Token.hpp"

#include <format>

std::string tokenToString(const Token& token)
{
    if      (const auto tok = std::get_if<TokenType::Identifier>(&token.token)) return std::format("Identifier(value={})", tok->value);
    else if (const auto tok = std::get_if<TokenType::Keyword>(&token.token))    return "Keyword";

    else if (const auto tok = std::get_if<TokenType::String>(&token.token))     return std::format("String(value={})", tok->value);
    else if (const auto tok = std::get_if<TokenType::Number>(&token.token))     return std::format("Number(value={})", tok->value);
    else if (const auto tok = std::get_if<TokenType::Boolean>(&token.token))    return std::format("Boolean(value={})", tok->value);
    else if (const auto tok = std::get_if<TokenType::Null>(&token.token))       return std::format("Null");
    
    else if (const auto tok = std::get_if<TokenType::Plus>(&token.token))       return "Plus";
    else if (const auto tok = std::get_if<TokenType::Minus>(&token.token))      return "Minus";
    else if (const auto tok = std::get_if<TokenType::Asterisk>(&token.token))   return "Asterisk";
    else if (const auto tok = std::get_if<TokenType::Slash>(&token.token))      return "Slash";

    else if (const auto tok = std::get_if<TokenType::Equals>(&token.token))     return "Equals";

    else if (const auto tok = std::get_if<TokenType::Semicolon>(&token.token))  return "Semicolon";
    else if (const auto tok = std::get_if<TokenType::Comma>(&token.token))      return "Comma";

    else if (const auto tok = std::get_if<TokenType::Dot>(&token.token))        return "Dot";
    else if (const auto tok = std::get_if<TokenType::DotDot>(&token.token))     return "DotDot";
    else if (const auto tok = std::get_if<TokenType::DotDotDot>(&token.token))  return "DotDotDot";

    else if (const auto tok = std::get_if<TokenType::LeftParen>(&token.token))  return "LeftParen";
    else if (const auto tok = std::get_if<TokenType::RightParen>(&token.token)) return "RightParen";

    else if (const auto tok = std::get_if<TokenType::LeftBracket>(&token.token))  return "LeftBracket";
    else if (const auto tok = std::get_if<TokenType::RightBracket>(&token.token)) return "RightBracket";

    else if (const auto tok = std::get_if<TokenType::LeftBrace>(&token.token))  return "LeftBrace";
    else if (const auto tok = std::get_if<TokenType::RightBrace>(&token.token)) return "RightBrace";

    throw std::runtime_error(
        std::format("Unknown token at line {}", token.line)
    );
}