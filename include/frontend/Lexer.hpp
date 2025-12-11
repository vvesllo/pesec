#pragma once

#include <string>
#include <vector>

#include "include/frontend/Token.hpp"


class Lexer final
{
private:
    const std::string m_source;
    std::string::const_iterator m_current;
    size_t m_line;

    Token createToken(const TokenAny& type) const;

    bool nextExists() const;
    bool match(char character) const;
    char advance();
    char peek() const;

    void  processSkipable();
    Token processNumber();
    Token processString();
    Token processOperator();
    Token processIdentifier();

public:
    Lexer(const std::string& source);

    std::vector<Token> process();
};