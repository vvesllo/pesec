#include "include/frontend/Parser.hpp"

#include "include/frontend/ast/BlockNode.hpp"
#include "include/frontend/ast/BinaryOpNode.hpp"
#include "include/frontend/ast/LiteralNode.hpp"

#include "include/frontend/ast/IfNode.hpp"

#include "include/frontend/ast/FunctionCallNode.hpp"

#include "include/frontend/ast/VariableAssignmentNode.hpp"
#include "include/frontend/ast/VariableDefinitionNode.hpp"
#include "include/frontend/ast/VariableNode.hpp"


#include <stdexcept>
#include <format>
#include <print>


bool Parser::nextExists() const { return m_current != m_tokens.cend(); }

Token Parser::peek() const { return *m_current; }

Token Parser::advance() { return *m_current++; }

template<class T>
bool Parser::match() const { return std::holds_alternative<T>(m_current->token); }
template<class T>
T Parser::eat()
{
    if (!match<T>())
        throw std::runtime_error(
            std::format("Unexpected token {} at line {}", tokenToString(peek()), peek().line)
        );

    return std::get<T>(advance().token);
}

std::unique_ptr<ASTNode> Parser::parseStatement()
{
    if (nextExists() && match<TokenType::LeftBracket>())
        return parseBlock();
    
    return parseExpression();
}

std::unique_ptr<ASTNode> Parser::parseBlock()
{
    eat<TokenType::LeftBracket>();
    
    std::vector<std::unique_ptr<ASTNode>> statements;

    while (nextExists() && !match<TokenType::RightBracket>())
    {
        statements.emplace_back(parseStatement());
        eat<TokenType::Semicolon>();
    }

    eat<TokenType::RightBracket>();

    return std::make_unique<BlockNode>(std::move(statements));
}

std::unique_ptr<ASTNode> Parser::parseExpression()
{
    std::unique_ptr<ASTNode> lhs = parseTerm();
    std::unique_ptr<ASTNode> rhs = nullptr;
    
    while (nextExists() && (
        match<TokenType::Plus>() ||
        match<TokenType::Minus>()
    ))
    {
        TokenAny token_type = peek().token;
        if (match<TokenType::Plus>()) 
            eat<TokenType::Plus>();
        else if (match<TokenType::Minus>())
            eat<TokenType::Minus>();
        
        rhs = parseTerm();
        lhs = std::make_unique<BinaryOpNode>(
            token_type,
            std::move(lhs),
            std::move(rhs)
        );
    }
    return lhs;
}

std::unique_ptr<ASTNode> Parser::parseTerm()
{
    std::unique_ptr<ASTNode> lhs = parseFactor();
    std::unique_ptr<ASTNode> rhs = nullptr;
    
    while (nextExists() && (
        match<TokenType::Asterisk>() ||
        match<TokenType::Slash>()
    ))
    {
        TokenAny token_type = peek().token;
        if (match<TokenType::Asterisk>()) 
            eat<TokenType::Asterisk>();
        else if (match<TokenType::Slash>())
            eat<TokenType::Slash>();
        
        rhs = parseFactor();
        lhs = std::make_unique<BinaryOpNode>(
            token_type,
            std::move(lhs),
            std::move(rhs)
        );
    }
    return lhs;
}

