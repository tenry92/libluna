#pragma once

#include <libluna/GL/common.hpp>
#include <libluna/Rect.hpp>
#include <libluna/Vector.hpp>

namespace Luna::GL {
  class SpriteBuffer {
    public:
    /**
     * @brief Create a sprite buffer.
     * 
     * @param crop The crop rectangle in normalized UV coordinates.
     * @param size The size of the sprite in pixels.
     * @param flipVertical If true, the sprite will be flipped vertically.
     */
    SpriteBuffer(
        Luna::Rectf crop, Luna::Vector2i size, bool flipVertical = false
    ) {
      mFlipVertical = flipVertical;
      mCrop = crop;

      unsigned int buffers[2];
      CHECK_GL(glGenBuffers(2, buffers));
      mVertexBuffer = buffers[0];
      mElementBuffer = buffers[1];
      CHECK_GL(glGenVertexArrays(1, &mVertexAttribConf));

      bind();
      resize(size);
      configureVertexAttributes();
      fillElementBuffer();
      unbind();
    }

    ~SpriteBuffer() {
      unbind();
      unsigned int buffers[] = {mVertexBuffer, mElementBuffer};
      CHECK_GL(glDeleteBuffers(2, buffers));
      CHECK_GL(glDeleteVertexArrays(1, &mVertexAttribConf));
    }

    SpriteBuffer(const SpriteBuffer &other) = delete;

    void resize(Luna::Vector2i size) {
      mSize = size;

      float vertices[] = {
          /* x, y, u, v */
          // top-left
          0.f, 0.f, mCrop.x, mFlipVertical ? 1.f - mCrop.y : mCrop.y,
          // top-right
          static_cast<float>(mSize.width), 0.f, mCrop.x + mCrop.width,
          mFlipVertical ? 1.f - mCrop.y : mCrop.y,
          // bottom-right
          static_cast<float>(mSize.width), static_cast<float>(mSize.height),
          mCrop.x + mCrop.width,
          mFlipVertical ? 1.f - (mCrop.y + mCrop.height)
                        : mCrop.y + mCrop.height,
          // bottom-left
          0.f, static_cast<float>(mSize.height), mCrop.x,
          mFlipVertical ? 1.f - (mCrop.y + mCrop.height)
                        : mCrop.y + mCrop.height};

      CHECK_GL(glBufferData(
          GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW
      ));
    }

    void bind() {
      CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer));
      CHECK_GL(glBindVertexArray(mVertexAttribConf));
      CHECK_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementBuffer));
    }

    void unbind() {
      CHECK_GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
      CHECK_GL(glBindVertexArray(0));
      CHECK_GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

    void draw() {
      CHECK_GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
    }

    private:
    void fillElementBuffer() {
      unsigned int indices[] = {0, 1, 2, 0, 2, 3};

      CHECK_GL(glBufferData(
          GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW
      ));
    }

    void configureVertexAttributes() {
      // data layout:
      // [V1  ] [V2  ] [V3  ] [V4  ]
      // [x, y] [x, y] [x, y] [x, y]
      CHECK_GL(glVertexAttribPointer(
          0,        // input location = 0
          2,        // vec2
          GL_FLOAT, // vector of floats
          GL_FALSE, // don't normalize input data
          4 * sizeof(float
              ),    // stride; offset between each vector (2 = xy, 4 = xyvu)
          (void *)0 // offset; where data begins in the buffer
      ));
      CHECK_GL(glEnableVertexAttribArray(0));

      CHECK_GL(glVertexAttribPointer(
          1,        // input location = 1
          2,        // vec2
          GL_FLOAT, // vector of floats
          GL_FALSE, // don't normalize input data
          4 * sizeof(float
              ), // stride; offset between each vector (2 = xy, 4 = xyvu)
          (void *)(2 * sizeof(float)) // offset; where data begins in the buffer
      ));
      CHECK_GL(glEnableVertexAttribArray(1));
    }

    Luna::Rectf mCrop;
    Luna::Vector2i mSize;
    bool mFlipVertical;
    unsigned int mVertexBuffer;
    unsigned int mElementBuffer;
    unsigned int mVertexAttribConf;
  };
} // namespace Luna::GL
