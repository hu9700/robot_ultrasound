CXX := g++
CXXFLAGS := -Wall -O2
LDFLAGS := -lgpiod -lpigpio
TARGET := follow_wall
SRC := main.cpp sensor.cpp motor.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)