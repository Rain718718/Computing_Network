SRC_DIR := src
OBJ_DIR := obj
# all src files
SRC := $(wildcard $(SRC_DIR)/*.c)
# all objects
OBJ := $(OBJ_DIR)/y.tab.o $(OBJ_DIR)/lex.yy.o $(OBJ_DIR)/parse.o $(OBJ_DIR)/example.o $(OBJ_DIR)/logger.o $(OBJ_DIR)/response.o
# all yacc, lex and parse objects
YLOOBJ := $(OBJ_DIR)/y.tab.o $(OBJ_DIR)/lex.yy.o $(OBJ_DIR)/parse.o $(OBJ_DIR)/logger.o $(OBJ_DIR)/response.o
# all binaries
# BIN := example echo_server echo_client
BIN := example liso_server liso_client
# C compiler
CC  := gcc
# C PreProcessor Flag
CPPFLAGS := -Iinclude
# compiler flags
CFLAGS   := -g -Wall
# DEPS = parse.h y.tab.h

default: all
all : example liso_server liso_client
# all : example echo_server echo_client

example: $(OBJ)
	$(CC) $^ -o $@

# test_example: example
# 	@for f in $(shell ls samples); do \
# 		echo "=====Test file" $$f "========="; \
# 		./example samples/$$f | grep Segmentation --color ; \
# 		echo "---------------------------------------\n"; \
# 	done	

$(SRC_DIR)/lex.yy.c: $(SRC_DIR)/lexer.l
	flex -o $@ $^

$(SRC_DIR)/y.tab.c: $(SRC_DIR)/parser.y
	yacc -d $^
	mv y.tab.c $@
	mv y.tab.h $(SRC_DIR)/y.tab.h

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# echo_server: $(YLOOBJ) $(OBJ_DIR)/echo_server.o
liso_server: $(YLOOBJ) $(OBJ_DIR)/liso_server.o
	$(CC) -Werror $^ -o $@

# echo_client: $(OBJ_DIR)/echo_client.o
liso_client: $(OBJ_DIR)/liso_client.o
	$(CC) -Werror $^ -o $@

$(OBJ_DIR):
	mkdir $@

clean:
	$(RM) $(OBJ) $(BIN) $(SRC_DIR)/lex.yy.c $(SRC_DIR)/y.tab.*
	$(RM) -r $(OBJ_DIR)
