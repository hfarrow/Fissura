#ifndef FS_TEST_GLOBAL_CONTEXT_H
#define FS_TEST_GLOBAL_CONTEXT_H

#include <functional>

#define FS_TEST_UNUSED(x)

#if SDL_ASSERT_LEVEL == 2
#define FS_REQUIRE_ASSERT(x) BOOST_CHECK(fs::test::GlobalContext::instance()->requireAssert((x)))
#else
#define FS_REQUIRE_ASSERT(x) FS_TEST_UNUSED(x)
#endif

namespace fs
{
namespace test
{
    class GlobalContext
    {
    public:
        GlobalContext();
        ~GlobalContext();
        static GlobalContext* instance();
        bool requireAssert(std::function<void()> func);

    private:
        static GlobalContext* _pInstance;
    };
}
}

#endif
