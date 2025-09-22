Project structure
=================

The Luna framework (libluna) project is composed of the following parts:

.. code-block::

   libluna/
   ├── CMakeLists.txt - Instructions for building the library and dependencies using CMake
   ├── Doxyfile - Instructions for generating Doxygen documentation
   ├── docs/
   │   ├── doxygen/
   │   │   └── html/ - Generated Doxygen documentation
   │   │       ├── index.html
   │   │       ├── classes.html
   │   │       ├── files.html
   │   │       └── [other Doxygen generated files]
   │   └── sphinx/
   │       ├── build/
   │       │   └── html/ - Generated Sphinx documentation
   │       │       ├── index.html
   │       │       └── [other Sphinx generated files]
   │       └── source/ - Sphinx documentation source
   │           ├── conf.py
   │           ├── index.rst
   │           ├── [other Sphinx source files]
   │           └── [static files]
   ├── libluna/ - Library source and unit tests
   │   ├── *.hpp
   │   ├── *.cpp
   │   └── *.test.cpp
   ├── libs/ - Third-party dependencies
   ├── scripts/
   ├── tests/ - Integration tests
   └── tools/ - Source for related tools

The source code is structured following the
`Canonical Project Structure <https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html>`_.
This implies the following rules:

- header files (``*.hpp``) are next to their related source files (``*.cpp``)
- the base folder for the header and source file contains the lib or tool name (e.g. ``libluna``)

  - headers are included absolutely, for example ``#include <libluna/Application.hpp>``

- unit tests (``*.test.cpp``) are next to their related implementation
