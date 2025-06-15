cmake_minimum_required(VERSION 3.10)
project(TCP-Tchat VERSION 1.0 LANGUAGES CXX)

# Standard C++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Sous-dossiers à traiter
add_subdirectory(server)
add_subdirectory(client)