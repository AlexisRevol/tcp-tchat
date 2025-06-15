find_package(Qt6 REQUIRED COMPONENTS Core)

# Créer une bibliothèque de logique commune
add_library(tchat_common_lib STATIC
    src/MessageHandler.cpp
    include/ParsedMessage.hpp
)

# Rendre son dossier d'include public pour que d'autres puissent l'utiliser
target_include_directories(tchat_common_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(tchat_common_lib PUBLIC Qt6::Core)