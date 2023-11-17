.ONESHELL:

CFLAGS += -g -Ofast -std=c++2a

SOURCES := "main.cpp"
SOURCES += $(wildcard src/*.cpp)
SOURCES += $(wildcard src/lexer/*.cpp)
SOURCES += $(wildcard src/parser/*.cpp)
SOURCES += $(wildcard src/parser/statements/*.cpp)

bin/koala main.cpp:
	mkdir -p bin

	c++ $(SOURCES) -o bin/koala \
		-I"src" \
		$(CFLAGS)

clean:
	rm -rf "bin"
