CXX = g++
CXXFLAGS = -Iinclude -Wall
SRC = src/main.cpp src/user.cpp src/attendance.cpp src/authentication.cpp src/storage.cpp src/input.cpp
OBJ = $(SRC:.cpp=.o)
# OBJS = src/main.o, src/user.o, src/attendance.o, src/authentication.o, src/storage.o, src/input.o, 
TARGET = project
RM = del

all: $(TARGET)

$(TARGET): $(OBJ)
	@echo Creating Attendance Management System Project...
	$(CXX) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

clean:
	@echo Deleting non-source-code files...
# $(RM) $(OBJ) project.exe

.PHONY: all clean