CC = g++
CFLAGS = -std=c++20 -Werror -ggdb
LIBS = -lncurses -lsfml-audio

main:main_copy.cpp
	$(CC) $(CFLAGS) main_copy.cpp -o snake2.0 $(LIBS) 
