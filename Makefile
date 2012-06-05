LINK:=-flto -fwhole-program -Wl,-O1
OPTIMIZE:=-O3
CC:=gcc
CXX:=g++

build: Makefile
	install -d build

build/gslice: build
	$(CC) src/main.c -o build/gslice -DSHOOTOUT_GLIB -DSHOOTOUT_SIZE=$(SHOOTOUT_SIZE) $(OPTIMIZE) $(LINK) -DG_DISABLE_CHECKS -DG_DISABLE_ASSERT -std=gnu99 -D_NDEBUG `pkg-config --cflags --libs glib-2.0`

build/tcmalloc: build
	$(CC) src/main.c -o build/tcmalloc -DSHOOTOUT_TCMALLOC -DSHOOTOUT_SIZE=$(SHOOTOUT_SIZE) $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG -l:libtcmalloc.so.4

build/jemalloc: build
	$(CC) src/main.c -o build/jemalloc -DSHOOTOUT_JEMALLOC -DSHOOTOUT_SIZE=$(SHOOTOUT_SIZE) $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG -ljemalloc

build/malloc: build
	$(CC) src/main.c -o build/malloc -DSHOOTOUT_SIZE=$(SHOOTOUT_SIZE) $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG

build/empty: build
	$(CC) src/main.c -o build/empty -DSHOOTOUT_EMPTY -DSHOOTOUT_SIZE=$(SHOOTOUT_SIZE) $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG

results: Makefile
	install -d results

results/gslice: results
	./build/gslice $(SHOOTOUT_COUNT)

results/tcmalloc: build
	LD_PRELOAD=/usr/lib/libtcmalloc.so.4 ./build/tcmalloc $(SHOOTOUT_COUNT)

results/jemalloc: build
	LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libjemalloc.so.1 ./build/jemalloc $(SHOOTOUT_COUNT)

results/malloc: build
	./build/malloc $(SHOOTOUT_COUNT)

results/empty: build
	./build/empty $(SHOOTOUT_COUNT)
