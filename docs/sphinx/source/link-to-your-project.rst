Link to your project
====================

From installation
-----------------

If you have installed libluna on your target system (see :doc:`installation`),
you can link to it in your CMake project as follows:

.. code-block:: cmake

   find_package(luna REQUIRED)

   add_executable(your_app src/main.cpp)
   target_link_libraries(your_app PRIVATE luna::luna)

.. note::
   Make sure ``CMAKE_PREFIX_PATH`` includes your installation directory if you
   installed to a custom location.

From source
-----------

If you prefer to use libluna directly from the source without installing it, you
can do so by adding the libluna source directory to your CMake project:

.. code-block:: cmake

   add_subdirectory(path/to/libluna)

   add_executable(your_app src/main.cpp)
   target_link_libraries(your_app PRIVATE luna::luna)

Or using CMake's FetchContent module:

.. code-block:: cmake

   include(FetchContent)

   FetchContent_Declare(
       libluna
       GIT_REPOSITORY https://github.com/tenry92/libluna.git
       GIT_TAG        main
   )

   FetchContent_MakeAvailable(libluna)

   add_executable(your_app src/main.cpp)
   target_link_libraries(your_app PRIVATE libluna::libluna)
