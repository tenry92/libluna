#include <vector>

#include <libluna/ResourceRef.hpp>
#include <libluna/String.hpp>
#include <libluna/Test.hpp>

using namespace std;
using namespace Luna;

enum LogType {
  ConstructingMyResource,
  DeconstructingMyResource,
  LoadingMyResource,
};

static std::vector<int> log;

class MyResource {
  public:
  MyResource(const Luna::String &text) {
    log.push_back(ConstructingMyResource);
    mText = text;
  }

  ~MyResource() { log.push_back(DeconstructingMyResource); }

  const Luna::String &getText() const { return mText; }

  private:
  Luna::String mText;
};

class MyResourceLoader {
  public:
  MyResourceLoader(const Luna::String &name) : mName(name) {}

  shared_ptr<MyResource> operator()() {
    log.push_back(LoadingMyResource);
    return make_shared<MyResource>(mName);
  }

  private:
  const Luna::String mName;
};

int main(int, char **) {
  TEST("futures", []() {
    shared_ptr<MyResource> resource1, resource2;
    auto resource = Luna::ResourceRef<MyResource>(MyResourceLoader("test"));

    ASSERT(resource.isReady() == false, "resource is not ready yet");

    auto future1 = resource.get();
    auto future2 = resource.get();
    resource1 = future1.get();
    ASSERT(resource.isReady() == true, "resource is ready");
    resource2 = future2.get();

    ASSERT(resource1->getText() == "test", "resource1 loaded");
    ASSERT(resource2->getText() == "test", "resource2 loaded");

    resource1 = nullptr;
    ASSERT(resource.isReady() == true, "resource is ready");
    resource2 = nullptr;
    ASSERT(resource.isReady() == false, "resource expired");
  });

  return runTests();
}
