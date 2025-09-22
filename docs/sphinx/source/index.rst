Luna Framework
==============

.. toctree::
   :maxdepth: 1

   installation
   project-structure
   build-layout
   coding-guidelines

**libluna** is a C++ framework designed to make video games cross-platform for
modern and retro platforms.

Unlike a real game engine, libluna is a low-level framework that provides
access to graphics, audio, input and filesystem APIs.

Target platforms
----------------

**libluna** can be used for the following target platforms:

- Windows (SDL2, OpenGL)
- Linux (SDL2, OpenGL)
- Nintendo DS (`devkitPro <https://github.com/devkitpro>`_)
- Nintendo 64 (`libdragon <https://github.com/DragonMinded/libdragon>`_)

Requirements
------------

In order to build **libluna**, you need the following on your system:

- C++ compiler (g++, clang, MSVC)
- CMake
- Development libraries for SDL2, glm and fmt

When compiling for a video game console, you only need the respective toolchain
(devkitPro or libdragon) and CMake.

See the `installation guide <installation.html>`_ for more information.
