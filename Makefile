CXX := g++
CXXFLAGS := -Wall -Werror -Wextra -g -std=c++11 -MP -MMD \
	-I include/
TARGET := chip8vm

SRCS := src/main.cpp src/vm.cpp
OBJS := $(SRCS:%.cpp=%.o)
DEPS := $(SRCS:%.cpp=%.d)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS)

clean:
	$(RM) $(OBJS) $(TARGET) $(DEPS)

-include $(DEPS)
