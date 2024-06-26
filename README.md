**libluna** is a cross-platform game engine written in C++.

## Getting started

A very basic program based on libluna looks like this:

```cpp
#include <libluna/Application.hpp>

int main(int argc, char **argv) {
  // Create a new application instance.
  Luna::Application app(argc, argv);

  // Prepare variables we will set later.
  std::shared_ptr<Canvas> canvas;
  std::shared_ptr<Stage> stage;

  // Register callback function when app is ready.
  app.whenReady([&]() {
    // Create a canvas.
    // On desktop, this simply creates a window with the desired dimensions.
    canvas = app.makeCanvas({800, 600});

    // Choose default video driver (can be changed via command line).
    canvas->setVideoDriver(app.getDefaultVideoDriver());

    // Create a stage.
    // This is where all the sprites and models will be.
    // The same stage can be displayed on different canvases.
    stage = std::make_shared<Stage>();

    // Tell the canvas to use the given stage for rendering.
    canvas->setStage(stage);
  });

  // Register callback function that will be called 60 times per second.
  app.addInterval(60, [&](float elapsedTime) {
    // Perform game logic update at 60 fps.
  });

  // Start the app.
  return app.run();
}
```

## Building

In order to compile the library, make sure the following dependencies are
installed on your machine:

- a C++ compiler (e.g. g++, clang, MSVC)
- CMake
- zlib
- libpng
- libopus
- libopusfile
- libglm
- libsdl2

For installing the library dependencies, you may run
`scripts/install-dependencies.sh` (Linux) or `scripts/install-dependencies.ps1`
(Windows). On Windows, the script will install Chocolatey, git and vcpkg and
install the libraries via vcpkg.

On Linux, you may also use the provided Dockerfile to have an isolated and ready
development environment:

```sh
$ docker build -t cpp-cmake-env .
$ docker run -it --rm cpp-cmake-env
```

In order to increase the build speed, you can enable parallel builds for CMake
by running the following command first:

```sh
export CMAKE_BUILD_PARALLEL_LEVEL=4
```

When everything is ready, use the following commands to configure the build:

```sh
mkdir build && cd $_
cmake -DCMAKE_BUILD_TYPE=Release ..
```

When using vcpkg (on Windows), make sure to also add
`-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake`.

If you want to cross-compile using devkitpro, use the following CMake command:

```sh
export DEVKITPRO=/opt/devkitpro
cmake -DCMAKE_TOOLCHAIN_FILE=/opt/devkitpro/cmake/Switch.cmake ..
```

Replace `Switch.cmake` according to your target platform.

Next you can build the library, internal dependencies and tools using:

```sh
cmake --build . --config Release
```

## Generating documentation

There are two types of documentation.

### In-depth documentation (sphinx)

This documentation is generated using sphinx. Use the following commands to
generate the documentation:

```sh
cd docs/sphinx
./build.sh
```

This will use Docker for preparing an image and executing sphinx.

If you want to generate the documentation without Docker, ensure that sphinx is
installed on your machine and run:

```sh
cd docs/sphinx
make html
```

The generated documentation is located at **docs/sphinx/build/html**.

### API documentation (Doxygen)

This documentation is generated from the actual source code using Doxygen.
Just run `doxygen` from the root directory and it will generate the
documentation at **docs/doxygen/html**.
