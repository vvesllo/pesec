#pragma once

#include <vector>
#include <memory>
#include "include/frontend/Token.hpp"
#include "include/frontend/ast/ASTNode.hpp"


class Parser final
{
private:
    const std::vector<Token> m_tokens;
    std::vector<Token>::const_iterator m_current;

    bool nextExists() const;

    Token peek() const;
    Token advance();

    template<class T>
    bool match() const;

    template<class T>
    T eat();

    bool isBlockStatement(ASTNode* node) const;
    std::vector<std::unique_ptr<ASTNode>> getArgumentList();


    std::vector<std::unique_ptr<ASTNode>> parseStatementList(bool is_block);

    std::unique_ptr<ASTNode> parseBlock();
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseComparison();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
    

    std::unique_ptr<ASTNode> parseWhile();
    std::unique_ptr<ASTNode> parseBreak();
    std::unique_ptr<ASTNode> parseIf();
    std::unique_ptr<ASTNode> parseReturn();
    
    
    std::unique_ptr<ASTNode> parseFunction();
    std::unique_ptr<ASTNode> parseFunctionCall(const std::string& name);
    

    std::unique_ptr<ASTNode> parseVariableAssignment(const std::string& name);
    std::unique_ptr<ASTNode> parseVariableDefinition(bool is_mutable);


    
public:
    Parser(const std::vector<Token>& tokens);

    std::vector<std::unique_ptr<ASTNode>> parse();
};