# Créer l'exécutable de test
add_executable(run_tests test_main.cpp)

# Le lier à Catch2 et à notre bibliothèque commune
target_link_libraries(run_tests PRIVATE
    Catch2::Catch2WithMain # La cible magique de Catch2
    tchat_common_lib     # Notre code à tester
)

# Ajouter à CTest
include(CTest)
add_test(NAME ProjectUnitTests COMMAND run_tests)