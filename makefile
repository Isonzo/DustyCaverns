OS = LINUX

IDIR = ./include/
LDIR = ./lib/
ODIR = ./obj/
SDIR = ./src/

CC = gcc
CFLAGS = -I$(IDIR) -L$(LDIR) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
NAME = caverns

ifeq ($(OS), WINDOWS)
CC = x86_64-w64-mingw32-gcc
CFLAGS = -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -s -O1 -I$(IDIR) -L.$(LDIR) -Wl,--subsystem,windows -lraylibwin -lopengl32 -lgdi32 -lwinmm -static -lpthread -DPLATFORM_DESKTOP
NAME = caverns.exe
endif
OBJ = main.o rogue.o

$(ODIR)%.o: $(SDIR)%.c 
	$(CC) -c -o $@ $< $(CFLAGS)

$(NAME): $(ODIR)$(OBJ)
	$(CC) -o bin/$(NAME) $^ $(CFLAGS)

clean:
	rm *.o

debug:
	gcc -g main.c rogue.c -I./include/ -L./libs/ -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
