#pragma once

#include <sqlite/driver.hpp>

#include <print>
#include <vector>

namespace Models {

struct Country {
    int id {};
    std::string name {};
    int population {};
    int area {};
};
using Countries = std::vector<Country>;

using std::to_string;
inline auto to_string(const Models::Country& country) -> std::string
{
    return std::format(
        "{{ id: {}, name: {}, population: {}, area: {} }}",
        country.id, country.name, country.population, country.area);
}

inline auto create_table(const SQLite::Conn& con) -> SQLite::Result
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

inline auto insert_country(const SQLite::Conn& con) -> SQLite::Result
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

inline auto query_countries(const SQLite::Conn& con) -> std::expected<Countries, SQLite::Result>
{
    Countries countries_out {};
    uint8_t countries_range { 100 };

    const auto bind_callback = [countries_range, &countries_out](sqlite3_stmt* stmt) -> SQLite::Result::Code {
        if (const auto code = SQLite::bind_stmt(*stmt, 1, countries_range); SQLITE_OK != code) {
            return code;
        }

        // Loop through the results, a row at a time
        SQLite::Result::Code code = SQLite::step(*stmt);
        for (; code == SQLITE_ROW; code = SQLite::step(*stmt)) {
            Country country {
                .id = sqlite3_column_int(stmt, 0),
                .name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)),
                .population = sqlite3_column_int(stmt, 2),
                .area = sqlite3_column_int(stmt, 3)
            };
            countries_out.push_back(std::move(country));
        }
        return code;
    };

    constexpr std::string_view query { R"(SELECT id, name, population, area FROM countries WHERE id <= ?;)" };

    const auto result = con.execute_stmt(query, bind_callback).get();
    if (result.code != SQLITE_OK && result.code != SQLITE_DONE) {
        return std::unexpected(result);
    }
    return countries_out;
}

inline auto delete_country(const SQLite::Conn& con) -> SQLite::Result
{
    static const Models::Country country {
        .name = "United States",
    };

    const auto bind_callback = [](sqlite3_stmt* stmt) -> SQLite::Result::Code {
        if (const auto code = SQLite::bind_stmt(*stmt, 1, country.name); SQLITE_OK != code) {
            return code;
        }
        return SQLite::step(*stmt);
    };

    constexpr std::string_view query { R"(DELETE FROM countries WHERE name = ?;)" };

    std::future<SQLite::Result> res = con.execute_stmt(query, bind_callback);
    return res.get();
}

} // Models
