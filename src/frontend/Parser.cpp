#include "include/frontend/Parser.hpp"

#include "include/frontend/ast/IntegerNode.hpp"
#include "include/frontend/ast/StringNode.hpp"
#include "include/frontend/ast/BinaryOpNode.hpp"


#include <stdexcept>
#include <print>

bool Parser::tokenExists() const
{
    return m_current != m_tokens.cend();
}

void Parser::eat(TokenType type)
{
    if (tokenExists() && m_current->type == type)
    {
        m_current++;
        return;
    }    

    throw std::runtime_error("Unexpected token type: " + std::to_string((int)m_current->type));
}

Parser::Parser(const std::vector<Token>& tokens)
    : m_tokens(tokens)
    , m_current(m_tokens.cbegin())
{

}


std::unique_ptr<ASTNode> Parser::parse()
{    
    if (!tokenExists()) throw std::runtime_error("Empty input");

    return parseExpression();
}


std::unique_ptr<ASTNode> Parser::parseExpression()
{
    std::unique_ptr<ASTNode> left = parseTerminal(); 
    
    while (tokenExists() && (
        m_current->type == TokenType::Plus ||
        m_current->type == TokenType::Minus
    ))
    {
        const std::string op = std::get<std::string>(m_current->value);
        eat(m_current->type);
        std::unique_ptr<ASTNode> right = parseTerminal(); 
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }

    return left;
}   

std::unique_ptr<ASTNode> Parser::parseTerminal()
{    
    std::unique_ptr<ASTNode> left = parseFactor(); 
    
    while (tokenExists() && (
        m_current->type == TokenType::Star ||
        m_current->type == TokenType::Slash
    ))
    {
        const std::string op = std::get<std::string>(m_current->value);
        eat(m_current->type);
        std::unique_ptr<ASTNode> right = parseFactor(); 
        left = std::make_unique<BinaryOpNode>(op, std::move(left), std::move(right));
    }

    return left;
}   

std::unique_ptr<ASTNode> Parser::parseFactor()
{    
    if (!tokenExists())
        throw std::runtime_error("Unexpected end of input");
    
    if (m_current->type == TokenType::Integer)
    {
        const long long value = std::get<long long>(m_current->value);
        eat(TokenType::Integer);
        return std::make_unique<IntegerNode>(value);
    }
    else if (m_current->type == TokenType::String)
    {
        const std::string value = std::get<std::string>(m_current->value);
        eat(TokenType::String);
        return std::make_unique<StringNode>(value);
    }
    else if (m_current->type == TokenType::LeftParen)
    {
        eat(TokenType::LeftParen);
        std::unique_ptr<ASTNode> expr = parseExpression();
        eat(TokenType::RightParen);
        
        return expr;
    }

    throw std::runtime_error("Unknow factor type");
}   
