CXX := g++
CXXFLAGS := -Wall -Werror -Wextra -g -std=c++11
TARGET := chip8vm

SRCS := main.cpp
OBJS := $(SRCS:%.cpp=%.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS)

clean:
	$(RM) $(OBJS) $(TARGET)
