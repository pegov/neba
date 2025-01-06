.PHONY: build run clean

build:
	@mkdir -p build/
	g++ -o build/neba src/main.cpp

run:
	@./build/neba

clean:
	rm -rf build/

