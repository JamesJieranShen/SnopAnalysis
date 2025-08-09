# Variables
BUILD_DIR := build
BIN_DIR := bin
CMAKE_FLAGS := -DCMAKE_BUILD_TYPE=Release
TARGET ?= SnopAnalysis

.PHONY: all configure build clean run

all: build

build:
	@mkdir -p $(BUILD_DIR)
	cmake -S . -B $(BUILD_DIR) $(CMAKE_FLAGS)
	cmake --build $(BUILD_DIR) -- $(MAKEFLAGS)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_DIR)

run: all
	./$(BUILD_DIR)/$(TARGET)

