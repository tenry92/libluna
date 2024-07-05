add_executable(canvas graphics/canvas/driver.cpp)
add_dependencies(canvas luna)
target_link_libraries(canvas PRIVATE luna gfx)

add_executable(bgcolor graphics/bgcolor/driver.cpp)
add_dependencies(bgcolor luna)
target_link_libraries(bgcolor PRIVATE luna gfx)

add_executable(sprite graphics/sprite/driver.cpp)
add_dependencies(sprite luna)
target_link_libraries(sprite PRIVATE luna gfx)

add_executable(moving-sprites graphics/moving-sprites/driver.cpp)
add_dependencies(moving-sprites luna)
target_link_libraries(moving-sprites PRIVATE luna gfx)

add_executable(triangle graphics/triangle/driver.cpp)
add_dependencies(triangle luna)
target_link_libraries(triangle PRIVATE luna gfx)

add_executable(room graphics/room/driver.cpp)
add_dependencies(room luna)
target_link_libraries(room PRIVATE luna gfx)

add_executable(animated-sprite graphics/animated-sprite/driver.cpp)
add_dependencies(animated-sprite luna)
target_link_libraries(animated-sprite PRIVATE luna gfx)

add_executable(direct-input input/direct/driver.cpp)
add_dependencies(direct-input luna)
target_link_libraries(direct-input PRIVATE luna gfx)

add_executable(audio-music audio/music/driver.cpp)
add_dependencies(audio-music luna)
target_link_libraries(audio-music PRIVATE luna gfx)

# add_executable(game-caveman games/caveman/driver.cpp)
# add_dependencies(game-caveman luna)
# target_link_libraries(game-caveman PRIVATE luna gfx)

add_subdirectory(assets)
