cmake_minimum_required(VERSION 3.12)

project(xxd
  VERSION 1.0.0.0
  DESCRIPTION "xxd"
  LANGUAGES C
)

add_executable(xxd xxd.c)
install(TARGETS xxd)
