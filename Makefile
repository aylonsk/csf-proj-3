 CXX = g++
CXXFLAGS = -g -Wall -pedantic -std=c++17

# Add any additional source files here
SRCS = main.cpp
OBJS = $(SRCS:.cpp=.o)

# Executable
TARGET = csim


# When submitting to Gradescope, submit all .cpp and .h files,
# as well as README.txt
FILES_TO_SUBMIT = $(shell ls *.cpp *.h README.txt 2> /dev/null)

# Rule for compiling .cpp to .o
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $*.cpp -o $*.o

# Executable target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Target to create a solution.zip file you can upload to Gradescope
.PHONY: solution.zip
solution.zip :
	zip -9r $@ $(FILES_TO_SUBMIT)

# Generate header file dependencies
depend :
	$(CXX) $(CXXFLAGS) -M $(SRCS) > depend.mak

depend.mak :
	touch $@

clean :
	rm -f csim *.o

include depend.mak