std::unique_ptr<ASTNode> Parser::parseFactor()
{
    std::unique_ptr<ASTNode> node;
    if (nextExists() && match<TokenType::LeftParen>())
    {
        eat<TokenType::LeftParen>();
        node = parseExpression();
        eat<TokenType::RightParen>();
    }
    else if (nextExists() && match<TokenType::Number>())
    {
        double value = eat<TokenType::Number>().value;
        node = std::make_unique<LiteralNode>(value);
    }
    else if (nextExists() && match<TokenType::Boolean>())
    {
        bool value = eat<TokenType::Boolean>().value;
        node = std::make_unique<LiteralNode>(value);
    }
    else if (nextExists() && match<TokenType::String>())
    {
        std::string value = eat<TokenType::String>().value;
        node = std::make_unique<LiteralNode>(value);
    }
    else if (nextExists() && match<TokenType::Null>())
    {
        eat<TokenType::Null>();
        node = std::make_unique<LiteralNode>();
    }
    else if (nextExists() && match<TokenType::Identifier>())
    {
        std::string value = eat<TokenType::Identifier>().value;
        
        if (nextExists() && match<TokenType::LeftParen>())
            node = parseFunctionCall(value);
        else if (nextExists() && match<TokenType::Equals>())
            node = parseVariableAssignment(value);
        else
            node = std::make_unique<VariableNode>(value);
    }
    else if (nextExists() && match<TokenType::Keyword>())
    {
        TokenType::Keyword keyword = eat<TokenType::Keyword>();
        
        switch (keyword) 
        {
        case TokenType::Keyword::Mutab: node = parseVariableDefinition(true); break;
        case TokenType::Keyword::Const: node = parseVariableDefinition(false); break;
        case TokenType::Keyword::If:    node = parseIf(); break;
        default: throw std::runtime_error("Undefined control statement");
        }
    }
    else 
    {
        throw std::runtime_error(
            std::format("Unexpected token {} at line {}", tokenToString(peek()), peek().line)
        );
    }


    return node;
}

std::unique_ptr<ASTNode> Parser::parseIf()
{
    std::unique_ptr<ASTNode> condition = parseExpression();
    std::unique_ptr<ASTNode> then_block = parseBlock();
    std::unique_ptr<ASTNode> else_block = nullptr;

    if (nextExists() && match<TokenType::Keyword>())
    {
        TokenType::Keyword keyword = eat<TokenType::Keyword>();
        if (keyword == TokenType::Keyword::Else)
            else_block = parseBlock();
    }


    return std::make_unique<IfNode>(
        std::move(condition),
        std::move(then_block),
        std::move(else_block)
    );
}

std::unique_ptr<ASTNode> Parser::parseFunctionCall(const std::string& name)
{
    eat<TokenType::LeftParen>();
    
    std::vector<std::unique_ptr<ASTNode>> arguments;
    
    if (nextExists() && !match<TokenType::RightParen>())
    {
        arguments.emplace_back(parseExpression());
        while (nextExists() && match<TokenType::Comma>())
        {
            eat<TokenType::Comma>();
            arguments.emplace_back(parseExpression());
        }
    }
    
    eat<TokenType::RightParen>();
    
    return std::make_unique<FunctionCallNode>(
        name, 
        std::move(arguments)
    );
}

std::unique_ptr<ASTNode> Parser::parseVariableAssignment(const std::string& name)
{
    eat<TokenType::Equals>();
    
    std::unique_ptr<ASTNode> value = parseExpression();
    
    return std::make_unique<VariableAssignmentNode>(
        name, 
        std::move(value)
    );
}

std::unique_ptr<ASTNode> Parser::parseVariableDefinition(bool is_mutable)
{
    const std::string name = eat<TokenType::Identifier>().value;
    
    std::unique_ptr<ASTNode> expr = nullptr;

    if (nextExists() && (
        match<TokenType::Equals>() ||
        is_mutable
    ))
    {
        eat<TokenType::Equals>();
        expr = parseExpression();
    }

    return std::make_unique<VariableDefinitionNode>(
        name,
        std::move(expr),
        is_mutable
    );
}

Parser::Parser(const std::vector<Token>& tokens)
    : m_tokens(tokens)
    , m_current(m_tokens.cbegin())
{

}

std::vector<std::unique_ptr<ASTNode>> Parser::parse()
{    
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    while (nextExists()) 
    {
        std::unique_ptr<ASTNode> statement = parseStatement();
        
        bool is_block_statement = 
            dynamic_cast<BlockNode*>(statement.get()) || 
            dynamic_cast<IfNode*>(statement.get());

        statements.emplace_back(std::move(statement));

        if (!is_block_statement)
        {
            if (!nextExists())
                throw std::runtime_error("Expected ';' after statement, but EOF reached");

            if (!match<TokenType::Semicolon>()) 
                throw std::runtime_error(
                    std::format("Expected ';' after statement at line {}", peek().line)
                );
            
            eat<TokenType::Semicolon>();
        }
    }
 
    return statements;
}
