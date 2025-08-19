# SQLite driver header-only library target
find_package(SQLite3 REQUIRED)

add_library(SQLiteWrap INTERFACE)

target_link_libraries(SQLiteWrap INTERFACE SQLite::SQLite3)
target_include_directories(SQLiteWrap INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
