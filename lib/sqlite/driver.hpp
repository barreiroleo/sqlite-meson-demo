#pragma once

#include <sqlite3.h>

#include <expected>
#include <future>

namespace SQLite {

/// @brief Result structure for SQLite operations
struct Result {
    using Code = int;
    std::string msg {};
    Code code { SQLITE_OK };
};

/// @brief Callback function for SQLite queries
/// @param[out] data: User data passed to the callback
/// @param[in] argc: Number of columns in the result set
/// @param[in] argv: Array of column values
/// @param[in] az_col_name: Array of column names
/// @return 0 on success, non-zero on error
using ExecCallback = Result::Code(void* data, int argc, char** argv, char** az_col_name);
using StmtCallback = std::function<Result::Code(sqlite3_stmt*)>;

/// @brief Connection class for managing SQLite database connections
class Conn {
public:
    /// @brief Factory method to create a new connection to the SQLite database.
    /// @param[in] file Path to the SQLite database file.
    /// @result A unique pointer to the Conn object on success, or an error Result on failure.
    static auto setup(std::string_view file) noexcept -> std::expected<std::unique_ptr<Conn>, Result>
    {
        try {
            return std::unique_ptr<Conn> { new Conn { file } };
        } catch (const std::exception& e) {
            return std::unexpected(Result { .msg = e.what(), .code = SQLITE_FAIL });
        };
    }

    ~Conn()
    {
        sqlite3_close(m_db);
    }

    /// @brief Exec executes a query without returning any rows.
    ///        The args are for any placeholder parameters in the query.
    /// @param[in] command Sqlite command to execute.
    /// @param[in] callback Callback function to handle results.
    /// @param[in] callback_args Callback arguments to pass to the callback function.
    /// @return A future that resolves to a Result containing the execution status.
    [[nodiscard]]
    auto execute(std::string_view command, ExecCallback* callback = nullptr, void* callback_args = nullptr) const -> std::future<Result>
    {
        return std::async(std::launch::async, [this, command, callback, callback_args]() {
            return execute_sync(command, callback, callback_args);
        });
    }

    /// @brief Executes a prepared statement with a binding callback.
    /// @param[in] command Sqlite command to prepare and execute.
    /// @param[in] bind_callback Callback function to bind parameters to the prepared statement.
    /// @return A future that resolves to a Result containing the execution status.
    [[nodiscard]]
    auto execute_stmt(std::string_view command, const StmtCallback& bind_callback) const -> std::future<Result>
    {
        return std::async(std::launch::async, [this, command, &bind_callback]() {
            return execute_stmt_sync(command, bind_callback);
        });
    }

private:
    Conn(std::string_view file)
    {
        if (SQLITE_OK != sqlite3_open(file.data(), &m_db)) {
            throw std::runtime_error(sqlite3_errmsg(m_db));
        }
    }

    /// @brief Executes a query synchronously without returning any rows.
    /// See execute() for asynchronous execution.
    [[nodiscard]]
    auto execute_sync(std::string_view command, ExecCallback* callback = nullptr, void* callback_args = nullptr) const -> Result
    {
        char* msg {};
        std::lock_guard<std::mutex> lock(m_mutex);

        if (const auto code = sqlite3_exec(m_db, command.data(), callback, callback_args, &msg); SQLITE_OK != code) {
            return { .msg = msg, .code = code };
        }
        return {};
    }

    /// @brief Executes a prepared statement synchronously.
    /// See execute_stmt() for asynchronous execution.
    [[nodiscard]]
    auto execute_stmt_sync(std::string_view command, const StmtCallback& bind_callback) const -> Result
    {
        sqlite3_stmt* stmt {};
        std::lock_guard<std::mutex> lock(m_mutex);

        if (const auto code = sqlite3_prepare_v2(m_db, command.data(), -1, &stmt, NULL); SQLITE_OK != code) {
            return { .msg = sqlite3_errmsg(m_db), .code = code };
        }

        if (const auto code = bind_callback(stmt); SQLITE_OK != code) {
            return { .msg = sqlite3_errmsg(m_db), .code = code };
        }

        if (const auto code = sqlite3_finalize(stmt); SQLITE_OK != code) {
            return { .msg = sqlite3_errmsg(m_db), .code = code };
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

/// @brief Binds a value to a prepared statement at the specified column index.
/// @param[in] stmt The prepared statement.
/// @param[in] col The column index (1-based).
/// @param[in] value The string value to bind.
/// @retrun Result::Code
static auto bind_stmt(sqlite3_stmt& stmt, int col, std::string_view value) -> Result::Code
{
    return sqlite3_bind_text(&stmt, col, value.data(), -1, SQLITE_TRANSIENT);
}

/// @brief Binds a value to a prepared statement at the specified column index.
/// @param[in] stmt The prepared statement.
/// @param[in] col The column index (1-based).
/// @param[in] value The string value to bind.
/// @retrun Result::Code
static auto bind_stmt(sqlite3_stmt& stmt, int col, int value) -> Result::Code
{
    return sqlite3_bind_int(&stmt, col, value);
}

/// @brief Steps through the prepared statement and resets it for future use.
/// @param[in] stmt The prepared statement.
/// @return True if the step and reset were successful, false otherwise.
static auto step(sqlite3_stmt& stmt) -> Result::Code
{
    if (const auto code = sqlite3_step(&stmt); SQLITE_OK != code) {
        return code;
    }
    return sqlite3_reset(&stmt);
}

}
