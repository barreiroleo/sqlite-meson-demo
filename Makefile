.PHONY: gen build run test valgrind-test

grn:
	meson setup --reconfigure build -Db_coverage=true

build:
	meson compile -C build

test:
	meson test -C build

valgrind-test:
	meson test -C build --wrap='valgrind'
