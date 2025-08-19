MAKEFLAGS += --silent

.EXPORT_ALL_VARIABLES:
ASAN_OPTIONS="symbolize=1:color=always"
ASAN_SYMBOLIZER_PATH=$(bash which llvm-symbolizer)
GTEST_COLOR=1

.PHONY: gen
gen:
	@cmake -B build -G Ninja \
		-DFETCHCONTENT_QUIET=false -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
		-DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_CXX_COMPILER='clang++' \
		-DCMAKE_COLOR_DIAGNOSTICS=ON # -DCMAKE_VERBOSE_MAKEFILE=ON

.PHONY: build
build:
	@cmake --build build --config Debug --target all -j

.PHONY: clean
clean:
	@cmake --build build --target clean -v

.PHONY: test
test: build
	ctest -j10 --test-dir build --output-on-failure --verbose

.PHONY: graph
graph:
	@ninja -C build -t graph all | dot -Tpng -o build_graph.png
