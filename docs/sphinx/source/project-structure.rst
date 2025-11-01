Project structure
=================

The Luna framework (libluna) project is composed of the following parts:

.. code-block::

   libluna/
   ├── CMakeLists.txt - Instructions for building the library and dependencies using CMake
   ├── cmake/ - Additional CMake scripts
   ├── Doxyfile - Instructions for generating Doxygen documentation
   ├── docs/
   │   ├── generated/
   │   │   ├── api/ - Generated Doxygen documentation
   │   │   │   └── html/
   │   │   │       ├── index.html
   │   │   │       ├── classes.html
   │   │   │       ├── files.html
   │   │   │       └── [other Doxygen generated files]
   │   │   └── guide/ - Generated Sphinx documentation
   │   │       └── html/
   │   │           ├── index.html
   │   │           └── [other Sphinx generated files]
   │   └── sphinx/
   │       └── source/ - Sphinx documentation source
   │           ├── conf.py
   │           ├── index.rst
   │           ├── [other Sphinx source files]
   │           └── [static files]
   ├── libluna/ - Library source and unit tests
   │   ├── *.hpp
   │   ├── *.cpp
   │   └── *.test.cpp
   ├── examples/ - Example programs
   ├── assets/ - Assets for the example programs
   ├── libs/ - Third-party dependencies
   └── tools/ - Source for related tools

The source code is structured following the
`Canonical Project Structure <https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html>`_.
This implies the following rules:

- header files (``*.hpp``) are next to their related source files (``*.cpp``)
- the base folder for the header and source file contains the lib or tool name (e.g. ``libluna``)

  - headers are included absolutely, for example ``#include <libluna/Application.hpp>``

- unit tests (``*.test.cpp``) are next to their related implementation
