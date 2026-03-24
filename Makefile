srcs := $(wildcard src/*.cpp)
objs := $(patsubst src/%.cpp,build/%.o,$(srcs))
app  := main.exe
deps := $(patsubst src/%.cpp,build/%.d,$(srcs))

CXXFLAGS := -g -std=c++20 -Wall -I ../raylib-5.5_win64_mingw-w64/include -MMD -MP
LDFLAGS := -static -lraylib -lopengl32 -lwinmm -lgdi32 -lpthread -lraylib -lopengl32 -lwinmm -lgdi32 -lpthread
LDLIBS := -L ..\raylib-5.5_win64_mingw-w64/lib
BUILD ?= debug

ifeq ($(BUILD),release)
	CXXFLAGS += -O2
else
	CXXFLAGS += -g
endif

-include $(deps)


$(app): build/main.o $(objs) 
	$(CXX)  $(LDLIBS) -o $@ $^ $(LDFLAGS) 

#run without debugger 
run: $(app)
	./$(app)

#run debugger
gdb: $(app)
	gdb ./$(app)

build/main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(objs) $(deps) $(app)