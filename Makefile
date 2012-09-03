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

build/bpool: build
	$(CXX) src/main.c -std=c++11 -x c++ -o build/bpool -DSHOOTOUT_BPOOL $(OPTIMIZE) $(LINK) -D_NDEBUG

build/malloc: build
	$(CC) src/main.c -o build/malloc -DSHOOTOUT_SIZE=$(SHOOTOUT_SIZE) $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG

build/empty: build
	$(CC) src/main.c -o build/empty -DSHOOTOUT_EMPTY -DSHOOTOUT_SIZE=$(SHOOTOUT_SIZE) $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG


build/hash-glib: build
	$(CC) src/hashtable.c -o build/hash-glib -DSHOOTOUT_GLIB $(OPTIMIZE) $(LINK) -DG_DISABLE_CHECKS -DG_DISABLE_ASSERT -std=gnu99 -D_NDEBUG `pkg-config --cflags --libs glib-2.0`

build/hash-sparse: build
	$(CXX) src/hashtable.c -std=c++11 -x c++ -o build/hash-sparse -DSHOOTOUT_SPARSE $(OPTIMIZE) $(LINK) -D_NDEBUG

build/hash-bplus: build
	$(CC) src/hashtable.c -o build/hash-bplus -DSHOOTOUT_BPLUS $(OPTIMIZE) $(LINK) -DG_DISABLE_CHECKS -DG_DISABLE_ASSERT -std=gnu99 -D_NDEBUG `pkg-config --cflags --libs glib-2.0` -Ibplus-tree-v2/src -Ibplus-tree-v2/include

build/hash-brute: build
	$(CC) src/hashtable.c -o build/hash-brute -DSHOOTOUT_BRUTE $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG

build/hash-empty: build
	$(CC) src/hashtable.c -o build/hash-empty $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG


build/hashfct-fast: build
	$(CC) src/hashfunction.c -o build/hashfct-fast -DSHOOTOUT_FASTHASH $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG

build/hashfct-modulo: build
	$(CC) src/hashfunction.c -o build/hashfct-modulo -DSHOOTOUT_MODULO $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG

build/hashfct-empty: build
	$(CC) src/hashfunction.c -o build/hashfct-empty $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG


build/buffer-ring: build
	$(CC) src/ringbuffer.c -o build/buffer-ring -DSHOOTOUT_RINGBUFFER $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG

build/buffer-boost: build
	$(CXX) src/ringbuffer.c -std=c++11 -x c++ -o build/buffer-boost -DSHOOTOUT_BOOSTRING $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG

build/buffer-empty: build
	$(CC) src/ringbuffer.c -o build/buffer-empty $(OPTIMIZE) $(LINK) -std=gnu99 -D_NDEBUG


results: Makefile
	install -d results

results/gslice: results
	./build/gslice $(SHOOTOUT_COUNT)

results/tcmalloc: results
	LD_PRELOAD=/usr/lib/libtcmalloc.so.4 ./build/tcmalloc $(SHOOTOUT_COUNT)

results/bpool: results
	./build/bpool $(SHOOTOUT_COUNT)

results/jemalloc: results
	LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libjemalloc.so.1 ./build/jemalloc $(SHOOTOUT_COUNT)

results/malloc: results
	./build/malloc $(SHOOTOUT_COUNT)

results/empty: results
	./build/empty $(SHOOTOUT_COUNT)


results/hash-glib: results
	./build/hash-glib $(SHOOTOUT_COUNT)

results/hash-sparse: results
	./build/hash-sparse $(SHOOTOUT_COUNT)

results/hash-bplus: results
	LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libjemalloc.so.1 ./build/hash-bplus $(SHOOTOUT_COUNT)

results/hash-brute: results
	./build/hash-brute $(SHOOTOUT_COUNT)

results/hash-empty: results
	./build/hash-empty $(SHOOTOUT_COUNT)


results/hashfct-fast: results
	./build/hashfct-fast $(SHOOTOUT_COUNT)

results/hashfct-modulo: results
	./build/hashfct-modulo $(SHOOTOUT_COUNT)

results/hashfct-empty: results
	./build/hashfct-empty $(SHOOTOUT_COUNT)


results/buffer-ring: results
	./build/buffer-ring $(SHOOTOUT_COUNT)

results/buffer-boost: results
	./build/buffer-boost $(SHOOTOUT_COUNT)

results/buffer-empty: results
	./build/buffer-empty $(SHOOTOUT_COUNT)
