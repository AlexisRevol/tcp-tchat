cmake_minimum_required(VERSION 3.12)
project(TCP-Tchat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTING "Build the tests" ON)

add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
add_subdirectory(client_qt)

if(BUILD_TESTING)
    # Intégration de Catch2
    include(FetchContent)
    FetchContent_Declare(
      Catch2
      GIT_REPOSITORY https://github.com/catchorg/Catch2.git
      GIT_TAG v3.6.0 
    )
    FetchContent_MakeAvailable(Catch2)

    add_subdirectory(test)
endif()