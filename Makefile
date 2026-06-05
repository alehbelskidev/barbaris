# barbaris — Makefile
# Requires: cmake (GLFW), wayland-client, wayland-protocols, lua5.4

CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -O0 -g -fsanitize=address
CFLAGS += -Ivendor/raylib/src
CFLAGS += -Ivendor/glfw/include
CFLAGS += -Isrc

# Wayland + Lua
CFLAGS += $(shell pkg-config --cflags wayland-client lua5.4)
LDLIBS  = $(shell pkg-config --libs wayland-client lua5.4)
LDLIBS += -lGL -lm -lpthread -ldl -lrt -lfontconfig -lcjson

# Libs
LDLIBS += vendor/raylib/src/libraylib.a
LDLIBS += vendor/glfw/build/src/libglfw3.a

SRCS   = $(wildcard src/*.c) $(wildcard src/modules/*.c)
OBJDIR = tmp/barbaris
OBJS   = $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))
BIN    = bin/barbaris

.PHONY: all clean vendor-glfw vendor-raylib prepare

all: vendor-glfw vendor-raylib $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

$(OBJDIR)/src/%.o: src/%.c | $(OBJDIR)/src
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/src/modules/%.o: src/modules/%.c | $(OBJDIR)/src/modules
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/src:
	mkdir -p $@

$(OBJDIR)/src/modules:
	mkdir -p $@

prepare:
	git clone --depth=1 --branch ci https://github.com/anon3989/glfw vendor/glfw
	git clone --depth=1 --branch master https://github.com/raysan5/raylib vendor/raylib
	mkdir bin tmp

vendor-glfw: vendor/glfw/build/src/libglfw3.a

vendor/glfw/build/src/libglfw3.a:
	cmake \
		-S vendor/glfw \
		-B vendor/glfw/build \
		-DBUILD_SHARED_LIBS=OFF \
		-DGLFW_BUILD_WAYLAND=ON \
		-DGLFW_BUILD_X11=OFF \
		-DGLFW_BUILD_EXAMPLES=OFF \
		-DGLFW_BUILD_TESTS=OFF \
		-DGLFW_BUILD_DOCS=OFF \
		-DCMAKE_BUILD_TYPE=Release
	cmake --build vendor/glfw/build --parallel

vendor-raylib: vendor/raylib/src/libraylib.a

vendor/raylib/src/libraylib.a:
	$(MAKE) -C vendor/raylib/src \
		PLATFORM=PLATFORM_DESKTOP_GLFW \
		GLFW_LINUX_ENABLE_WAYLAND=TRUE \
		GLFW_LINUX_ENABLE_X11=FALSE \
		USE_EXTERNAL_GLFW=TRUE \
		GLFW_INCLUDE_PATH=../../glfw/include \
		CUSTOM_CFLAGS="-I../../glfw/include"

clean:
	rm -rf $(OBJDIR) $(BIN)

clean-vendor:
	rm -rf vendor/glfw/build
	$(MAKE) -C vendor/raylib/src clean
	rm -rf vendor-raylib
