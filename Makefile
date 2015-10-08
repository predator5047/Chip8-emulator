CXX := g++
CXXFLAGS := -Wall -Werror -Wextra -g -std=c++11 -MP -MMD
TARGET := chip8vm

SRCS := main.cpp vm.cpp
OBJS := $(SRCS:%.cpp=%.o)
DEPS := $(SRCS:%.cpp=%.d)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS)

clean:
	$(RM) $(OBJS) $(TARGET) $(DEPS)

-include $(DEPS)
