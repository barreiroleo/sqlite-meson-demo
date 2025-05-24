#include <sqlite3.h>

#include <print>
#include <string_view>

class DBConn {
public:
    DBConn(std::string_view db_name)
        : m_db_name(db_name) { };

    // TODO: Ideally, applications should [sqlite3_finalize | finalize] all [prepared statements],
    // [sqlite3_blob_close | close] all [BLOB handles], and [sqlite3_backup_finish | finish] all
    // [sqlite3_backup] objects associated with the [sqlite3] object prior to attempting to close
    // the object.
    ~DBConn()
    {
        if (!m_is_opened) {
            std::println("[WARN] You're trying to close an unopened connection");
            return;
        }
        sqlite3_close(m_db_ptr);
    };

    [[nodiscard]] auto open() -> bool
    {
        m_is_opened = (sqlite3_open(m_db_name.c_str(), &m_db_ptr) == SQLITE_OK);
        if (!m_is_opened) {
            std::println("Database could not be opened: {}", sqlite3_errmsg(m_db_ptr));
        }
        return m_is_opened;
    }

    [[nodiscard]] auto execute(std::string_view command) const -> bool
    {
        char* err_msg { nullptr };
        const int err = sqlite3_exec(m_db_ptr, command.data(), nullptr, nullptr, &err_msg);
        if (err != SQLITE_OK) {
            std::println("Error running command: {}", err_msg);
        }
        return err == SQLITE_OK;
    }

public:
    DBConn(DBConn&&) = delete;
    DBConn(const DBConn&) = delete;
    DBConn& operator=(DBConn&&) = delete;
    DBConn& operator=(const DBConn&) = delete;

private:
    std::string m_db_name;
    sqlite3* m_db_ptr { nullptr };
    bool m_is_opened { false };
};

namespace {

constexpr std::string_view create_students_table { R"(
CREATE TABLE IF NOT EXISTS student(
    id INT PRIMARY KEY NOT NULL,
    first_name TEXT NOT NULL,
    last_name TEXT NOT NULL,
    email TEXT NOT NULL); )" };

constexpr std::string_view insert_students_data { R"(
INSERT OR REPLACE INTO student(id, first_name, last_name, email) VALUES
    (1, "John", "Doe", "john@doe.com"),
    (2, "Peter", "Griffin", "peter@griffin.com"),
    (3, "Homer", "Simpson", "homer@simpson.com"); )" };

constexpr std::string_view delete_student_entry { R"(
DELETE FROM "student" WHERE ID=2;
)" };

constexpr std::string_view update_student_entry { R"(
UPDATE "student" SET EMAIL = "random@gmail.com" WHERE ID=2;
)" };
}

int main(int argc, char* argv[])
{
    DBConn connection { "aquarium.db" };
    if (!connection.open()) {
        return 1;
    };

    if (!connection.execute(create_students_table)) {
        return 1;
    }

    if (!connection.execute(insert_students_data)) {
        return 1;
    }

    if (!connection.execute(update_student_entry)) {
        return 1;
    }

    if (!connection.execute(delete_student_entry)) {
        return 1;
    }

    return 0;
}
