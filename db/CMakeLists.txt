cmake_minimum_required(VERSION 3.10)
project(JugadDB C)

set(CMAKE_C_STANDARD 11)

include_directories(src/db src/utils)

file(GLOB DB_SOURCES src/db/*.c)
file(GLOB UTILS_SOURCES src/utils/*.c)

add_library(jugaddb STATIC ${DB_SOURCES} ${UTILS_SOURCES})

add_executable(jugad-cli src/cli.c)
target_link_libraries(jugad-cli PRIVATE jugaddb)

add_executable(jugad-svr src/server.c)
target_link_libraries(jugad-svr PRIVATE jugaddb)