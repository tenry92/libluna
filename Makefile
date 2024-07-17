GNUMAKEFLAGS=--no-print-directory

# one of: Debug, Release
BUILD_TYPE := Debug

CMAKE ?= cmake

.PHONY: libluna libluna-n64 libluna-switch
.PHONY: clean clean-n64 clean-switch
.PHONY: docs format

libluna:
	@mkdir -p build
	@export export CMAKE_BUILD_PARALLEL_LEVEL=$(shell nproc)
	@$(CMAKE) -S . -B build -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	@$(CMAKE) --build build

clean:
	@rm -rf build

libluna-n64:
	@mkdir -p build-n64
	@export CMAKE_BUILD_PARALLEL_LEVEL=$(shell nproc)
	@$(CMAKE) -S . -B build-n64 -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_TOOLCHAIN_FILE=cmake/Nintendo64.cmake
	@$(CMAKE) --build build-n64

clean-n64:
	@rm -rf build-n64

libluna-switch:
	@mkdir -p build-switch
	@export CMAKE_BUILD_PARALLEL_LEVEL=$(shell nproc)
	@export DEVKITPRO=/opt/devkitpro
	@$(CMAKE) -S . -B build-switch -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_TOOLCHAIN_FILE=/opt/devkitpro/cmake/Switch.cmake
	@$(CMAKE) --build build-switch

clean-switch:
	@rm -rf build-switch

docs:
	@doxygen
	@cd docs/sphinx && ./build.sh

format:
	@find libluna -type f -iname *.hpp -o -iname *.cpp | xargs clang-format -i
