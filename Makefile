NAME      := lego
FILES     := lego.c usage.c hash.c parse.c license.c image.c encode.c decode.c
BUILD_DIR := build
TEST_DIR  := test
BIN_DIR   := /usr/local/bin
EXEC      := ./$(BUILD_DIR)/$(NAME)

all: build

build: $(FILES) lego.h parse.h
	mkdir -p $(BUILD_DIR)
	gcc -Wall -Wextra -o $(EXEC) $(FILES) -lm -lpng -ljpeg -D_LEGO_TRANSPARENCY -O2

test: build
	mkdir -p $(TEST_DIR)
	curl -o $(TEST_DIR)/lisa.jpg http://upload.wikimedia.org/wikipedia/commons/thumb/6/6a/Mona_Lisa.jpg/396px-Mona_Lisa.jpg
	$(EXEC) encode $(TEST_DIR)/lisa.jpg
	$(EXEC) decode $(TEST_DIR)/lisa.lego

install: build
	install -m 0755 $(BUILD_DIR)/$(NAME) $(BIN_DIR)

uninstall:
	rm -f $(BIN_DIR)/$(NAME)

clean:
	rm -fr $(BUILD_DIR) $(TEST_DIR)/lisa*
