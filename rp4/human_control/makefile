CXX := g++
CXXFLAGS := -Wall -O2
LDFLAGS := -lgpiod -lpigpio
TARGET := human_control
SRC := main.cpp keyhandle.cpp motor.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)