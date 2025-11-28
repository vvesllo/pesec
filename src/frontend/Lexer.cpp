#include "include/frontend/Lexer.hpp"

#include <sstream>
#include <string>
#include <print>
#include <stdexcept>

    
void Lexer::tokenizeNumber()
{
    std::stringstream buffer;

    uint8_t point_count = 0;
    while (std::isdigit(*m_current) || *m_current == '.')
    {
        buffer << *m_current;
        m_current++;
        if (*m_current == '.')
        {
            if (++point_count > 1) 
                throw std::runtime_error("Invalid float number");
        }
    }
    
    if (point_count == 1)
        m_tokens.emplace_back(Token{
            TokenType::Float,
            std::stod(buffer.str())
        });
    else
        m_tokens.emplace_back(Token{
            TokenType::Integer,
            std::stoll(buffer.str())
        });
}

void Lexer::tokenizeString()
{
    std::stringstream buffer;

    m_current++;
    while (*m_current != '"')
    {
        buffer << *m_current;
        m_current++;
    }
    m_current++;
    
    m_tokens.emplace_back(Token{
        TokenType::String,
        buffer.str()
    });
}

void Lexer::tokenizeIdentifier()
{
    std::stringstream buffer;

    while (std::isalnum(*m_current) || *m_current == '_')
    {
        buffer << *m_current;
        m_current++;
    }
    
    m_tokens.emplace_back(Token{
        TokenType::String,
        buffer.str()
    });
}
    
void Lexer::tokenizeOperator()
{
    const std::string op_string(1, *m_current);

    switch (*m_current)
    {
    case ';':  m_tokens.emplace_back(Token{TokenType::Semicolon, op_string});   m_current++; break;
    
    case '(':  m_tokens.emplace_back(Token{TokenType::LeftParen, op_string});   m_current++; break;
    case ')':  m_tokens.emplace_back(Token{TokenType::RightParen, op_string});  m_current++; break;
    
    case '\0': m_tokens.emplace_back(Token{TokenType::Eof, op_string});         m_current++; break;
    default:
        throw std::runtime_error("Unknown operator: " + op_string);
    }
    
}

void Lexer::skipWhitespace()
{
    while (std::isspace(*m_current))
        m_current++;
}

Lexer::Lexer(const std::string& source)
    : m_source(source)
    , m_current(m_source.cbegin())
{

}

std::vector<Token> Lexer::tokenize()
{
    while (m_current != m_source.cend()) 
    {
        if      (std::isspace(*m_current))  skipWhitespace();
        else if (std::isdigit(*m_current))  tokenizeNumber();
        else if (std::isalpha(*m_current))  tokenizeIdentifier();
        else if (*m_current == '"')         tokenizeString();
        else                                tokenizeOperator();
    }

    return m_tokens;
}
