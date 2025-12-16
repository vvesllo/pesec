#include "include/frontend/Lexer.hpp"

#include <sstream>
#include <format>


Token Lexer::createToken(const TokenAny& type) const { return Token{ type, m_line }; }

bool Lexer::nextExists() const { return m_current != m_source.cend(); }

bool Lexer::match(char character) const { return peek() == character; }

char Lexer::advance() { return *m_current++; }

char Lexer::peek() const { return *m_current; }

char Lexer::peek(size_t offset) const { return *(m_current + offset); }

void Lexer::processSkipable()
{
    while (nextExists() && std::isspace(peek())) 
    {
        if (peek() == '\n') ++m_line;
        advance();
    }
}

Token Lexer::processNumber()
{
    std::ostringstream oss;

    while (nextExists() && (
        std::isdigit(peek()) ||
        match('\'') ||
        match('.')
    )) 
    {
        if (peek() == '.' && peek(1) == '.')
            break;
        
        if (!match('\'')) 
            oss << peek();
        advance();
    }

    return createToken(TokenType::Number{ std::stold(oss.str()) });
}

Token Lexer::processString()
{
    std::ostringstream oss;

    advance();

    while (nextExists() && !match('"'))
        oss << advance();

    advance();

    return createToken(TokenType::String{ oss.str() });
}

Token Lexer::processOperator()
{
    switch (peek()) 
    {
    case '+': 
        advance();
        if (peek() == '=') 
        {
            advance();
            return createToken(TokenType::PlusEquals{});
        }
        return createToken(TokenType::Plus{});
    case '-': 
        advance();
        if (peek() == '=') 
        {
            advance();
            return createToken(TokenType::MinusEquals{});
        }
        return createToken(TokenType::Minus{});
    case '*': 
        advance();
        if (peek() == '=') 
        {
            advance();
            return createToken(TokenType::AsteriskEquals{});
        }
        else if (peek() == '*') 
        {
            advance();
            return createToken(TokenType::AsteriskAsterisk{});
        }
        return createToken(TokenType::Asterisk{});
    case '/': 
        advance();
        if (peek() == '=') 
        {
            advance();
            return createToken(TokenType::SlashEquals{});
        }
        return createToken(TokenType::Slash{});
    case '%': 
        advance();
        if (peek() == '=') 
        {
            advance();
            return createToken(TokenType::PercentEquals{});
        }
        return createToken(TokenType::Percent{});
    case '(': 
        advance();
        return createToken(TokenType::LeftParen{});
    case ')': 
        advance();
        return createToken(TokenType::RightParen{});
    case '{': 
        advance();
        return createToken(TokenType::LeftBracket{});
    case '}': 
        advance();
        return createToken(TokenType::RightBracket{});
    case '[': 
        advance();
        return createToken(TokenType::LeftBrace{});
    case ']': 
        advance();
        return createToken(TokenType::RightBrace{});
    case ';': 
        advance();
        return createToken(TokenType::Semicolon{});
    case ',': 
        advance();
        return createToken(TokenType::Comma{});
    case '.': 
        advance();
        if (peek() == '.') 
        {
            advance();
            if (peek() == '.') 
            {
                advance();
                return createToken(TokenType::DotDotDot{});
            }
            return createToken(TokenType::DotDot{});
        }
        return createToken(TokenType::Dot{});
    case '!': 
        advance();
        if (peek() == '=') 
        {
            advance();
            return createToken(TokenType::NotEquals{});
        }
        return createToken(TokenType::Not{});
    case '=': 
        advance();
        if (peek() == '=') 
        {
            advance();
            return createToken(TokenType::EqualsEquals{});
        }
        else if (peek() == '>') 
        {
            advance();
            return createToken(TokenType::ArrowRight{});
        }
        return createToken(TokenType::Equals{});
    case '<': 
        advance();
        if (peek() == '=') 
        {
            advance();
            return createToken(TokenType::LessEquals{});
        }
        return createToken(TokenType::Less{});
    case '>': 
        advance();
        if (peek() == '=') 
        {
            advance();
            return createToken(TokenType::GreaterEquals{});
        }
        return createToken(TokenType::Greater{});
    }

    throw std::runtime_error(
        std::format("Unknown character '{}'", peek())
    );
}

Token Lexer::processIdentifier()
{
    std::ostringstream oss;

    while (nextExists() && (
        std::isalnum(peek()) ||
        match('_')
    )) oss << advance();

    std::string value = oss.str();

    if      (value == "mut")  return createToken(TokenType::Keyword::Mut);
    else if (value == "const")  return createToken(TokenType::Keyword::Const);

    else if (value == "fn")     return createToken(TokenType::Keyword::Fn);
    else if (value == "return") return createToken(TokenType::Keyword::Return);

    else if (value == "use")    return createToken(TokenType::Keyword::Use);

    else if (value == "if")     return createToken(TokenType::Keyword::If);
    else if (value == "else")   return createToken(TokenType::Keyword::Else);

    else if (value == "while")  return createToken(TokenType::Keyword::While);
    else if (value == "for")    return createToken(TokenType::Keyword::For);
    else if (value == "in")     return createToken(TokenType::Keyword::In);

    else if (value == "null")   return createToken(TokenType::Null{});
    else if (value == "true")   return createToken(TokenType::Boolean{ true });
    else if (value == "false")  return createToken(TokenType::Boolean{ false });

    return createToken(TokenType::Identifier { value });
}


Lexer::Lexer(const std::string& source)
    : m_source(source)
    , m_current(m_source.cbegin())
    , m_line(1)
{

}

std::vector<Token> Lexer::process()
{
    std::vector<Token> tokens;
    
    while (nextExists()) 
    {
        if (std::isspace(peek())) 
            processSkipable();
        else if (match('"')) 
            tokens.emplace_back(processString());
        else if (std::isdigit(peek())) 
            tokens.emplace_back(processNumber());
        else if (std::isalpha(peek()) || match('_')) 
            tokens.emplace_back(processIdentifier());
        else
            tokens.emplace_back(processOperator());
    }

    return tokens;
}