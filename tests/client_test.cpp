#include <src/sqlite3_wrapper.hpp>

#include <vector>

namespace {

struct Book {
    using USDCent = float;
    int id;
    std::string title;
    std::string author;
    int publication_year;
    USDCent price;
};
using Books = std::vector<Book>;

constexpr std::string_view create_books_table { R"(
CREATE TABLE IF NOT EXISTS books (
    id INTEGER PRIMARY KEY NOT NULL,
    title TEXT NOT NULL,
    author TEXT NOT NULL,
    publication_year INTEGER NOT NULL,
    price INTEGER NOT NULL);
)" };

constexpr std::string_view insert_books { R"(
INSERT OR REPLACE INTO books (id, title, author, publication_year, price)
VALUES
    (1, '1989', 'George Orwell', 1949, 9.99),
    (2, 'Lord of the rings', 'J. R. R. Tolkien', 1954, 4.99),
    (3, 'The Great Gatsby', 'F. Scott Fitzgerald', 1925, 10.99),
    (4, 'The Catcher in the Rye', 'J. D. Salinger', 1951, 12.99),
    (5, 'To Kill a Mockingbird', 'Harper Lee', 1960, 14.99),
    (6, 'The Hobbit', 'J. R. R. Tolkien', 1937, 16.99);
)" };

constexpr std::string_view delete_student_entry { R"(DELETE FROM books WHERE id = 2)" };

constexpr std::string_view update_student_entry { R"(UPDATE books SET price = "99.99" WHERE id = 2)" };

struct select_title_author {
    static constexpr std::string_view cmd { R"(SELECT id, title, author FROM books)" };

    static auto callback(void* data, int argc, char** argv, char** az_col_name) -> int
    {
        Books* books = static_cast<Books*>(data);
        // for (size_t i {}; i < argc; ++i) {
        //     std::println("{}: {}", az_col_name[i], argv[i]);
        // }
        for (size_t i {}; i < argc; ++i) {
            Book new_book {
                .id = argv[i] ? std::atoi(argv[i]) : -1,
                .title = argv[++i] ? argv[i] : "",
                .author = argv[++i] ? argv[i] : "",
            };
            books->push_back(std::move(new_book));
        }
        return 0;
    }

    static_assert(std::is_same<decltype(callback), SQLite::Callback>::value, "Callback doesn't met the callback signature");
};

}

int main(int argc, char* argv[])
{
    SQLite::DBConn connection { "library.db" };
    if (!connection.open()) {
        return 1;
    };

    if (!connection.execute(create_books_table)) {
        return 1;
    }

    if (!connection.execute(insert_books)) {
        return 1;
    }

    Books books;
    auto query = connection.execute_async(select_title_author::cmd, select_title_author::callback, &books);
    if (!query.get()) {
        return 1;
    }
    for (const auto& book : books) {
        std::println("Id: {}, Title: {}, Author: {}", book.id, book.title, book.author);
    }

    if (!connection.execute(update_student_entry)) {
        return 1;
    }

    if (!connection.execute(delete_student_entry)) {
        return 1;
    }

    return 0;
}
