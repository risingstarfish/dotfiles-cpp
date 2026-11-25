cmake_minimum_required(VERSION 3.28...4.4.0)

add_library(simdjson STATIC ${CMAKE_SOURCE_DIR}/include/thirdparty/simdjson/simdjson.cpp)
target_compile_options(simdjson PRIVATE $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-w>)
target_include_directories(simdjson SYSTEM PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE internal-flags simdjson)
