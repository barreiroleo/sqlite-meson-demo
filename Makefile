.PHONY: gen build run test test-valgrind coverage

gen:
	meson setup --reconfigure build -Db_coverage=true

build:
	meson compile -C build

test:
	meson test -C build

test-valgrind:
	meson test -C build --wrap='valgrind'

coverage:
	@ninja -C build coverage > /dev/null 2>&1
	@cat build/meson-logs/coverage.txt
	@echo "HTML coverage report generated in ${PWD}/build/meson-logs/coveragereport/index.html"
