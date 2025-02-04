.PHONY: build run clean

build:
	@mkdir -p build/
	cmake -B build -DVENDOR_DIR=vendor && cmake --build build -- -j16

run:
	@./build/neba

clean:
	rm -rf build/

