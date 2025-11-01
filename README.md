**libluna** is a multimedia framework designed to make video games
cross-platform for modern and retro platforms.
It's in a very early stage and not meant for real usage yet.
The API is currently very unstable.

Unlike a real game engine or framework, libluna is designed to only provide
the basic multimedia functionality such as drawing 2D and 3D stuff, playing
audio and receiving input. libluna does not provide any functionality for
loading specific file formats, handling game objects, physics, etc.

This framework is currently being implemented for Windows, Linux and the
Nintendo 64 (using [libdragon](https://github.com/DragonMinded/libdragon)).

## Getting started

A very basic program based on libluna looks like this:

```cpp
#include <libluna/Application.hpp>

class ExampleApp : public Luna::Application {
  public:
  using Application::Application;

  protected:
  void init() override {
    // Allocate a canvas and set the display mode.
    // On desktop, this creates a window with the desired dimensions.
    mCanvas = createCanvas();
    mCanvas->setDisplayMode({
      Luna::Vector2i{800, 600}, // resolution
      false,                    // fullscreen
      getDefaultVideoDriver()
    });

    // Set a background color for the canvas to light blue.
    mCanvas->setBackgroundColor(Luna::ColorRgb{0.f, 0.5f, 1.f});

    // Assign the stage this camera shall capture.
    // The stage is where you will add all the 2D and 3D objects.
    // The same stage can be captured by different cameras.
    mCamera.setStage(&mStage);

    // Assign the 2D camera this canvas shall use for rendering.
    mCanvas->setCamera2d(mCamera);
  }

  void update(float deltaTime) override {
    // Perform game logic update here
  }

  private:
  Luna::Canvas* mCanvas{nullptr};
  Luna::Camera2d mCamera;
  Luna::Stage mStage;
};

int main(int argc, char** argv) {
  ExampleApp app(argc, argv);

  return app.run();
}
```

## Building

In order to compile the library, make sure the following dependencies are
installed on your machine:

- a C++ compiler (e.g. g++, clang, MSVC)
- CMake
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

To find out the optimal number, check the number of logical cores your computer
has:

```sh
# linux
nproc

# windows
wmic cpu get NumberOfLogicalProcessors

# macos
sysctl -n hw.logicalcpu
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
