# SQLite C++ Header-Only Wrapper

This is a silly, header-only C++ wrapper for SQLite. Just an excuse to try out
Meson the build system.

---

## Key Features

- **Header-Only Library**
  All functionality is implemented in `src/sqlite3_wrapper.hpp`. Just include
  this header in your project—no need to compile or link extra sources.

- **Meson Build System**
  The project uses Meson for configuration, dependency management, and build
  orchestration.

- **Snapshot Testing**
  Automated snapshot tests validate the current database state against a "last
  known good" (LKG) version. This helps catch regressions and unintended changes
  in database structure or content.

- **Automated Targets via Makefile**
  Common tasks such as build, test, coverage analysis, and memory checking
  (Valgrind) are automated through a simple Makefile interface.

- **Code Coverage**
  Integrated coverage reporting ensures that tests exercise all critical code
  paths. Coverage reports are generated in both text and HTML formats.

---

## Getting Started

### 1. Requirements

- Meson >= 1.3.0
- Ninja
- SQLite3 development files
- C++23 compatible compiler

### 2. Building and Testing

Use the provided Makefile for common tasks:

- `make gen`
  Configure the build directory with coverage enabled.

- `make build`
  Compile the project using Meson/Ninja.

- `make test`
  Run all automated tests (including snapshot tests).

- `make test-valgrind`
  Run tests under Valgrind to check for memory errors.

- `make coverage`
  Generate and display code coverage reports (text and HTML).

---

## Snapshot Testing

Snapshot tests ensure that the database schema and contents remain consistent. The workflow is:

1. Run the test executable to generate or modify the database.
2. Dump the current database state to SQL.
3. Compare the dump to the LKG snapshot (`scripts/lkg_library.db`).
4. If differences are found, the test fails—otherwise, it passes.

To update the snapshot after intentional changes:

```sh
cp library.db scripts/lkg_library.db
```

---

## Example Usage

Include the wrapper in your project:

```cpp
#include <src/sqlite3_wrapper.hpp>

SQLite::DBConn db("library.db");
if (db.open()) {
    // Use db to execute queries, manage transactions, etc.
}
```

See `tests/client_test.cpp` for more usage examples.
