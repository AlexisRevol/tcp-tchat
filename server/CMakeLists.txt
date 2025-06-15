# --- Définition de la bibliothèque ---

add_library(tchat_server_lib STATIC
    src/Server.cpp
)
target_include_directories(tchat_server_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(tchat_server_lib PUBLIC tchat_common_lib)

# Dépendances
if(WIN32)
    target_link_libraries(tchat_server_lib PUBLIC ws2_32)
endif()
if(UNIX AND NOT APPLE)
    target_link_libraries(tchat_server_lib PUBLIC pthread)
endif()


# --- Définition des exécutables ---

add_executable(server
    src/main_server.cpp
)
target_link_libraries(server PUBLIC tchat_server_lib)