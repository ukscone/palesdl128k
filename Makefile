CC = gcc -std=c99
OBJ = io.o pale.o sound.o disk.o video.o rom.o lynx128k.o keys.o taps.o rawtape.o KOGEL/z80.o fonts.o directory.o gui.o
LINKOBJ = io.o pale.o sound.o disk.o video.o rom.o lynx128k.o keys.o taps.o rawtape.o KOGEL/z80.o fonts.o directory.o gui.o
LIBS = -ldsk `sdl-config --libs`
INCS = -I/usr/local/include
BIN = palesdl128k
CFLAGS =-fsigned-char -fexpensive-optimizations -O3  -mcpu=arm1176jzf-s $(INCS)
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after

clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LINKOBJ) -o $(BIN) $(LIBS)

video.o: video.c
	$(CC) -c video.c -o video.o $(CFLAGS)

io.o: io.c
	$(CC) -c io.c -o io.o $(CFLAGS)

pale.o: pale.c
	$(CC) -c pale.c -o pale.o $(CFLAGS)

rom.o: rom.c
	$(CC) -c rom.c -o rom.o $(CFLAGS)

sound.o: sound.c
	$(CC) -c sound.c -o sound.o $(CFLAGS)

lynx128k.o: lynx128k.c
	$(CC) -c lynx128k.c -o lynx128k.o $(CFLAGS)

keys.o: keys.c
	$(CC) -c keys.c -o keys.o $(CFLAGS)

taps.o: taps.c
	$(CC) -c taps.c -o taps.o $(CFLAGS)

disk.o: disk.c
	$(CC) -c disk.c -o disk.o $(CFLAGS)

rawtape.o: rawtape.c
	$(CC) -c rawtape.c -o rawtape.o $(CFLAGS)

gui.o: gui.c
	$(CC) -c gui.c -o gui.o $(CFLAGS)

fonts.o: fonts.c
	$(CC) -c fonts.c -o fonts.o $(CFLAGS)

directory.o: directory.c
	$(CC) -c directory.c -o directory.o $(CFLAGS)

KOGEL/z80.o: KOGEL/z80.c
	$(CC) -c KOGEL/z80.c -o KOGEL/z80.o $(CFLAGS)
