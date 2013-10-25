#ifndef FS_TEST_GLOBAL_CONTEXT_H
#define FS_TEST_GLOBAL_CONTEXT_H

#include <functional>

#define FS_REQUIRE_ASSERT(x) fs::test::GlobalContext::instance()->requireAssert((x))

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
        void requireAssert(std::function<void()> func);

    private:
        static GlobalContext* _pInstance;
    };
}
}

#endif
