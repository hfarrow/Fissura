#include <boost/test/unit_test_monitor.hpp>
#include <boost/test/unit_test.hpp>

#include "fstest/global_context.h"

using namespace fs::test;

GlobalContext* GlobalContext::_pInstance = nullptr;

GlobalContext::GlobalContext()
{
    _pInstance = this;
}

GlobalContext::~GlobalContext()
{

}

GlobalContext* GlobalContext::instance()
{
    return _pInstance;
}

namespace fs
{
    extern bool assertTriggered;
    namespace Logger
    {
        extern bool surpressStdOutput;
    }
}
bool GlobalContext::requireAssert(std::function<void()> func)
{
    bool surpressStdOutputOld = fs::Logger::surpressStdOutput;
    fs::Logger::surpressStdOutput = true;
    fs::assertTriggered = false;

    func();

    fs::Logger::surpressStdOutput = surpressStdOutputOld;
    return fs::assertTriggered;
    
    // If the assert results in an abort(), the below code would work.
    // SDL_assert, however can not easily be made to abort with out corrupting its state so
    // a custom assert hanlder is used to ignore the assert and set fs:assertTriggered to true.
    //
    // BOOST_TEST_MESSAGE("GlobalContext requireAssert - Assert expected below this point.");
    // auto& monitor = boost::unit_test::unit_test_monitor_t::instance();
    // BOOST_REQUIRE_THROW(monitor.execute([&func](){func(); return 0;}), boost::execution_exception);
    // BOOST_TEST_MESSAGE("GlobalContext requireAssert - End of expected assert above this point.");
}
