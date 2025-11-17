project(
  'sqlite-demo',
  'cpp',
  version: 'v0.0.1',
  meson_version: '>= 1.3.0',
  default_options: ['warning_level=3', 'cpp_std=c++23'],
)

subdir('lib/sqlite')
inc_lib = include_directories('lib')

sqlite_demo = executable(
  'sqlite-demo',
  'src/main.cpp',
  dependencies: sqlite_cpp_dep,
  include_directories: inc_lib,
)

test(
  'snapshot_test',
  files('scripts/snapshot.sh'),
  depends: [sqlite_demo],
  timeout: 30,
  suite: 'snapshot',
)
