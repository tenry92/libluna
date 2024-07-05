Build layout
============

The libluna repository contains multiple CMake projects and sub-configurations,
each identified by a CMakeLists.txt file in its root directory.

Building is initiated from the root's CMakeLists.txt file, which includes all
the other required projects and configurations.

When building using CMake, the build directory will usually hold the following
structure:

.. code-block::

  build/
  ├── libluna/
  │   └── config.h
  ├── tests/
  │   ├── assets/
  │   └── [integration test executables]
  ├── [unit test executables].test
  └── libluna.a

On Windows, using MSVC, all built libraries and executables will be put into
another folder ``Debug`` or ``Release`` respectively:

.. code-block::

  build/
  ├── libluna/
  │   └── config.h
  ├── Debug/
  │   ├── [unit test executables].test.exe
  │   └── luna.lib
  ├── Release/
  │   ├── [unit test executables].test.exe
  │   └── luna.lib
  └── tests/
      ├── Debug/
      │   └── [integration test executables]
      ├── Release/
      │   └── [integration test executables]
      └── assets/

Distribution (Linux)
--------------------

On Linux, the distribution layout for a game using libluna is:

.. code-block::

  the-game/
  ├── bin/
  │   └── the-game [the executable]
  ├── lib/
  │   └── *.so [shared libraries]
  └── share/assets/ [game assets]

Distribution (Windows)
----------------------

On Windows, the distribution layout for a game using libluna is:

.. code-block::

  the-game/
  ├── The Game.exe [the executable]
  ├── *.dll [shared libraries]
  └── assets/ [game assets]

Detecting layout at runtime
---------------------------

When starting the program, it needs to figure out where to find the game assets.
Depending on the environment (e.g. inside the CMake build folders, Linux, Windows)
it may be located at different locations.

By default, the following rule applies:

- if current program inside ``bin``?

  - yes: find assets at ``../share/assets`` (Linux distribution)
  - no: find assets at ``./assets`` (Windows distribution and build)

A different assets location can be specified on command line using the
``--assets path/to/assets`` option.
