cmake_minimum_required(VERSION 3.20)
project(Systemy_Operacyjne_Projekt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(run_manager run_manager.cpp)

add_custom_target(run
        COMMAND run_manager
        DEPENDS run_manager
        COMMENT "Running the 'run_manager' executable"
)