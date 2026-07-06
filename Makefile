CXX = g++
CC = gcc
CXXFLAGS = -std=c++14 -Wall -Wextra -O2
CFLAGS = -std=c11 -Wall -Wextra -O2

SYSTEMC_INC = $(SYSTEMC_HOME)/include
SYSTEMC_LIB = $(SYSTEMC_HOME)/lib

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

CXXFLAGS += -I$(INC_DIR) -I$(SYSTEMC_INC)
CFLAGS += -I$(INC_DIR)
LDFLAGS = -L$(SYSTEMC_LIB) -lsystemc -lm

CPP_SRCS = $(wildcard $(SRC_DIR)/*.cpp)
C_SRCS = $(wildcard $(SRC_DIR)/*.c)

CPP_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CPP_SRCS))
C_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(C_SRCS)) 

OBJS = $(CPP_OBJS) $(C_OBJS)
TARGET = project

all: $(TARGET)

$(TARGET): $(OBJS)
		$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
		$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
		$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
		mkdir -p $(OBJ_DIR)

clean:
		rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all project clean