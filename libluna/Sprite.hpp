#pragma once

#include <memory>

#include <libluna/Color.hpp>
#include <libluna/Image.hpp>
#include <libluna/ResourceRef.hpp>
#include <libluna/String.hpp>
#include <libluna/Vector.hpp>

namespace Luna {
  class Sprite final {
    public:
    Sprite();
    ~Sprite();

    void setImage(std::shared_ptr<ResourceRef<Image>> image);
    std::shared_ptr<ResourceRef<Image>> getImage() const;

    void
    setPalette(std::shared_ptr<ResourceRef<std::vector<Luna::Color>>> palette);
    std::shared_ptr<ResourceRef<std::vector<Luna::Color>>> getPalette() const;

    void setFrame(int frame);
    int getFrame() const;
    void setAnimationFrames(int firstFrame, int frameCount);
    void setFrameRate(float framesPerSecond);
    void advanceAnimation(float seconds);

    void setPosition(const Vector2f &position);
    Vector2f getPosition() const;

    private:
    class impl;
    std::unique_ptr<impl> mImpl;
  };
} // namespace Luna
