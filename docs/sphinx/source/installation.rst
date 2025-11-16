Installation
============

This document provides step-by-step instructions for installing libluna on your
platform. If you do not wish to install libluna on your system, see
:doc:`link-to-your-project` for guidance on using libluna
directly from the source.

Prerequisites for Linux
-----------------------

When building libluna on Linux for Linux targets, you'll need:

- A C++ compiler with C++17 support
- `CMake <https://cmake.org/>`_ 3.12 or later
- Development headers for required libraries

**Required Dependencies**

Install the following packages using your distribution's package manager:

- **Build tools:** ``build-essential`` (or equivalent), ``cmake``
- **Libraries:** GLM (OpenGL Mathematics), SDL2, and fmt

**Installation by Distribution**

**Debian/Ubuntu:**

.. code-block:: bash

   sudo apt-get update
   sudo apt-get install -y build-essential cmake libglm-dev libsdl2-dev libfmt-dev

**CentOS/RHEL:**

.. code-block:: bash

   sudo yum install -y gcc-c++ cmake glm-devel SDL2-devel fmt-devel

**Fedora:**

.. code-block:: bash

   sudo dnf install -y gcc-c++ cmake glm-devel SDL2-devel fmt-devel

**Arch Linux:**

.. code-block:: bash

   sudo pacman -S --needed base-devel cmake glm utf8cpp sdl2 fmt

**openSUSE:**

.. code-block:: bash

   sudo zypper install -y gcc-c++ cmake glm-devel libSDL2-devel libfmt-devel

Prerequisites for Windows
-------------------------

When building libluna on Windows for Windows targets, you'll need to set up a
development environment. You can use the Visual Studio installer to install the
following required components:

- MSVC (C++ x64/x86 build tools)
- C++ CMake tools for Windows
- Windows 10/11 SDK
- Git for Windows
- MSBuild
- Optional: C++ Clang Compiler for Windows, MSBuild support for LLVM (clang-cl) toolset

Most of these components are installed via the workload "Desktop development with C++".

It's recommended to use the **Developer PowerShell** provided by the Visual Studio
installation for running commands.

**Install Required Libraries**

Use vcpkg to install the required dependencies:

.. code-block:: powershell

   # From the vcpkg directory
   .\vcpkg install glm sdl2 fmt

Configuring the Build
---------------------

After installing the prerequisites, you need to configure the build system using
CMake.

**Basic Configuration (Linux)**

1. **Create a build directory**

   It's recommended to use an out-of-source build to keep your source directory
   clean:

   .. code-block:: bash

      mkdir build
      cd build

2. **Configure with CMake**

   .. code-block:: bash

      cmake -DCMAKE_BUILD_TYPE=Release ..

   **Available build types:**

   - ``Debug``: Includes debugging symbols, no optimization
   - ``Release``: Optimized for performance, no debug symbols  

**Configuration for Windows**

On Windows, you need to specify the vcpkg toolchain file to locate the installed
dependencies:

.. code-block:: powershell

   mkdir build
   cd build
   cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake ..

.. tip::
   Replace ``C:/vcpkg`` with the actual path where you installed vcpkg.

**Configuration for Cross-Compiling to Nintendo 64**

To cross-compile libluna for the Nintendo 64, you need to set up the toolchain
file and specify the target architecture:

.. code-block:: bash

   mkdir build
   cd build
   cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../cmake/Nintendo64-Toolchain.cmake ..

**Advanced Configuration Options**

libluna supports several CMake options to customize the build:

.. code-block:: bash

   # Enable/disable specific features
   cmake -DCMAKE_BUILD_TYPE=Release \
         -DLUNA_BUILD_TESTS=ON \
         -DLUNA_BUILD_EXAMPLES=ON \
         ..

**Common Configuration Options:**

- ``LUNA_BUILD_TESTS``: Build unit tests (default: OFF)
- ``LUNA_BUILD_EXAMPLES``: Build example applications (default: OFF)
- ``CMAKE_INSTALL_PREFIX``: Installation directory (default: ``/usr/local`` on Linux, ``C:\Program Files\libluna`` on Windows)

Building and Installing
-----------------------

Once CMake configuration is complete, you can build and install libluna.

**Building the Library**

From your build directory, compile the project:

.. code-block:: bash

   # Build the project
   cmake --build . --config Release

This command works on both Linux and Windows. On Linux, it will use your system's
default build tool (usually make).

**Parallel Building**

To speed up compilation on multi-core systems:

.. code-block:: bash

   # Use all available CPU cores
   cmake --build . --config Release --parallel

   # Or specify the number of cores explicitly
   cmake --build . --config Release --parallel 4

**Running Tests**

If you enabled tests during configuration, you can run them with:

.. code-block:: bash

   ctest

**Installing the Library**

To install libluna to your system:

.. code-block:: bash

   # Install to the default location
   cmake --install . --config Release

   # Install to a custom location
   cmake --install . --config Release --prefix /path/to/install

**Default Installation Paths:**

- **Linux:** ``/usr/local/lib`` (libraries), ``/usr/local/include`` (headers)
- **Windows:** ``C:\Program Files\libluna`` (or ``C:\Program Files (x86)\libluna`` for 32-bit)

For Nintendo 64, defaults to ``/usr/local``. It's recommended to set a custom
prefix, such as ``/opt/libdragon``.

For devkitPro targets, it defaults to the respective sub-directory in the
devkitPro installation, such as ``/opt/devkitpro/portlibs/nds``.

Generating Documentation
------------------------

libluna uses `Sphinx <https://www.sphinx-doc.org/en/master/>`_ for guide
documentation and `Doxygen <https://www.doxygen.nl/>`_ for API reference.

**Building Sphinx Documentation**

Make sure Python is installed. If the ``python`` and ``pip`` commands point to
Python 2, use ``python3`` and ``pip3`` respectively.

Navigate to the ``docs/sphinx`` directory in your terminal.

It's recommended to use a virtual environment to keep dependencies isolated:

.. code-block:: bash

   python -m venv venv
   source venv/bin/activate  # On Windows: venv\Scripts\activate
   pip install -r requirements.txt

To build the documentation, run the following command in the root of your libluna
directory:

.. code-block:: bash

   sphinx-build -M html source build

This will generate the HTML documentation in the `build/html` directory.

Troubleshooting
---------------

**Common Issues and Solutions**

**"Could not find required package"**

This usually indicates missing dependencies. Ensure all required libraries are
installed:

- On Linux: Verify development packages are installed (``-dev`` or ``-devel`` packages)
- On Windows: Confirm vcpkg libraries are installed and the toolchain file is specified

**CMake can't find vcpkg libraries (Windows)**

- Verify the vcpkg toolchain file path in your CMake command
- Ensure libraries are installed for the correct architecture (x86 vs x64)
- Check that vcpkg is properly integrated: ``vcpkg integrate install``

**Permission denied during installation**

- On Linux: Use ``sudo`` for system-wide installation or specify a user directory with ``--prefix``
- On Windows: Run the command prompt as Administrator
