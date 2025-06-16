# --- Définition de la bibliothèque ---
add_library(tchat_server_lib STATIC
    src/Server.cpp
)
target_include_directories(tchat_server_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
# On lie seulement les bibliothèques du projet ici
target_link_libraries(tchat_server_lib PUBLIC tchat_common_lib)

# --- Définition de l'exécutable ---
add_executable(server
    src/main_server.cpp
)
# On lie notre bibliothèque à l'exécutable
target_link_libraries(server PUBLIC tchat_server_lib)

# --- CORRECTION ---
# On lie les bibliothèques système DIRECTEMENT à l'exécutable final
if(WIN32)
    target_link_libraries(server PRIVATE ws2_32)
endif()
if(UNIX AND NOT APPLE)
    target_link_libraries(server PRIVATE pthread)
endif()