// tests/test_main.cpp
#include <catch2/catch_test_macros.hpp>

// On inclut les deux headers de notre bibliothèque commune
#include "MessageHandler.hpp"
#include "ParsedMessage.hpp"

TEST_CASE("Protocol messages are handled correctly", "[MessageHandler]") {
    MessageHandler handler;

    // --- SECTION 1 : Tester l'Encodage (ce que le serveur fait) ---
    
    SECTION("Encoding chat messages") {
        REQUIRE(handler.encode_chat_message("Bob", "Salut !") == "MSG|Bob|Salut !");
        // Test d'un cas limite : message vide
        REQUIRE(handler.encode_chat_message("Alice", "") == "MSG|Alice|");
    }

    SECTION("Encoding join messages") {
        REQUIRE(handler.encode_join_message("Charles") == "JOIN|Charles");
    }

    SECTION("Encoding leave messages") {
        REQUIRE(handler.encode_leave_message("Eve") == "PART|Eve");
    }


    // --- SECTION 2 : Tester le Décodage (ce que le client fait) ---

    SECTION("Decoding a valid chat message") {
        std::string raw_msg = "MSG|Alice|Hello World";
        ParsedMessage result = handler.decode(raw_msg);

        // On vérifie chaque partie du message décodé
        REQUIRE(result.command == Command::MSG);
        REQUIRE(result.params.size() == 2);
        REQUIRE(result.params[0] == "Alice");
        REQUIRE(result.params[1] == "Hello World");
    }

    SECTION("Decoding a valid join message") {
        ParsedMessage result = handler.decode("JOIN|David");

        REQUIRE(result.command == Command::JOIN);
        REQUIRE(result.params.size() == 1);
        REQUIRE(result.params[0] == "David");
    }

    SECTION("Decoding a valid part message") {
        ParsedMessage result = handler.decode("PART|Frank");

        REQUIRE(result.command == Command::PART);
        REQUIRE(result.params.size() == 1);
        REQUIRE(result.params[0] == "Frank");
    }


    // --- SECTION 3 : Tester les Cas Limites et Erreurs de Décodage ---

    SECTION("Decoding a malformed message") {
        // Un message qui ne respecte pas le protocole
        ParsedMessage result = handler.decode("GARBAGE DATA");
        REQUIRE(result.command == Command::UNKNOWN);
    }

    SECTION("Decoding an empty message") {
        ParsedMessage result = handler.decode("");
        REQUIRE(result.command == Command::UNKNOWN);
    }
    
    SECTION("Decoding a message with missing parts") {
        ParsedMessage result = handler.decode("MSG|Alice"); // Il manque le contenu du message
        
        REQUIRE(result.command == Command::MSG);
        REQUIRE(result.params.size() == 1); // Le décodeur a quand même extrait le pseudo
        REQUIRE(result.params[0] == "Alice");
        // Ce sera au client de gérer le fait que le message est incomplet
    }
}