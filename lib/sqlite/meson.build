sqlite = dependency('sqlite3', version: '>= 3.0.0')

# Interface library
sqlite_cpp_inc = include_directories('.')

sqlite_cpp_dep = declare_dependency(
  include_directories: sqlite_cpp_inc,
  dependencies: sqlite,
)
