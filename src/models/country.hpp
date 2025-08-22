#pragma once

#include <sqlite/driver.hpp>

#include <print>

namespace Models {

struct Country {
    int id {};
    std::string name {};
    int population {};
    int area {};
};

using std::to_string;
inline auto to_string(const Models::Country& country) -> std::string
{
    return std::format(
        "{{ id: {}, name: {}, population: {}, area: {} }}",
        country.id, country.name, country.population, country.area);
}

inline auto create_table(const SQLite::Conn& con) -> std::expected<SQLite::Result, std::string_view>
{
    constexpr std::string_view query {
        R"( CREATE TABLE IF NOT EXISTS countries(
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE,
            population INTEGER NOT NULL CHECK(population >= 0),
            area INTEGER NOT NULL CHECK(area >= 0)
        );)"
    };

    std::future<SQLite::Result> res = con.execute(query);
    return res.get();
}

inline auto insert_country(const SQLite::Conn& con) -> std::expected<SQLite::Result, std::string_view>
{
    static constexpr Models::Country country {
        .name = "United States",
        .population = 331002651,
        .area = 9833517
    };
    std::println("Country: {}", to_string(country));

    constexpr std::string_view query { R"(INSERT INTO countries (name, population, area) VALUES (?, ?, ?);)" };

    const auto bind_callback = [](sqlite3_stmt* stmt) -> SQLite::Result::Code {
        if (const auto code = SQLite::bind_stmt(*stmt, 1, country.name); SQLITE_OK != code) {
            return code;
        }
        if (const auto code = SQLite::bind_stmt(*stmt, 2, country.population); SQLITE_OK != code) {
            return code;
        }
        if (const auto code = SQLite::bind_stmt(*stmt, 3, country.area); SQLITE_OK != code) {
            return code;
        }
        return SQLite::step(*stmt);
    };

    std::future<SQLite::Result> res = con.execute_stmt(query, bind_callback);
    return res.get();
}

} // Models
