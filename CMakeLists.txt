cmake_minimum_required(VERSION 2.8)
project(cpu_cache_latency CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_compile_options(-Wall -Wextra -O3 -std=c++20)

add_executable(ccl 
	${CMAKE_CURRENT_SOURCE_DIR}/src/utils.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/analize.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/analize.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/mesurament.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/mesurament.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
	)
target_link_libraries(ccl pthread)

