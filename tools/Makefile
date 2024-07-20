PKGS = glib-2.0
CFLAGS += -Wall $(shell pkg-config --cflags $(PKGS))
LIBS = $(shell pkg-config --libs $(PKGS))
TOOLS = wordle-tool wordle-stats

all: $(TOOLS)

clean:
	rm -f $(TOOLS) *~

%: %.c
	gcc $(CFLAGS) -o $@ $< $(LIBS)
