# On dit à CMake d'activer les outils magiques de Qt pour ce sous-projet.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# On doit trouver les paquets Qt DANS ce sous-projet, car la bibliothèque
# en dépend directement maintenant. On a besoin de Qt6::Core pour QObject.
find_package(Qt6 REQUIRED COMPONENTS Core)

# ---- ÉTAPE 1 : Créer une bibliothèque pour la logique du client ----
add_library(tchat_client_lib STATIC
    src/Client.cpp
    "${CMAKE_CURRENT_SOURCE_DIR}/include/Client.hpp" 
)

# On indique à cette bibliothèque où trouver ses headers.
target_include_directories(tchat_client_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(tchat_client_lib PUBLIC 
    tchat_common_lib
    Qt6::Core
)

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

if(WIN32)
    target_link_libraries(client PRIVATE ws2_32)
endif()