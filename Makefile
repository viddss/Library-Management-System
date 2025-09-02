CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17
TARGET = library_system
SRCS = main.cpp Book.cpp Account.cpp User.cpp Library.cpp
OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean 