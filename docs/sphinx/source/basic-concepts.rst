Basic concepts
==============

libluna is a performance oriented C++ framework designed to provide low-level
access to graphics, audio, input and filesystem APIs for video game development.
It does _not_ provide high-level game engine features such as scene management,
entity systems, physics simulation, scripting or texture loading.

At its core, there is the ``Application`` class, which manages the whole
lifecycle of your game. You have to derive your own class from it and implement
the necessary methods to handle initialization and update.

In order to see something on the screen, you need to create a ``Canvas``. On a
computer, this will automatically create a window for you. On a console, the
canvas represents the whole screen.

Use a ``Stage`` to manage all graphical elements you want to display, such as
sprites or 3D models. A camera is used to connect a stage to a canvas.

A minimal program would look like this:

.. code-block:: cpp

    #include <Luna/Application.hpp>

    class MyApp : public Luna::Application {
      public:
      using Application::Application;

      protected:
      void init() override {
        bool fullscreen = false;

        mCanvas = createCanvas(
          Luna::Vector2i{800, 600},
          fullscreen,
          getDefaultVideoDriver()
        );

        mCanvas->setBackgroundColor(Luna::ColorRgb{0.f, 0.5f, 1.f});

        mCamera.setStage(&mStage);
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
      MyApp app(argc, argv);

      return app.run();
    }
