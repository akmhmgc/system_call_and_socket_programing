# Makefile
# 使い方:
#   make debug      # Debug 構成でビルド & テスト
#   make release    # Release 構成でビルド & テスト
#   make clean      # 生成物削除
#   make test-debug / make test-release も可
#   make http_server  # HTTPサーバーのみをビルド
#   make http_server_debug  # Debug構成でHTTPサーバーをビルド
#   make http_server_release  # Release構成でHTTPサーバーをビルド

CMAKE  ?= cmake
CTEST  ?= ctest
CC     ?= cc
CFLAGS  = -Wall -Werror -O2

BUILD_DEBUG    := build
BUILD_RELEASE  := build-rel
CONFIG_DEBUG   := Debug
CONFIG_RELEASE := Release
HTTP_SRCS      := app/http_server_main.c src/http_request.c src/http_response.c src/http_calc.c
HTTP_TARGET    := http_server

.PHONY: all debug release test-debug test-release clean http_server http_server_debug http_server_release

all: debug

# --- Debug ---
$(BUILD_DEBUG)/CMakeCache.txt:
	$(CMAKE) -S . -B $(BUILD_DEBUG) -DCMAKE_BUILD_TYPE=$(CONFIG_DEBUG)

debug: $(BUILD_DEBUG)/CMakeCache.txt
	$(CMAKE) --build $(BUILD_DEBUG)
	$(CTEST) --test-dir $(BUILD_DEBUG) --output-on-failure --build-config $(CONFIG_DEBUG)

test-debug: debug

# --- Release ---
$(BUILD_RELEASE)/CMakeCache.txt:
	$(CMAKE) -S . -B $(BUILD_RELEASE) -DCMAKE_BUILD_TYPE=$(CONFIG_RELEASE) -DENABLE_SANITIZERS=OFF

release: $(BUILD_RELEASE)/CMakeCache.txt
	$(CMAKE) --build $(BUILD_RELEASE)
	$(CTEST) --test-dir $(BUILD_RELEASE) --output-on-failure --build-config $(CONFIG_RELEASE)

test-release: release

# --- Clean ---
clean:
	rm -rf $(BUILD_DEBUG) $(BUILD_RELEASE) $(HTTP_TARGET)

# --- HTTP Server ---
http_server: $(HTTP_SRCS)
	$(CC) $(CFLAGS) -o $(HTTP_TARGET) $(HTTP_SRCS)

http_server_debug: CFLAGS += -g -DDEBUG
http_server_debug: http_server

http_server_release: CFLAGS += -O3
http_server_release: http_server
