#pragma once

#include "include/frontend/Context.hpp"

#include <windows.h>

using LibraryInitFunction = void(*)(Context&);

class LibraryLoader
{
private:
    static std::vector<HMODULE> m_libraries;

public:
    ~LibraryLoader();

    static bool load(
        std::string filepath, 
        Context& context
    );
};