# minimum CMake version required
cmake_minimum_required(VERSION 3.15)
# Project name, version and description
project(discord-bot VERSION 1.0 DESCRIPTION "C++ bot")
 
# Add DPP as dependency
add_subdirectory(libs/DPP)
# You can also add any other libs you want to use
 
# Create an executable
add_executable(${PROJECT_NAME}
    src/bot.cpp
)
 
# Linking libraries
target_link_libraries(${PROJECT_NAME}
    dpp
    # Add any other libs you want to use here
)
 
# Specify includes
target_include_directories(${PROJECT_NAME} PRIVATE
    libs/DPP/include
    # Remember to add the include directories of any other libraries too
)
 
# Set C++ version
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
