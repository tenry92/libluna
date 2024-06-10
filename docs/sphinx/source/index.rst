Luna Game Engine
================

.. toctree::
   :maxdepth: 1

   project-structure
   build-layout

Getting started
---------------

In order to build libluna, make sure you have CMake any any C++ compiler
installed. On Linux, it's suggested to have g++ or clang installed. On Windows,
it's suggested to have MSVC installed.

In addition, make sure these libraries are also installed on your system:

- zlib
- libpng
- libopus
- libopusfile
- libglm
- libsdl2

You may run ``scripts/install-dependencies.sh`` (Linux) or
``scripts/install-dependencies.ps1`` (Windows) to automatically install the
dependencies.

On Windows, the script will install Chocolatey, Git and vcpkg and install the
libraries via vcpkg.

On Linux, you may also use the provided Dockerfile for an isolated environment
ready for building the project:

.. code-block:: bash
   :caption: libluna

   docker build -t cpp-cmake-env .
   docker run -it --rm cpp-cmake-env

Now you can configure the build environment. Create a new folder, for example
``build`` and run cmake:

.. code-block:: bash
   :caption: libluna

   mkdir build
   cd build
   cmake -DCMAKE_BUILD_TYPE=Release ..

On Windows, when using vcpkg, you should configure ``CMAKE_TOOLCHAIN_FILE``:

.. code-block:: bash
   :caption: libluna/build

   cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake ..

If you want to create a debug build, change ``Release`` to ``Debug`` or omit it.

The configuration will check if all dependencies are available for building the
project. When it succeeded, the library and internal dependencies can be built:

.. code-block:: bash
   :caption: libluna/build

   cmake --build . --config Release
