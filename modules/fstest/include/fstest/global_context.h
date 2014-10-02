#ifndef FS_TEST_GLOBAL_CONTEXT_H
#define FS_TEST_GLOBAL_CONTEXT_H

#include <functional>
#include "fscore.h"
// #include "fslog.h"

#define FS_TEST_UNUSED(x)

#if SDL_ASSERT_LEVEL == 2
#define FS_REQUIRE_ASSERT(x) BOOST_REQUIRE(fs::test::GlobalContext::instance()->requireAssert((x)))
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
            }

            ~GlobalContext()
            {
            }

            static GlobalContext* instance()
            {
                static GlobalContext context;
                return &context;
            }

            bool requireAssert(std::function<void()> func)
            {
                auto oldLogger = core::getLogger();
                core::setLogger(nullptr);

                // auto rootLogger = log::getRootLogger();
                // bool oldSurpression;
                // if(rootLogger)
                // {
                //     oldSurpression = rootLogger->isConsoleSurpressed();
                //     rootLogger->setConsoleSurpressed(true);
                // }

                    fs::setAssertTriggered(false);

                    fs::setIgnoreAsserts(true);
                    func();
                    fs::setIgnoreAsserts(false);

                core::setLogger(oldLogger);

                // if(rootLogger)
                // {
                //     rootLogger->setConsoleSurpressed(oldSurpression);
                // }

                return fs::getAssertTriggered();
            }
        };
    }
}

#endif
