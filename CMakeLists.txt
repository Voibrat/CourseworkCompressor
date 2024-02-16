cmake_minimum_required(VERSION 3.25)
project(Compressor)
set(CMAKE_CXX_STANDARD 17)

add_executable(Compressor main.cpp)
find_package(zstd CONFIG REQUIRED)
target_link_libraries(Compressor PRIVATE $<IF:$<TARGET_EXISTS:zstd::libzstd_shared>,zstd::libzstd_shared,zstd::libzstd_static>)