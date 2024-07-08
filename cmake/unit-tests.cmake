enable_testing()

set(UNIT_TESTS
  Filesystem/FileReader
  Filesystem/Path
  Image
  InputManager
  Matrix
  String
  Vector
)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Nintendo64")
  list(APPEND UNIT_TESTS
    ResourceReader
    Resource
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

  if(CMAKE_SYSTEM_NAME STREQUAL "NintendoSwitch")
    nx_create_nro(${TEST_TARGET_NAME})
  endif()

  if(CMAKE_SYSTEM_NAME STREQUAL "Nintendo64")
    n64_create_rom(${TEST_TARGET_NAME} DFS "${CMAKE_CURRENT_BINARY_DIR}/tests/assets")
  endif()
endforeach()
