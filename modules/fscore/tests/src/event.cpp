#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

using namespace fs;

using DelegateArena = MemoryArena<Allocator<HeapAllocator, AllocationHeaderU32>,
                                MultiThread<MutexPrimitive>,
                                SimpleBoundsChecking,
                                FullMemoryTracking,
                                MemoryTagging>;

using TestEvent = Event<DelegateArena, void(void)>;

// These are already declared in delegate test. Should namespace them.
void func_VoidVoid();
// {
//     FS_ASSERT(!"Test should verify that invoking delegate triggers this assert");
// }
//
void func_VoidInt(u32 i);
// {
//     ++i;
//     FS_ASSERT(!"Test should verify that invoking delegate triggers this assert");
// }
//
void func_VoidIntInt(u32 a, u32 b);
// {
//     auto c = a+b;
//     (void)c;
//     FS_ASSERT(!"Test should verify that invoking delegate triggers this assert");
// }
//
u32 func_IntInt(u32 i);
// {
//     FS_ASSERT(!"Test should verify that invoking delegate triggers this assert");
//     return i*i;
// }

class DelegateClass
{
    public:
        void func_VoidVoid()
        {
            FS_ASSERT(!"Test should verify that invoking delegate triggers this assert");
        }

        void func_VoidInt(u32 i)
        {
            ++i;
            FS_ASSERT(!"Test should verify that invoking delegate triggers this assert");
        }

        void func_VoidIntInt(u32 a, u32 b)
        {
            auto c = a+b;
            (void)c;
            FS_ASSERT(!"Test should verify that invoking delegate triggers this assert");
        }

        u32 func_IntInt(u32 i)
        {
            FS_ASSERT(!"Test should verify that invoking delegate triggers this assert");
            return i*i;
        }

        void func_VoidVoidConst() const
        {
            FS_ASSERT(!"Test should verify that invoking delegate triggers this assert");
        }

        void func_VoidIntConst(u32 i) const
        {
            ++i;
            FS_ASSERT(!"Test should verify that invoking delegate triggers this assert");
        }

        void func_VoidIntIntConst(u32 a, u32 b) const
        {
            auto c = a+b;
            (void)c;
            FS_ASSERT(!"Test should verify that invoking delegate triggers this assert");
        }

        u32 func_IntIntConst(u32 i) const
        {
            FS_ASSERT(!"Test should verify that invoking delegate triggers this assert");
            return i*i;
        }
};

struct EventFixture
{
    EventFixture() :
        area(FS_SIZE_OF_MB * 4),
        arena(area, "DelegateArena"),
        event(&arena)
    {
    }

    ~EventFixture()
    {
    }

    DelegateClass instance;

    // Order below matters for allocation deallocation order
    HeapArea area;
    DelegateArena arena;
    TestEvent event;
};

BOOST_AUTO_TEST_SUITE(core)
BOOST_FIXTURE_TEST_SUITE(event, EventFixture)

BOOST_AUTO_TEST_CASE(add_to_channel_check_and_remove)
{
    auto channel = event.makeChannel(10);
    auto lambda = [](){};

    BOOST_CHECK(!channel->has<&func_VoidVoid>());
    BOOST_CHECK((!channel->has<DelegateClass, &DelegateClass::func_VoidVoid>(&instance)));
    BOOST_CHECK((!channel->has<DelegateClass, &DelegateClass::func_VoidVoidConst>(&instance)));
    BOOST_CHECK(!channel->has(lambda));

    channel->add<&func_VoidVoid>();
    BOOST_CHECK(channel->size() == 1);
    channel->add<DelegateClass, &DelegateClass::func_VoidVoid>(&instance);
    BOOST_CHECK(channel->size() == 2);
    channel->add<DelegateClass, &DelegateClass::func_VoidVoidConst>(&instance);
    BOOST_CHECK(channel->size() == 3);
    channel->add(lambda);
    BOOST_CHECK(channel->size() == 4);

    BOOST_CHECK(channel->has<&func_VoidVoid>());
    BOOST_CHECK((channel->has<DelegateClass, &DelegateClass::func_VoidVoid>(&instance)));
    BOOST_CHECK((channel->has<DelegateClass, &DelegateClass::func_VoidVoidConst>(&instance)));
    BOOST_CHECK(channel->has(lambda));

    channel->remove<&func_VoidVoid>();
    BOOST_CHECK(channel->size() == 3);
    channel->remove<DelegateClass, &DelegateClass::func_VoidVoid>(&instance);
    BOOST_CHECK(channel->size() == 2);
    channel->remove<DelegateClass, &DelegateClass::func_VoidVoidConst>(&instance);
    BOOST_CHECK(channel->size() == 1);
    channel->remove(lambda);
    BOOST_CHECK(channel->size() == 0);

    BOOST_CHECK(!channel->has<&func_VoidVoid>());
    BOOST_CHECK((!channel->has<DelegateClass, &DelegateClass::func_VoidVoid>(&instance)));
    BOOST_CHECK((!channel->has<DelegateClass, &DelegateClass::func_VoidVoidConst>(&instance)));
    BOOST_CHECK(!channel->has(lambda));
}

BOOST_AUTO_TEST_CASE(add_to_channel_max_assert)
{
    auto channel1 = event.makeChannel(1);
    channel1->add<&func_VoidVoid>();

    auto lambda = [&](){channel1->add<&func_VoidVoid>();};
    FS_REQUIRE_ASSERT(lambda);
}

BOOST_AUTO_TEST_CASE(add_to_event)
{

}

BOOST_AUTO_TEST_CASE(remove_from_event)
{

}

BOOST_AUTO_TEST_CASE(has_in_event)
{

}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
