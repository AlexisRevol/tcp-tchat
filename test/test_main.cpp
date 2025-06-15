#include <catch2/catch_test_macros.hpp>

#include "MessageHandler.hpp"

TEST_CASE("Message formatting works correctly", "[MessageHandler]") {
    MessageHandler handler;

    SECTION("Join messages") {
        REQUIRE(handler.format_join_message("Alice") == "Alice a rejoint le tchat.");
    }

    SECTION("Chat messages") {
        REQUIRE(handler.format_chat_message("Bob", "Salut") == "Bob: Salut");
        REQUIRE(handler.format_chat_message("Test", "") == "Test: ");
    }

    SECTION("Leave messages") {
        REQUIRE(handler.format_leave_message("Eve") == "Eve a quitte le tchat.");
    }
}