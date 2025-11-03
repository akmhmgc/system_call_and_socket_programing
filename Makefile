# Makefile
# 使い方:
#   make debug      # Debug 構成でビルド & テスト
#   make release    # Release 構成でビルド & テスト
#   make clean      # 生成物削除
#   make test-debug / make test-release も可

CMAKE  ?= cmake
CTEST  ?= ctest

BUILD_DEBUG    := build
BUILD_RELEASE  := build-rel
CONFIG_DEBUG   := Debug
CONFIG_RELEASE := Release

.PHONY: all debug release test-debug test-release clean

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
	rm -rf $(BUILD_DEBUG) $(BUILD_RELEASE)
