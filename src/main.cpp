#include "models/country.hpp"
#include "sqlite/driver.hpp"

#include <print>
#include <sqlite3.h>

int main(int argc, char* argv[])
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    const auto conn = SQLite::Conn::setup("./build/demo.db");
    if (!conn) {
        std::println("Failed to open database: {} (code {})", conn.error().msg, conn.error().code);
    }

    if (const auto result = Models::create_table(*conn.value()); result.code != SQLITE_OK) {
        std::println("Failed to create countries table: {}", result.msg);
    }

    if (const auto result = Models::insert_country(*conn.value()); result.code != SQLITE_OK) {
        std::println("Failed to insert countries: {}", result.msg);
    }

    if (const auto result = Models::query_countries(*conn.value()); !result.has_value()) {
        std::println("Failed to query countries: {}", result.error().msg);
    } else {
        for (const auto& country : result.value()) {
            std::println("Country: {}", Models::to_string(country));
        }
    }

    if (const auto result = Models::delete_country(*conn.value()); result.code != SQLITE_OK) {
        std::println("Failed to delete countries: {}", result.msg);
    }

    return 0;
}
