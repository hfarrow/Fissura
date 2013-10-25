#include <boost/test/unit_test_monitor.hpp>
#include <boost/test/unit_test.hpp>

#include "fscore/assert.h"

#include "global_context.h"

using namespace fs::test;

GlobalContext* GlobalContext::_pInstance = nullptr;

GlobalContext::GlobalContext()
{
    _pInstance = this;
    fs::setAbortOnAssert(true);
}

GlobalContext::~GlobalContext()
{

}

GlobalContext* GlobalContext::instance()
{
    return _pInstance;
}

void GlobalContext::requireAssert(std::function<void()> func)
{
    BOOST_TEST_MESSAGE("GlobalContext requireAssert - Assert expected below this point.");
    auto& monitor = boost::unit_test::unit_test_monitor_t::instance();
    BOOST_REQUIRE_THROW(monitor.execute([&func](){func(); return 0;}), boost::execution_exception);
}
