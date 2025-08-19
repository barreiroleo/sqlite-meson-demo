#pragma once

#include <sqlite3.h>

#include <expected>
#include <future>
#include <memory>
#include <string>

namespace SQLite {

/// @brief Result structure for SQLite operations

struct Result {
    std::string msg {};
    int code { SQLITE_OK };
};

/// @brief Callback function for SQLite queries
/// @param[out] data: User data passed to the callback
/// @param[in] argc: Number of columns in the result set
/// @param[in] argv: Array of column values
/// @param[in] az_col_name: Array of column names
/// @return 0 on success, non-zero on error
using Callback = int(void* data, int argc, char** argv, char** az_col_name);

/// @brief Connection class for managing SQLite database connections
class Conn {
public:
    /// @brief Factory method to create a new connection to the SQLite database.
    ///
    /// @param[in] file Path to the SQLite database file.
    /// @result A unique pointer to the Conn object on success, or an error Result on failure.
    static auto setup(std::string_view file) noexcept -> std::expected<std::unique_ptr<Conn>, Result>
    {
        try {
            return std::unique_ptr<Conn> { new Conn { file } };
        } catch (const std::exception& e) {
            return std::unexpected(SQLite::Result { .msg = e.what(), .code = SQLITE_FAIL });
        };
    }

    ~Conn()
    {
        sqlite3_close(m_db);
    }

    /// @brief Exec executes a query without returning any rows.
    ///        The args are for any placeholder parameters in the query.
    ///
    /// @param[in] command Sqlite command to execute.
    /// @param[in] callback Callback function to handle results.
    /// @param[in] callback_args Callback arguments to pass to the callback function.
    /// @return A future that resolves to a Result containing the execution status.
    [[nodiscard]]
    auto execute(std::string_view command, Callback* callback = nullptr, void* callback_args = nullptr) const -> std::future<Result>
    {
        return std::async(std::launch::async, [this, command, callback, callback_args]() {
            return execute_sync(command, callback, callback_args);
        });
    }

private:
    Conn(std::string_view file)
    {
        if (sqlite3_open(file.data(), &m_db) != SQLITE_OK) {
            throw std::runtime_error(sqlite3_errmsg(m_db));
        }
    }

    /// @brief Executes a query synchronously without returning any rows.
    ///        See execute() for asynchronous execution.
    [[nodiscard]]
    auto execute_sync(std::string_view command, SQLite::Callback* callback = nullptr, void* callback_args = nullptr) const -> Result
    {
        char* msg {};
        std::lock_guard<std::mutex> lock(m_mutex);

        if (const auto code = sqlite3_exec(m_db, command.data(), callback, callback_args, &msg); code != SQLITE_OK) {
            return { .msg = msg, .code = code };
        }
        return {};
    }

    Conn(const Conn&) = delete;
    Conn(Conn&&) = delete;
    Conn& operator=(const Conn&) = delete;
    Conn& operator=(Conn&&) = delete;

private:
    sqlite3* m_db {};
    static inline std::mutex m_mutex {};
};

}
