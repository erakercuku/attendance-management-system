CXX = g++
CXXFLAGS = -Iinclude -Wall
LDLIBS = -lsqlite3
SRC = src/main.cpp src/user.cpp src/authentication.cpp src/storage.cpp src/time.cpp src/input.cpp
OBJ = $(SRC:.cpp=.o)
# OBJS = src/main.o, src/user.o, src/authentication.o, src/storage.o, src/time.o, src/input.o, 
TARGET = project
CSV_TARGET = csv_project
RM = del

all: $(TARGET)
$(CSV_TARGET): CXXFLAGS += -DCSV_VER

$(TARGET): $(OBJ)
	@echo Creating Attendance Management System Project...
	$(CXX) -o $@ $^ $(LDLIBS)

$(CSV_TARGET): $(OBJ)
	@echo Creating CSV-based version of the Attendance Management System Project...
	$(CXX) -o $@ $^ $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

clean:
	@echo Deleting non-source-code files...
# $(RM) $(OBJ) project.exe

.PHONY: all clean
