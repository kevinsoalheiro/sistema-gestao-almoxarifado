# C7 - Testes doctest e cobertura gcovr
CXX = g++
FLAGS = -std=c++17 -Wall -Wextra -Iinclude
CXXFLAGS = $(FLAGS) -O0 -g --coverage
LDFLAGS = --coverage

FONTES = $(filter-out src/main.cpp,$(wildcard src/*.cpp))
TESTES = tests/doctest_main.cpp $(filter-out tests/test_almoxarifado.cpp,$(wildcard tests/test_*.cpp))
EXECUTAVEL = build/testes.exe
PROGRAMA = build/almoxarifado.exe

.PHONY: all run test clean

all:
	@mkdir -p build
	$(CXX) $(FLAGS) $(wildcard src/*.cpp) -o $(PROGRAMA)

run: all
	./$(PROGRAMA)

test:
	@mkdir -p build
	@rm -f build/*.gcda
	$(CXX) $(CXXFLAGS) $(FONTES) $(TESTES) -o $(EXECUTAVEL) $(LDFLAGS)
	@./$(EXECUTAVEL); resultado=$$?; \
	gcovr --root . --filter 'src/' --object-directory build \
	  --txt build/cobertura.txt --html-details build/cobertura.html \
	  --print-summary; cobertura=$$?; \
	if [ $$resultado -ne 0 ]; then exit $$resultado; fi; \
	exit $$cobertura

clean:
	rm -f build/testes.exe build/almoxarifado.exe build/*.gcda build/*.gcno \
	  build/cobertura.txt build/cobertura*.html
