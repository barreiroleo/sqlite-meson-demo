MAKEFLAGS += --silent

.EXPORT_ALL_VARIABLES:
ASAN_OPTIONS="symbolize=1:color=always"
ASAN_SYMBOLIZER_PATH=$(bash which llvm-symbolizer)
GTEST_COLOR=1

.PHONY: default gen build test test-valgrind coverage graph
default:
	@if [ ! -d build ]; then $(MAKE) gen; fi
	$(MAKE) build

gen:
	meson setup --reconfigure build --buildtype=debug -Db_coverage=true

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

perf-launch:
	@bash ./scripts/profiling.sh --launch sqlite-demo

perf-listen:
	@bash ./scripts/profiling.sh --listen sqlite-demo

graph:
	@ninja -C build -t graph all | dot -Tpng -o build_graph.png


.PHONY: gen-cmake build-cmake test-cmake clean-cmake
gen-cmake:
	@cmake -B build -G Ninja \
		-DFETCHCONTENT_QUIET=false -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
		-DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_CXX_COMPILER='clang++' \
		-DCMAKE_COLOR_DIAGNOSTICS=ON # -DCMAKE_VERBOSE_MAKEFILE=ON

build-cmake:
	@cmake --build build --config Debug --target all -j

test-cmake: build-cmake
	ctest -j10 --test-dir build --output-on-failure --verbose

clean-cmake:
	@cmake --build build --target clean -v
