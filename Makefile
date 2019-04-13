APP=v
LIBS=-lncursesw
CFLAGS=-Wall -std=c11 -O2

SRC_FILES=main.c buffer.c line_buffer.c
HEADERS=common.h buffer.h line_buffer.h

clean:
		rm $(APP)


$(APP): $(HEADERS) $(SRC_FILES)
	 	 gcc $(CFLAGS) -o $(APP) -static $(SRC_FILES) $(LIBS)
