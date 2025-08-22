#include "models/country.hpp"
#include "sqlite/driver.hpp"

#include <print>
#include <sqlite3.h>

int main(int argc, char* argv[])
{
    const auto conn = SQLite::Conn::setup("./build/my.db");
    if (!conn) {
        std::println("Failed to open database: {} (code {})", conn.error().msg, conn.error().code);
    }

    if (const auto result = Models::create_table(*conn.value()); !result) {
        std::println("Failed to create countries table: {}", result.error());
    }

    if (const auto result = Models::insert_country(*conn.value()); !result) {
        std::println("Failed to insert countries: {}", result.error());
    }

    return 0;
}
