CXX := g++
CXXFLAGS := -Wall -O2
LDFLAGS := -lgpiod
TARGET := pulse_measure
SRC := main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)