# tcp-tchat
Application de tchat utilisant le protocole TCP 

## Prerequisites

- A C++17 compliant compiler (MSVC, GCC, Clang)
- CMake (version 3.12 or higher)
- Qt6 (version 6.2 or higher). During installation, make sure to select the components corresponding to your compiler (e.g., MSVC 2019/2022 on Windows).

## Build Steps

1.  Clone the repository:
    `git clone https://github.com/AlexisRevol/tcp-tchat.git`
2.  Create a build directory:
    `cd tcp-tchat`
    `mkdir build && cd build`
3.  Configure with CMake. You will need to tell CMake where to find your Qt installation.
    `cmake -D CMAKE_PREFIX_PATH="C:/path/to/Qt/6.x.x/msvc20xx_64" ..`
4.  Build the project:
    `cmake --build .`