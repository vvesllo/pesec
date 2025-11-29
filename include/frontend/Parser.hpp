#pragma once

#include <vector>
#include <memory>
#include "include/frontend/Token.hpp"
#include "include/frontend/ast/ASTNode.hpp"


class Parser
{
private:
    const std::vector<Token> m_tokens;
    std::vector<Token>::const_iterator m_current;
    
    bool tokenExists() const;
    
    void eat(TokenType type);

public:
    Parser(const std::vector<Token>& tokens);

    std::unique_ptr<ASTNode> parse();

private:
    std::unique_ptr<ASTNode> parseExpression();    
    std::unique_ptr<ASTNode> parseTerminal();    
    std::unique_ptr<ASTNode> parseFactor();    
};