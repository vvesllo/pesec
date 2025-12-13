#include "include/frontend/LibraryLoader.hpp"


std::vector<HMODULE> LibraryLoader::m_libraries;

LibraryLoader::~LibraryLoader()
{
    for (size_t i=0; i < m_libraries.size(); i++)
        FreeLibrary(m_libraries[i]);
}

bool LibraryLoader::load(
    std::string filepath, 
    Context& context
)
{
    m_libraries.emplace_back(LoadLibraryA(filepath.c_str()));
    if (m_libraries.back() == nullptr) 
        return false;
    
    LibraryInitFunction init = (LibraryInitFunction)GetProcAddress(m_libraries.back(), "init");
    
    if (init) init(context);

    return true;
}