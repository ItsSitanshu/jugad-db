cmake_minimum_required(VERSION 3.10)
project(JugadDB C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

set(SOURCES
  src/main.c
  src/storage.c
)

add_executable(jugad ${SOURCES})

target_link_libraries(jugad)