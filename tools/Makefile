PKGS = glib-2.0
FLAGS = $(shell pkg-config --cflags $(PKGS))
LIBS = $(shell pkg-config --libs $(PKGS))

FLAGS += -g

all: wordle-tool

clean:
	rm -f wordle-tool *~

%: %.c
	gcc -Wall $(FLAGS) -o $@ $< $(LIBS)
