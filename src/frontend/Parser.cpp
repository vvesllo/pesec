#include "include/frontend/Parser.hpp"

#include "include/frontend/ast/BlockNode.hpp"
#include "include/frontend/ast/BinaryOpNode.hpp"
#include "include/frontend/ast/ValueNode.hpp"

#include "include/frontend/ast/ReturnNode.hpp"
#include "include/frontend/ast/BreakNode.hpp"

#include "include/frontend/ast/UseNode.hpp"

#include "include/frontend/ast/WhileNode.hpp"
#include "include/frontend/ast/ForNode.hpp"
#include "include/frontend/ast/IfNode.hpp"

#include "include/frontend/ast/ArrayNode.hpp"
#include "include/frontend/ast/ArrayRangeNode.hpp"
#include "include/frontend/ast/ArrayAccessNode.hpp"
#include "include/frontend/ast/ArrayAccessAssignmentNode.hpp"

#include "include/frontend/ast/FunctionNode.hpp"
#include "include/frontend/ast/FunctionCallNode.hpp"
#include "include/frontend/ast/FunctionInstantCallNode.hpp"

#include "include/frontend/ast/VariableNode.hpp"
#include "include/frontend/ast/VariableDefinitionNode.hpp"
#include "include/frontend/ast/VariableAssignmentNode.hpp"
#include "include/frontend/ast/VariableCompoundAssignmentNode.hpp"


#include <stdexcept>
#include <format>


bool Parser::nextExists() const { return m_current != m_tokens.cend(); }

Token Parser::peek() const { return *m_current; }

Token Parser::advance() { return *m_current++; }

template<class T>
bool Parser::match(const TokenAny& token) const { return std::holds_alternative<T>(token); }

template<class T>
bool Parser::match() const { return match<T>(m_current->token); }

template<class T>
T Parser::eat()
{
    if (!match<T>())
        throw std::runtime_error(
            std::format(
                "Unexpected token {} at line {} ({} expected)", 
                    tokenToString(peek()), peek().line,
                    typeid(T).name()
            )
        );

    return std::get<T>(advance().token);
}

bool Parser::isBlockStatement(ASTNode* node) const
{
    // return  dynamic_cast<BlockNode*>(node) || 
    //         dynamic_cast<WhileNode*>(node) || 
    //         dynamic_cast<ForNode*>(node) || 
    //         dynamic_cast<FunctionNode*>(node) || 
    //         dynamic_cast<IfNode*>(node);
    return false; // temporary
}

std::vector<std::unique_ptr<ASTNode>> Parser::getArgumentList()
{
    eat<TokenType::LeftParen>();
    
    std::vector<std::unique_ptr<ASTNode>> arguments;
    
    if (nextExists() && !match<TokenType::RightParen>())
    {
        arguments.emplace_back(parseComparison());
        while (nextExists() && match<TokenType::Comma>())
        {
            eat<TokenType::Comma>();
            arguments.emplace_back(parseComparison());
        }
    }
    
    eat<TokenType::RightParen>();
    return std::move(arguments);
}

std::vector<std::unique_ptr<ASTNode>> Parser::parseStatementList(bool is_block)
{
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    while (nextExists())
    {
        if (is_block && match<TokenType::RightBracket>())
            break;

        std::unique_ptr<ASTNode> statement = parseStatement();    
        
        if (!isBlockStatement(statement.get()))
        {
            if (!nextExists())
                throw std::runtime_error("Expected ';' after statement, but EOF reached");
            
            if (!match<TokenType::Semicolon>()) 
                throw std::runtime_error(
                    std::format("Expected ';' after statement at line {}", peek().line)
                );

            eat<TokenType::Semicolon>();
        }

        statements.emplace_back(std::move(statement));
    }

    return statements;
}

std::unique_ptr<ASTNode> Parser::parseBlock()
{
    eat<TokenType::LeftBracket>();
    
    auto statement_list = parseStatementList(true);
    
    eat<TokenType::RightBracket>();
    
    return std::make_unique<BlockNode>(std::move(statement_list));
}

std::unique_ptr<ASTNode> Parser::parseStatement()
{
    if (nextExists() && match<TokenType::LeftBracket>())
        return parseBlock();
    
    return parseComparison();
}

