#include "include/frontend/ast/UseNode.hpp"
#include "include/frontend/LibraryLoader.hpp"

#include <stdexcept>
#include <format>


UseNode::UseNode(const std::string& filepath)
    : m_filepath(filepath)
{

}

Value UseNode::evaluate(Context& context) const
{
    if (!LibraryLoader::load(m_filepath, context))
        throw std::runtime_error(std::format(
            "Failed to load io lib '{}'",
            m_filepath
        ));

    return Value();    
}

std::unique_ptr<ASTNode> UseNode::clone() const
{
    return std::make_unique<UseNode>(m_filepath);
}
