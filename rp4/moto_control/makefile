CXX := g++
CXXFLAGS := -Wall -O2
LDFLAGS := -lgpiod -lpigpio
TARGET := moto_control
SRC := main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)