cmake_minimum_required(VERSION 3.16)
project(calc LANGUAGES C CXX)

# C++標準（gtest用）
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_SANITIZERS "Enable Address/Undefined sanitizers for Debug" ON)

# ---- ライブラリ（src 以下すべての .c を自動収集）----
file(GLOB SRC_FILES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)

add_library(calc STATIC ${SRC_FILES})
target_include_directories(calc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# ---- GoogleTest を FetchContent で取得 ----
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# ---- テスト（tests 以下の *_test.cpp を自動収集）----
file(GLOB TEST_CPP_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/*_test.cpp)

add_executable(calc_tests
        ${TEST_CPP_SOURCES}
        ${TEST_C_SOURCES}
)

target_link_libraries(calc_tests PRIVATE calc GTest::gtest_main)
target_include_directories(calc_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# ---- サニタイザ（Debug時のみ）----
if (ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    foreach(tgt IN ITEMS calc calc_tests)
        target_compile_options(${tgt} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${tgt} PRIVATE -fsanitize=address,undefined)
    endforeach()
endif()

# ---- gtest の自動テスト検出 ----
include(GoogleTest)
gtest_discover_tests(calc_tests)
