#pragma once

#include <vector>

#include "include/frontend/Token.hpp"

class Lexer
{
private:
    std::vector<Token> m_tokens;
    const std::string m_source;
    std::string::const_iterator m_current;
    
    void tokenizeNumber();
    void tokenizeString();
    void tokenizeIdentifier();
    void tokenizeOperator();

    void skipWhitespace();

public:
    Lexer(const std::string& source);

    std::vector<Token> tokenize();
};