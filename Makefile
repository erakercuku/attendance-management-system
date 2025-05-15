CXX = g++
CXXFLAGS = -Iinclude -Wall
LDLIBS = -lsqlite3
SRC = src/main.cpp src/user.cpp src/authentication.cpp src/storage.cpp src/time.cpp src/input.cpp src/qr_code.cpp src/qr/qrcodegen.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = project
CSV_TARGET = csv_project
RM = del /f

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
	$(RM) $(TARGET).exe $(CSV_TARGET).exe
	$(RM) src\*.o

.PHONY: all clean
