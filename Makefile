APP=v
LIBS=-lncursesw
CFLAGS=-Wall -std=c11 -O2


clean:
		rm $(APP)


$(APP): main.c
	 	 gcc $(CFLAGS) -o $(APP) -static main.c $(LIBS)
