CXX = g++
CXXFLAGS = -std=c++17
TARGET = main
OBJECT = image

$(TARGET): $(TARGET).cpp $(OBJECT).h $(OBJECT).cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(TARGET).cpp $(OBJECT).cpp

clean:
	$(RM) $(TARGET)