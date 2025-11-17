# SQLite driver header-only library target
find_package(SQLite3 REQUIRED)

add_library(sqlite-cpp INTERFACE)

target_link_libraries(sqlite-cpp INTERFACE SQLite::SQLite3)
target_include_directories(sqlite-cpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
