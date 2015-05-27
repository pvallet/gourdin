CIBLE = out
SRCS =  $(wildcard src/*.cpp)
LIBS =  -lsfml-graphics -lsfml-window -lsfml-system -lGLU -lGL -lX11 -lGLEW

CFLAGS = -Wall -std=c++11 -g

OBJS = $(addprefix obj/,$(notdir $(SRCS:.cpp=.o)))

$(CIBLE): $(OBJS)
	g++ $^ -o $(CIBLE) $(LIBS) $(LDFLAGS)
clean:
	rm -f  *~  $(CIBLE) $(OBJS)

obj/main.o: src/main.cpp
	g++ -c $< -o $@ $(LIBS) $(CFLAGS)
obj/%.o: src/%.cpp src/%.h
	g++ -c $< -o $@ $(LIBS) $(CFLAGS)
	
