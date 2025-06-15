# ---- ÉTAPE 1 : Créer une bibliothèque pour la logique du client ----
add_library(tchat_client_lib STATIC
    src/Client.cpp
)

# On indique à cette bibliothèque où trouver ses headers.
target_include_directories(tchat_client_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(tchat_client_lib PUBLIC tchat_common_lib)

# On indique à la bibliothèque ses dépendances.
if(WIN32)
    target_link_libraries(tchat_client_lib PUBLIC ws2_32)
endif()
if(UNIX AND NOT APPLE)
    target_link_libraries(tchat_client_lib PUBLIC pthread)
endif()


# ---- ÉTAPE 2 : Créer l'exécutable qui UTILISE la bibliothèque ----
add_executable(client
    src/main_client.cpp
)

# On lie notre exécutable "client" à notre bibliothèque "tchat_client_lib".
target_link_libraries(client PUBLIC tchat_client_lib)