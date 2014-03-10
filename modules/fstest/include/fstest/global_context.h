#ifndef FS_TEST_GLOBAL_CONTEXT_H
#define FS_TEST_GLOBAL_CONTEXT_H

#include <functional>
#include "fscore.h"

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
        GlobalContext()
        {
            _pInstance = this;
        }

        ~GlobalContext()
        {
        
        }

        static GlobalContext* instance();

        bool requireAssert(std::function<void()> func)
        {
            //bool surpressStdOutputOld = fs::Logger::getSurpressStdOutput();
            //fs::Logger::setSurpressStdOutput(true);
            fs::setAssertTriggered(false);

            func();

            //fs::Logger::setSurpressStdOutput(surpressStdOutputOld);
            return fs::getAssertTriggered();
            
            // If the assert results in an abort(), the below code would work.
            // SDL_assert, however can not easily be made to abort with out corrupting its state so
            // a custom assert hanlder is used to ignore the assert and set fs:assertTriggered to true.
            //
            // BOOST_TEST_MESSAGE("GlobalContext requireAssert - Assert expected below this point.");
            // auto& monitor = boost::unit_test::unit_test_monitor_t::instance();
            // BOOST_REQUIRE_THROW(monitor.execute([&func](){func(); return 0;}), boost::execution_exception);
            // BOOST_TEST_MESSAGE("GlobalContext requireAssert - End of expected assert above this point.");
        }

    private:
        static GlobalContext* _pInstance;
    };


    GlobalContext* GlobalContext::_pInstance = nullptr;
}
}

#endif
