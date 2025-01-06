.PHONY: build run clean

build:
	@mkdir -p build/
	g++ -Ivendor/include -fuse-ld=mold -Lvendor/lib -o build/neba src/main.cpp -lraylib

run:
	@./build/neba

clean:
	rm -rf build/

