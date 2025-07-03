project(
  'sqlite',
  'cpp',
  version: 'v0.0.1',
  meson_version: '>= 1.3.0',
  default_options: ['warning_level=3', 'cpp_std=c++23'],
)

sqlite = dependency('sqlite3', version: '>= 3.0.0')
dependencies = [sqlite]

sqlite3_wrapper_lib = library(
  'sqlite3_wrapper',
  'src/sqlite3_wrapper.hpp',
  dependencies: dependencies,
)

client_test = executable(
  'client_test',
  'tests/client_test.cpp',
  link_with: sqlite3_wrapper_lib,
  dependencies: dependencies,
)

test('client_test', client_test, suite: 'snapshot')
