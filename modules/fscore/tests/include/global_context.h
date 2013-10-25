#ifndef FS_TEST_GLOBAL_CONTEXT_H
#define FS_TEST_GLOBAL_CONTEXT_H

#include <functional>

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
