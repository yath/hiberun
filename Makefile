CC=i586-mingw32msvc-gcc

hiberun.exe: hiberun.c
	$(CC) -o $@ $< -lpowrprof
