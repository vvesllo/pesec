set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(math)

add_library(
    ${PROJECT_NAME} 
    SHARED 
    lib.cpp                                         lib.hpp
    ../../src/frontend/Context.cpp                  ../../include/frontend/Context.hpp
    ../../src/frontend/Value.cpp                    ../../include/frontend/Value.hpp
    ../../src/frontend/FunctionValue.cpp            ../../include/frontend/FunctionValue.hpp
    
    ../../src/frontend/ast/ReturnValueException.cpp ../../include/frontend/ast/ReturnValueException.hpp
)

target_include_directories(
    ${PROJECT_NAME} 
    PRIVATE 
    ${CMAKE_SOURCE_DIR}
)

target_compile_definitions(
    ${PROJECT_NAME}
    PRIVATE 
    BUILD_DLL
)