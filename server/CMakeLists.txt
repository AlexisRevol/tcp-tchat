# --- Définition de la bibliothèque ---

add_library(tchat_server_lib STATIC
    src/Server.cpp
)
target_include_directories(tchat_server_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Dépendances
if(WIN32)
    target_link_libraries(tchat_server_lib PRIVATE ws2_32)
endif()
if(UNIX AND NOT APPLE)
    target_link_libraries(tchat_server_lib PRIVATE pthread)
endif()


# --- Définition des exécutables ---

add_executable(server
    src/main_server.cpp
)
target_link_libraries(server PRIVATE tchat_server_lib)