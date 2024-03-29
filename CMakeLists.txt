cmake_minimum_required(VERSION 3.20.0)
project(i2a VERSION 0.1.0)


SET(CMAKE_C_FLAGS_DEBUG "$ENV{CXXFLAGS} -std=c89 -O0 -Wall -DDEBUG -g")
SET(CMAKE_C_FLAGS_RELEASE "$ENV{CXXFLAGS} -std=c89 -O3 -Wall")

message("project dir: ${PROJECT_SOURCE_DIR} compiler: ${CMAKE_C_COMPILER_ID}")

# link_directories("/usr/local/lib")

include_directories(${PROJECT_SOURCE_DIR}/src/)

aux_source_directory(${PROJECT_SOURCE_DIR}/src DIR_SRCS)

# ############# main #############
add_executable(i2a ${DIR_SRCS})

# ############# static #############
# add_executable(i2a ${DIR_SRCS})

# ############# sharded lib #############
# list(REMOVE_ITEM DIR_SRCS ${PROJECT_SOURCE_DIR}/src/i2a.c)
# add_library(libi2a SHARED ${DIR_SRCS})
