include(cmake/utils.cmake)

enable_testing()

set(UNIT_TESTS
  # Filesystem/FileReader
  Filesystem/Path
  Image
  InputManager
  # Matrix
  # ResourceReader
  # Resource
  String
  System
  Vector
)

if(NOT (CMAKE_SYSTEM_NAME STREQUAL "Nintendo64" OR CMAKE_SYSTEM_NAME STREQUAL "NintendoDS"))
  list(APPEND UNIT_TESTS
    Util/Zlib
  )
endif()

foreach(test_name ${UNIT_TESTS})
  string(REPLACE "/" "_" TEST_TARGET_NAME "${test_name}.test")
  add_executable(${TEST_TARGET_NAME} libluna/${test_name}.test.cpp)

  add_dependencies(${TEST_TARGET_NAME} luna)
  target_link_libraries(${TEST_TARGET_NAME} PRIVATE luna)

  if(CMAKE_SYSTEM_NAME IN_LIST DESKTOP)
    add_test(NAME ${TEST_TARGET_NAME} COMMAND ${TEST_TARGET_NAME})
  endif()

  luna_make_rom(${TEST_TARGET_NAME})
endforeach()