std::unique_ptr<ASTNode> Parser::parseComparison()
{
    std::unique_ptr<ASTNode> lhs = parseExpression();
    std::unique_ptr<ASTNode> rhs = nullptr;
    
    while (nextExists() && (
        match<TokenType::EqualsEquals>() ||
        match<TokenType::NotEquals>() ||
        match<TokenType::Less>() ||
        match<TokenType::Greater>() ||
        match<TokenType::LessEquals>() ||
        match<TokenType::GreaterEquals>()
    ))
    {
        TokenAny token_type = peek().token;
        if      (match<TokenType::EqualsEquals>())  eat<TokenType::EqualsEquals>();
        else if (match<TokenType::NotEquals>())     eat<TokenType::NotEquals>();
        else if (match<TokenType::Less>())          eat<TokenType::Less>();
        else if (match<TokenType::Greater>())       eat<TokenType::Greater>();
        else if (match<TokenType::LessEquals>())    eat<TokenType::LessEquals>();
        else if (match<TokenType::GreaterEquals>()) eat<TokenType::GreaterEquals>();
        
        rhs = parseExpression();
        lhs = std::make_unique<BinaryOpNode>(
            token_type,
            std::move(lhs),
            std::move(rhs)
        );
    }
    return lhs;
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
    std::unique_ptr<ASTNode> lhs = parsePower();
    std::unique_ptr<ASTNode> rhs = nullptr;
    
    while (nextExists() && (
        match<TokenType::Asterisk>() ||
        match<TokenType::Slash>() ||
        match<TokenType::Percent>()
    ))
    {
        TokenAny token_type = peek().token;
        if (match<TokenType::Asterisk>()) 
            eat<TokenType::Asterisk>();
        else if (match<TokenType::Slash>())
            eat<TokenType::Slash>();
        else if (match<TokenType::Percent>())
            eat<TokenType::Percent>();
        
        rhs = parsePower();
        lhs = std::make_unique<BinaryOpNode>(
            token_type,
            std::move(lhs),
            std::move(rhs)
        );
    }
    return lhs;
}

