CXX = g++
CXXFLAGS = -std=c++14 -Wall -Wextra -O2

SYSTEMC_INC = $(SYSTEMC_HOME)/include
SYSTEMC_LIB = $(SYSTEMC_HOME)/lib-linux64

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

CXXFLAGS += -I$(INC_DIR) -I$(SYSTEMC_INC)
LDFLAGS = -L$(SYSTEMC_LIB) -lsystemc -lm

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

TARGET = project

all: $(TARGET)
project: $(TARGET)
$(TARGET): $(OBJS)
		$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
		$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
		mkdir -p $(OBJ_DIR)

clean:
		rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all project clean