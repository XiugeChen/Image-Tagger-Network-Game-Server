IDIR = include
ODIR = obj
SDIR = src
BIN_DIR = bin

CC = gcc
CFLAGS = -std=c99 -O3 -Wall -Wpedantic -I$(IDIR)

_DEPS = http_server.h socket_manage.h
_OBJ = http_server.o socket_manage.o image_tagger.o

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

image_tagger: $(OBJ)
	$(CC) -o $(BIN_DIR)/$@ $^ $(CFLAGS)

all: mkdirs image_tagger

.PHONY: clean

clean:
	rm -rf $(BIN_DIR) $(ODIR)

mkdirs:
	mkdir -p $(BIN_DIR) $(ODIR)
