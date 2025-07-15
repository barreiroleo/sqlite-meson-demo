#pragma once

#include <future>
#include <iostream>
#include <mutex>
#include <sqlite3.h>
#include <string_view>

namespace SQLite {

/// @brief Callback function for SQLite queries
/// @param[out] data: User data passed to the callback
/// @param[in] argc: Number of columns in the result set
/// @param[in] argv: Array of column values
/// @param[in] az_col_name: Array of column names
/// @return 0 on success, non-zero on error
using Callback = int(void* data, int argc, char** argv, char** az_col_name);

class DBConn {
public:
    DBConn(std::string_view db_name)
        : m_db_name(db_name) { };

    // TODO: Ideally, applications should do all the following on objects associated with the
    // [sqlite3] object prior to attempting to close the object.
    // - [sqlite3_finalize | finalize] all [prepared statements],
    // - [sqlite3_blob_close | close] all [BLOB handles],
    // - [sqlite3_backup_finish | finish] all [sqlite3_backup]
    ~DBConn()
    {
        if (!m_is_opened) {
            std::cout << "[WARN] You're trying to close an unopened connection\n";
            return;
        }
        sqlite3_close(m_db_ptr);
    };

    [[nodiscard]]
    auto open() -> bool
    {
        m_is_opened = (sqlite3_open(m_db_name.c_str(), &m_db_ptr) == SQLITE_OK);
        if (!m_is_opened) {
            std::cout << std::format("Database could not be opened: {}\n", sqlite3_errmsg(m_db_ptr));
        }
        return m_is_opened;
    }

    [[nodiscard]]
    auto execute(std::string_view command, Callback* callback = nullptr, void* callback_args = nullptr) const -> bool
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (char* err_msg {}; sqlite3_exec(m_db_ptr, command.data(), callback, callback_args, &err_msg) != SQLITE_OK) {
            std::cout << std::format("Error running command: {}\n", err_msg);
            return false;
        }
        return true;
    }

    [[nodiscard]]
    auto execute_async(std::string_view command, Callback* callback = nullptr, void* callback_args = nullptr) const -> std::future<bool>
    {
        return std::async(std::launch::async, [this, command, callback, callback_args]() {
            return execute(command, callback, callback_args);
        });
    }

public:
    DBConn(DBConn&&) = delete;
    DBConn(const DBConn&) = delete;
    DBConn& operator=(DBConn&&) = delete;
    DBConn& operator=(const DBConn&) = delete;

private:
    static inline std::mutex m_mutex {};
    std::string m_db_name;
    sqlite3* m_db_ptr { nullptr };
    bool m_is_opened { false };
};

};
