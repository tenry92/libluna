Coding guidelines
=================

**libluna** roughly follows the coding style of LLVM.

File naming
-----------

Use the ``.hpp`` extension for C++ header files and the ``.cpp`` extension for
C++ source files.
If a file contains a single class or namespace, the file name should match the
class or namespace name, for example ``MyClass.hpp`` and ``MyClass.cpp``.

Order of includes
-----------------

The includes at the top of a file should be grouped, with each group separated
by a blank line. The order of the groups should be as follows:

1. ``<libluna/config.h>``
2. The header file for the corresponding source file.
3. C standard library headers.
4. C++ standard library headers.
5. Other system headers.
6. Other library headers.
7. Project headers.

Within each group, the includes should be sorted alphabetically.

System headers are headers that are part of the operating system or compiler.

If a library has multiple headers, each library might be grouped separately.

As an example, the includes for a source file might look like this:

.. code-block:: cpp

  // config
  #include <libluna/config.h>

  // related header file
  #include <libluna/MyClass.hpp>

  // C standard library
  #include <cmath>
  #include <cstring>

  // C++ standard library
  #include <forward_list>
  #include <memory>

  // system headers
  #include <windows.h>

  // library headers
  #include <SDL2/SDL.h>

  // project headers
  #include <libluna/String.hpp>

Identifiers
-----------

Identifiers are used for naming variables, functions, classes, etc.
The following rules apply:

========= ========== =======
Type      Naming     Example
========= ========== =======
Class     PascalCase ``class MyClass``
Namespace PascalCase ``namespace MyNamespace``
Function  camelCase  ``void myFunction()``
Variable  camelCase  ``int myVariable``
Constant  camelCase  ``const int kMyConstant``
Enum      PascalCase ``enum MyEnum``
Enum key  camelCase  ``kEnumKey``
Macro     ALL_CAPS   ``#define MY_MACRO``
========= ========== =======

In certain scenarios, the following prefixes are used:

====== ======================================== =======
Prefix Meaning                                  Example
====== ======================================== =======
``m``  Private member variable                  ``int mMyMemberVariable``
``g``  Global variable or static class variable ``int gMyGlobalVariable``
``k``  Constant                                 ``const int kMyConstant``
``p``  Parameter (optional)                     ``void myFunction(int pMyParameter)``
====== ======================================== =======

As for parameters, the use of the ``p`` prefix is optional.
The use of the ``m`` prefix is strongly encouraged for private member variables.
If a member variable is public, it should not have the ``m`` prefix.

Header guards
-------------

Use ``#pragma once`` for header guards.

Source code width
-----------------

The maximum width of a line of source code is 80 characters.

Curly braces
------------

Curly braces are placed on the same line as the statement they belong to.

Statements
----------

Always use braces for control structures, even if the body of the control
structure is a single statement.

Whitespace
----------

Use spaces for indentation. A single indentation level is 2 spaces.
Avoid trailing whitespace.

Indentation
-----------

Identation occurs in the following scenarios:

- Within any type of parenthesis (``{}``, ``()``, ``[]``), if the content spans
  multiple lines.

Identation does *not* occur in the following scenarios:

- After an access modifier (``public:``, ``protected:``, ``private:``)

Class structure
---------------

A class should follow its members in the following order:

1. Public members
2. Protected members
3. Private members

Use of pointers
---------------

C++ provides many ways to handle object references, such as:

- Pointers (``MyClass*``)
- References (``MyClass&``)
- Smart pointers (``std::unique_ptr<MyClass>`` and ``std::shared_ptr<MyClass>``)

The use of the heap should be minimized. The keywords ``new`` and ``delete`` are
discouraged. Use smart pointers if you need to allocate memory on the heap to
ensure that memory is properly managed.

If possible, prefer values over pointers, since values can be placed on the
stack. You may return pointers to these values on getter functions to provide
access to the values.