std::unique_ptr<ASTNode> Parser::parsePower()
{
    std::unique_ptr<ASTNode> lhs = parseFactor();
    std::unique_ptr<ASTNode> rhs = nullptr;
    
    while (nextExists() && (
        match<TokenType::AsteriskAsterisk>()
    ))
    {
        TokenAny token_type = peek().token;
        if (match<TokenType::AsteriskAsterisk>()) 
            eat<TokenType::AsteriskAsterisk>();
        
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
        node = parseComparison();
        eat<TokenType::RightParen>();

        if (match<TokenType::DotDot>() || match<TokenType::DotDotDot>())
            node = parseArrayRange(std::move(node));
    }
    else if (nextExists() && match<TokenType::LeftBrace>())
    {
        eat<TokenType::LeftBrace>();
        node = parseArray();
        eat<TokenType::RightBrace>();
    }
    else if (nextExists() && match<TokenType::Number>())
    {
        long double value = eat<TokenType::Number>().value;
        node = std::make_unique<ValueNode>(value);

        if (match<TokenType::DotDot>() || match<TokenType::DotDotDot>())
            node = parseArrayRange(std::move(node));
    }
    else if (nextExists() && match<TokenType::Boolean>())
    {
        bool value = eat<TokenType::Boolean>().value;
        node = std::make_unique<ValueNode>(value);
    }
    else if (nextExists() && match<TokenType::String>())
    {
        std::string value = eat<TokenType::String>().value;
        node = std::make_unique<ValueNode>(value);
    }
    else if (nextExists() && match<TokenType::Null>())
    {
        eat<TokenType::Null>();
        node = std::make_unique<ValueNode>();
    }
    else if (nextExists() && match<TokenType::Identifier>())
    {
        std::string value = eat<TokenType::Identifier>().value;
        
        if (nextExists())
        {
            if      (match<TokenType::LeftParen>()) node = parseFunctionCall(value);
            else if (match<TokenType::LeftBrace>()) node = parseArrayAccess(value);
            
            else if (match<TokenType::Equals>())         node = parseVariableAssignment(value, eat<TokenType::Equals>());
            else if (match<TokenType::PlusEquals>())     node = parseVariableAssignment(value, eat<TokenType::PlusEquals>());
            else if (match<TokenType::MinusEquals>())    node = parseVariableAssignment(value, eat<TokenType::MinusEquals>());
            else if (match<TokenType::AsteriskEquals>()) node = parseVariableAssignment(value, eat<TokenType::AsteriskEquals>());
            else if (match<TokenType::SlashEquals>())    node = parseVariableAssignment(value, eat<TokenType::SlashEquals>());
            else 
            {
                node = std::make_unique<VariableNode>(value);
                if (match<TokenType::DotDot>() || match<TokenType::DotDotDot>())
                    node = parseArrayRange(std::move(node));
            }
        }
    }
    else if (nextExists() && match<TokenType::Keyword>())
    {
        TokenType::Keyword keyword = eat<TokenType::Keyword>();
        
        switch (keyword) 
        {
        case TokenType::Keyword::Use:    node = parseUse(); break;
        case TokenType::Keyword::While:  node = parseWhile(); break;
        case TokenType::Keyword::For:    node = parseFor(); break;
        case TokenType::Keyword::If:     node = parseIf(); break;
        case TokenType::Keyword::Break:  node = parseBreak(); break;
        case TokenType::Keyword::Mut:    node = parseVariableDefinition(true); break;
        case TokenType::Keyword::Const:  node = parseVariableDefinition(false); break;
        case TokenType::Keyword::Fn:     node = parseFunction(); break;
        case TokenType::Keyword::Return: node = parseReturn(); break;
        default: throw std::runtime_error(std::format(
            "Undefined control statement at line {}", 
            peek().line
        ));
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

std::unique_ptr<ASTNode> Parser::parseUse()
{
    std::string filepath = eat<TokenType::String>().value;
    
    return std::make_unique<UseNode>(filepath);
}

std::unique_ptr<ASTNode> Parser::parseFor()
{
    std::string iterator = eat<TokenType::Identifier>().value;
    std::unique_ptr<ASTNode> iterable = nullptr;
    std::unique_ptr<ASTNode> else_block = nullptr;

    
    if (nextExists() && match<TokenType::Keyword>())
    {
        TokenType::Keyword keyword = eat<TokenType::Keyword>();
        if (keyword == TokenType::Keyword::In)
            iterable = parseComparison();
    }
    
    std::unique_ptr<ASTNode> for_block = parseBlock();
    
    if (nextExists() && match<TokenType::Keyword>())
    {
        TokenType::Keyword keyword = eat<TokenType::Keyword>();
        if (keyword == TokenType::Keyword::Else)
            else_block = parseBlock();
    }

    return std::make_unique<ForNode>(
        iterator,
        std::move(iterable),
        std::move(for_block),
        std::move(else_block)
    );
}

std::unique_ptr<ASTNode> Parser::parseWhile()
{
    std::unique_ptr<ASTNode> condition = parseComparison();
    std::unique_ptr<ASTNode> while_block = parseBlock();
    std::unique_ptr<ASTNode> else_block = nullptr;

    if (nextExists() && match<TokenType::Keyword>())
    {
        TokenType::Keyword keyword = eat<TokenType::Keyword>();
        if (keyword == TokenType::Keyword::Else)
            else_block = parseBlock();

    }

    return std::make_unique<WhileNode>(
        std::move(condition),
        std::move(while_block),
        std::move(else_block)
    );
}

std::unique_ptr<ASTNode> Parser::parseIf()
{
    std::unique_ptr<ASTNode> condition = parseComparison();
    std::unique_ptr<ASTNode> then_block = parseBlock();
    std::unique_ptr<ASTNode> else_block = nullptr;

    if (nextExists() && match<TokenType::Keyword>())
    {
        TokenType::Keyword keyword = eat<TokenType::Keyword>();
        if (keyword == TokenType::Keyword::Else)
        {
            if (nextExists() && match<TokenType::Keyword>())
            {
                keyword = eat<TokenType::Keyword>();
                if (keyword == TokenType::Keyword::If)
                    else_block = parseIf();
            }
            else
                else_block = parseBlock();
        }
    }


    return std::make_unique<IfNode>(
        std::move(condition),
        std::move(then_block),
        std::move(else_block)
    );
}

std::unique_ptr<ASTNode> Parser::parseBreak()
{
    std::unique_ptr<ASTNode> node = nullptr;
    
    if (!match<TokenType::Semicolon>())
        node = parseStatement();
    
    return std::make_unique<BreakNode>(std::move(node));
}
std::unique_ptr<ASTNode> Parser::parseReturn()
{
    std::unique_ptr<ASTNode> node = nullptr;
    
    if (!match<TokenType::Semicolon>())
        node = parseStatement();
    
    return std::make_unique<ReturnNode>(std::move(node));
}

std::unique_ptr<ASTNode> Parser::parseArray()
{
    std::vector<std::unique_ptr<ASTNode>> values;

    if (nextExists() && !match<TokenType::RightBrace>())
    {
        values.emplace_back(parseComparison());
        while (nextExists() && match<TokenType::Comma>())
        {
            eat<TokenType::Comma>();
            values.emplace_back(parseComparison());
        }
    }
    
    return std::make_unique<ArrayNode>(std::move(values));
}
    
std::unique_ptr<ASTNode> Parser::parseArrayRange(std::unique_ptr<ASTNode> begin)
{
    bool contains_last = match<TokenType::DotDotDot>();
    
    if (match<TokenType::DotDot>())
        eat<TokenType::DotDot>();
    else if (match<TokenType::DotDotDot>())
        eat<TokenType::DotDotDot>();

    std::unique_ptr<ASTNode> end = parseComparison();

    return std::make_unique<ArrayRangeNode>(
        std::move(begin), std::move(end), contains_last
    );
}

std::unique_ptr<ASTNode> Parser::parseArrayAccess(const std::string& name)
{
    std::vector<std::unique_ptr<ASTNode>> indices;
    
    eat<TokenType::LeftBrace>();
    indices.emplace_back(parseComparison());
    eat<TokenType::RightBrace>();
    
    std::unique_ptr<ASTNode> node = std::make_unique<ArrayAccessNode>(name, indices.back()->clone());
    
    while (nextExists() && match<TokenType::LeftBrace>())
    {
        eat<TokenType::LeftBrace>();
        indices.emplace_back(parseComparison());
        eat<TokenType::RightBrace>();
        node = std::make_unique<ArrayAccessNode>(
            std::move(node), 
            indices.back()->clone()
        );
    }

    if (match<TokenType::Equals>())
    {
        eat<TokenType::Equals>();
        return parseArrayAccessAssignment(name, std::move(indices));
    }

    return std::move(node);
}

std::unique_ptr<ASTNode> Parser::parseArrayAccessAssignment(const std::string& name, std::vector<std::unique_ptr<ASTNode>> indices)
{
    std::unique_ptr<ASTNode> value = parseComparison();

    return std::make_unique<ArrayAccessAssignmentNode>(
        name,
        std::move(indices),
        std::move(value)
    );
}

std::unique_ptr<ASTNode> Parser::parseFunction()
{
    eat<TokenType::LeftParen>();
    
    std::vector<std::string> parameters;
    
    if (nextExists() && !match<TokenType::RightParen>())
    {
        parameters.emplace_back(eat<TokenType::Identifier>().value);
        while (nextExists() && match<TokenType::Comma>())
        {
            eat<TokenType::Comma>();
            parameters.emplace_back(eat<TokenType::Identifier>().value);
        }
    }
    
    eat<TokenType::RightParen>();

    std::unique_ptr<ASTNode> function_body = parseBlock();

    if (match<TokenType::LeftParen>())
    {
        std::vector<std::unique_ptr<ASTNode>> arguments = getArgumentList();
        return std::make_unique<FunctionInstantCallNode>(
            parameters,
            std::move(function_body),
            std::move(arguments)
        );
    }

    return std::make_unique<FunctionNode>(
        parameters,
        std::move(function_body)
    );
}

std::unique_ptr<ASTNode> Parser::parseFunctionCall(const std::string& name)
{
    std::vector<std::unique_ptr<ASTNode>> arguments = getArgumentList();
    return std::make_unique<FunctionCallNode>(
        name, 
        std::move(arguments)
    );
}

std::unique_ptr<ASTNode> Parser::parseVariableAssignment(const std::string& name, const TokenAny& token)
{
    std::unique_ptr<ASTNode> rhs = parseComparison();

    if (match<TokenType::Equals>(token))
    {
        return std::make_unique<VariableAssignmentNode>(
            name, 
            std::move(rhs)
        );
    }
    else
    {
        return std::make_unique<VariableCompoundAssignmentNode>(
            token,
            name,
            std::move(rhs)
        );
    }
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
        expr = parseComparison();
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
    auto statement_list = parseStatementList(false);
 
    return statement_list;
}