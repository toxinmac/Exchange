CC = gcc
CFLAGS := -lstdc++ -std=c++17 

EXECUTABLE = test


SERVER_TEST = ./tests/test_server.cpp Server.cpp

SOURCE = $(SERVER_TEST)

UNAME = $(shell uname)

OPEN_REPORT :=
LEAKS :=

ifeq ($(UNAME), Linux)
OPEN_REPORT += xdg-open
LEAKS += valgrind --leak-check=full -s -q --track-origins=yes
endif

ifeq ($(UNAME), Darwin)
OPEN_REPORT += open
LEAKS += leaks -atExit --
endif

all: clean test

clean:
	@rm -rf $(EXECUTABLE)
	@rm -rf a.out
	@rm -rf *.gcno *.gcda report
	@rm -rf tests.info

leaks_check: test
	@$(LEAKS) ./$(EXECUTABLE)

test:
	@$(CC) $(CFLAGS) $(SERVER_TEST) -lgtest_main -lgtest -o $(EXECUTABLE) && ./$(EXECUTABLE)

run:
	./$(EXECUTABLE)

.PHONY: all clean leaks_check run