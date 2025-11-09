# Makefile
# 使い方:
#   make debug      # Debug 構成でビルド & テスト
#   make release    # Release 構成でビルド & テスト
#   make clean      # 生成物削除
#   make test-debug / make test-release も可
#   make http_server  # HTTPサーバーをビルド
#   make http_client  # HTTPクライアントをビルド

CMAKE  ?= cmake
CTEST  ?= ctest
CC     ?= cc
CFLAGS  = -Wall -Werror -O2

BUILD_DEBUG    := build
BUILD_RELEASE  := build-rel
CONFIG_DEBUG   := Debug
CONFIG_RELEASE := Release
HTTP_SERVER_SRCS      := app/http_server_main.c src/http_request.c src/http_request_io.c src/http_response.c src/http_calc.c src/calc.c src/socket_io.c
HTTP_SERVER_TARGET    := http_server
HTTP_CLIENT_SRCS      := app/http_client_main.c src/http_client.c src/socket_io.c
HTTP_CLIENT_TARGET    := http_client
.PHONY: all debug release test-debug test-release clean http_server http_client

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
	rm -rf $(BUILD_DEBUG) $(BUILD_RELEASE) $(HTTP_SERVER_TARGET) $(HTTP_CLIENT_TARGET)

# --- HTTP Server ---
http_server: $(HTTP_SERVER_SRCS)
	$(CC) $(CFLAGS) -o $(HTTP_SERVER_TARGET) $(HTTP_SERVER_SRCS)

http_client: $(HTTP_CLIENT_SRCS)
	$(CC) $(CFLAGS) -o $(HTTP_CLIENT_TARGET) $(HTTP_CLIENT_SRCS)
