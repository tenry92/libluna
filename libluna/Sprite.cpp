#include <cmath>

#include <libluna/Sprite.hpp>

using namespace Luna;

class Sprite::impl {
  public:
  std::shared_ptr<ResourceRef<Image>> mImage;
  std::shared_ptr<ResourceRef<std::vector<Luna::Color>>> mPalette;
  float mCurrentFrame{0.0f};
  int mFirstFrame{0};
  int mFrameCount{1};
  float mFrameRate{0.0f};
  Vector2f mPosition;
};

Sprite::Sprite() : mImpl{std::make_unique<impl>()} {}

Sprite::~Sprite() = default;

void Sprite::setImage(std::shared_ptr<ResourceRef<Image>> image) {
  mImpl->mImage = image;
}

std::shared_ptr<ResourceRef<Image>> Sprite::getImage() const {
  return mImpl->mImage;
}

void Sprite::setPalette(
    std::shared_ptr<ResourceRef<std::vector<Luna::Color>>> palette
) {
  mImpl->mPalette = palette;
}

std::shared_ptr<ResourceRef<std::vector<Luna::Color>>>
Sprite::getPalette() const {
  return mImpl->mPalette;
}

void Sprite::setFrame(int frame) {
  mImpl->mCurrentFrame = static_cast<float>(frame);
}

int Sprite::getFrame() const { return static_cast<int>(mImpl->mCurrentFrame); }

void Sprite::setAnimationFrames(int firstFrame, int frameCount) {
  mImpl->mFirstFrame = firstFrame;
  mImpl->mFrameCount = frameCount;
}

void Sprite::setFrameRate(float framesPerSecond) {
  mImpl->mFrameRate = framesPerSecond;
}

void Sprite::advanceAnimation(float seconds) {
  mImpl->mCurrentFrame =
      static_cast<float>(std::fmod(
          mImpl->mCurrentFrame - static_cast<float>(mImpl->mFirstFrame) +
              mImpl->mFrameRate * seconds,
          mImpl->mFrameCount
      )) +
      static_cast<float>(mImpl->mFirstFrame);
}

void Sprite::setPosition(const Vector2f &position) {
  mImpl->mPosition = position;
}

Vector2f Sprite::getPosition() const { return mImpl->mPosition; }
