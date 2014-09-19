#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

using namespace fs;

using DelegateArena = MemoryArena<Allocator<HeapAllocator, AllocationHeaderU32>,
                                MultiThread<MutexPrimitive>,
                                SimpleBoundsChecking,
                                FullMemoryTracking,
                                MemoryTagging>;

using DelegateVoidVoid= Delegate<void(void), DelegateArena>;
using DelegateVoidInt= Delegate<void(u32), DelegateArena>;
using DelegateVoidIntInt= Delegate<void(u32, u32), DelegateArena>;
using DelegateIntInt= Delegate<u32(u32), DelegateArena>;

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

struct DelegateFixture
{
    DelegateFixture() :
        area(FS_SIZE_OF_MB * 4),
        arena(area, "DelegateArena")
    {
    }

    ~DelegateFixture()
    {
    }

    void bindAllToFreeFunctions()
    {
        d1VoidVoid.bind<&func_VoidVoid>();
        d2VoidVoid = DelegateVoidVoid::from<&func_VoidVoid>();
        d1VoidInt.bind<&func_VoidInt>();
        d2VoidInt = DelegateVoidInt::from<&func_VoidInt>();
        d1VoidIntInt.bind<&func_VoidIntInt>();
        d2VoidIntInt = DelegateVoidIntInt::from<&func_VoidIntInt>();
        d1IntInt.bind<&func_IntInt>();
        d2IntInt = DelegateIntInt::from<&func_IntInt>();
    }

    void bindAllToMemberFunctions()
    {
        d1VoidVoid.bind<DelegateClass, &DelegateClass::func_VoidVoid>(&instance);
        d2VoidVoid = DelegateVoidVoid::from<DelegateClass, &DelegateClass::func_VoidVoid>(&instance);
        d1VoidInt.bind<DelegateClass, &DelegateClass::func_VoidInt>(&instance);
        d2VoidInt = DelegateVoidInt::from<DelegateClass, &DelegateClass::func_VoidInt>(&instance);
        d1VoidIntInt.bind<DelegateClass, &DelegateClass::func_VoidIntInt>(&instance);
        d2VoidIntInt = DelegateVoidIntInt::from<DelegateClass, &DelegateClass::func_VoidIntInt>(&instance);
        d1IntInt.bind<DelegateClass, &DelegateClass::func_IntInt>(&instance);
        d2IntInt = DelegateIntInt::from<DelegateClass, &DelegateClass::func_IntInt>(&instance);
    }

    void bindAllToConstMemberFunctions()
    {
        d1VoidVoid.bind<DelegateClass, &DelegateClass::func_VoidVoidConst>(&instance);
        d2VoidVoid = DelegateVoidVoid::from<DelegateClass, &DelegateClass::func_VoidVoidConst>(&instance);
        d1VoidInt.bind<DelegateClass, &DelegateClass::func_VoidIntConst>(&instance);
        d2VoidInt = DelegateVoidInt::from<DelegateClass, &DelegateClass::func_VoidIntConst>(&instance);
        d1VoidIntInt.bind<DelegateClass, &DelegateClass::func_VoidIntIntConst>(&instance);
        d2VoidIntInt = DelegateVoidIntInt::from<DelegateClass, &DelegateClass::func_VoidIntIntConst>(&instance);
        d1IntInt.bind<DelegateClass, &DelegateClass::func_IntIntConst>(&instance);
        d2IntInt = DelegateIntInt::from<DelegateClass, &DelegateClass::func_IntIntConst>(&instance);
    }

    void bindAllToLambdaFunctions()
    {
        auto lVoidVoid = [](){FS_ASSERT(!"Test should verify that invoking delegate triggers this assert");};
        auto lVoidInt = [](u32 i){i++;FS_ASSERT(!"Test should verify that invoking delegate triggers this assert");};
        auto lVoidIntInt = [](u32 a, u32 b){auto c = a + b;(void)c;FS_ASSERT(!"Test should verify that invoking delegate triggers this assert");};
        auto lIntInt = [](u32 i){FS_ASSERT(!"Test should verify that invoking delegate triggers this assert");return i*i;};

        d1VoidVoid = DelegateVoidVoid(lVoidVoid, &arena);
        d2VoidVoid = DelegateVoidVoid(lVoidVoid, &arena);
        d1VoidInt = DelegateVoidInt(lVoidInt, &arena);
        d2VoidInt = DelegateVoidInt(lVoidInt, &arena);
        d1VoidIntInt = DelegateVoidIntInt(lVoidIntInt, &arena);
        d2VoidIntInt = DelegateVoidIntInt(lVoidIntInt, &arena);
        d1IntInt = DelegateIntInt(lIntInt, &arena);
        d2IntInt = DelegateIntInt(lIntInt, &arena);
    }

    void testInvoke()
    {
        FS_REQUIRE_ASSERT([&](){d1VoidVoid.invoke();});
        FS_REQUIRE_ASSERT([&](){d2VoidVoid.invoke();});
        FS_REQUIRE_ASSERT([&](){d1VoidVoid();});
        FS_REQUIRE_ASSERT([&](){d2VoidVoid();});

        FS_REQUIRE_ASSERT([&](){d1VoidInt.invoke(1);});
        FS_REQUIRE_ASSERT([&](){d2VoidInt.invoke(1);});
        FS_REQUIRE_ASSERT([&](){d1VoidInt(1);});
        FS_REQUIRE_ASSERT([&](){d2VoidInt(1);});

        FS_REQUIRE_ASSERT([&](){d1VoidIntInt.invoke(1, 2);});
        FS_REQUIRE_ASSERT([&](){d2VoidIntInt.invoke(1, 2);});
        FS_REQUIRE_ASSERT([&](){d1VoidIntInt(1, 2);});
        FS_REQUIRE_ASSERT([&](){d2VoidIntInt(1, 2);});

        FS_REQUIRE_ASSERT([&](){d1IntInt.invoke(2);});
        FS_REQUIRE_ASSERT([&](){d2IntInt.invoke(2);});
        FS_REQUIRE_ASSERT([&](){d1IntInt(2);});
        FS_REQUIRE_ASSERT([&](){d2IntInt(2);});
    }

    DelegateClass instance;

    // Order below matters for allocation deallocation order
    HeapArea area;
    DelegateArena arena;

    DelegateVoidVoid d1VoidVoid;
    DelegateVoidVoid d2VoidVoid;
    DelegateVoidInt d1VoidInt;
    DelegateVoidInt d2VoidInt;
    DelegateVoidIntInt d1VoidIntInt;
    DelegateVoidIntInt d2VoidIntInt;
    DelegateIntInt d1IntInt;
    DelegateIntInt d2IntInt;
};

BOOST_AUTO_TEST_SUITE(core)
BOOST_FIXTURE_TEST_SUITE(delegate, DelegateFixture)

BOOST_AUTO_TEST_CASE(free_function_bind_and_invoke)
{
    bindAllToFreeFunctions();
    testInvoke();
}

BOOST_AUTO_TEST_CASE(member_function_bind_and_invoke)
{
    bindAllToMemberFunctions();
    testInvoke();
}

BOOST_AUTO_TEST_CASE(const_member_function_bind_and_invoke)
{
    bindAllToConstMemberFunctions();
    testInvoke();
}

BOOST_AUTO_TEST_CASE(lambda_function_bind_and_invoke)
{
    bindAllToLambdaFunctions();
    testInvoke();
}

BOOST_AUTO_TEST_CASE(rebind_and_invoke)
{
    bindAllToFreeFunctions();
    bindAllToFreeFunctions();
    testInvoke();

    bindAllToMemberFunctions();
    testInvoke();

    bindAllToConstMemberFunctions();
    testInvoke();

    bindAllToLambdaFunctions();
    testInvoke();

    bindAllToFreeFunctions();
    testInvoke();

    bindAllToLambdaFunctions();
    testInvoke();

    bindAllToLambdaFunctions();
    testInvoke();
}

BOOST_AUTO_TEST_CASE(operator_equals)
{
    // Create two delegates from same function
    DelegateVoidVoid d1 = DelegateVoidVoid::from<&func_VoidVoid>();
    DelegateVoidVoid d2 = DelegateVoidVoid::from<&func_VoidVoid>();
    BOOST_CHECK(d1 == d1);
    BOOST_CHECK(d1 == d2);

    // Create a delegate to a similar member function
    DelegateVoidVoid d3 = DelegateVoidVoid::from<DelegateClass, &DelegateClass::func_VoidVoid>(&instance);

    BOOST_CHECK(d3 != d1);
    BOOST_CHECK(d3 != d2);
    BOOST_CHECK(d3 == d3);

    // Create an empty delegate
    DelegateVoidVoid d4;
    BOOST_CHECK(d4 != d1);
    BOOST_CHECK(d4 != d3);
    BOOST_CHECK(d4 == d4);

    // copy existing delegate to empty delegate
    d4 = d1;
    BOOST_CHECK(d4 == d1);
    BOOST_CHECK(d4 == d2);
    BOOST_CHECK(d4 != d3);

    // Delegate from lambda
    DelegateVoidVoid d5([](){}, &arena);
    BOOST_CHECK(d5 != d1);
    BOOST_CHECK(d5 != d3);
    BOOST_CHECK(d5 == d5);

    // Make copy of lambda
    DelegateVoidVoid d6 = d5;
    BOOST_CHECK(d6 == d5);

    // Make new lambda of same signature and implementation... should be a unique delegate.
    DelegateVoidVoid d7([](){}, &arena);
    BOOST_CHECK(d7 != d5);
    BOOST_CHECK(d7 != d6);

    // assign non lambda to existing lambda delegate
    d5 = d1;
    BOOST_CHECK(d5 != d6);
    BOOST_CHECK(d5 == d1);

    // assign lambda to empty delegate
    DelegateVoidVoid d8;
    d8 = d6;
    BOOST_CHECK(d8 == d6);
}

BOOST_AUTO_TEST_CASE(check_bound)
{
    DelegateVoidVoid d1;
    BOOST_CHECK(!d1.bound());
    d1 = DelegateVoidVoid::from<&func_VoidVoid>();
    BOOST_CHECK(d1.bound());

    auto d2 = DelegateVoidVoid::from<&func_VoidVoid>();
    BOOST_CHECK(d2.bound());

    DelegateVoidVoid d3([](){}, &arena);
    BOOST_CHECK(d3.bound());

    auto d4 = d3;
    BOOST_CHECK(d4.bound());

    DelegateVoidVoid d5(&arena);
    BOOST_CHECK(!d5.bound());
    d5.bind<&func_VoidVoid>();
    BOOST_CHECK(d5.bound());
}

BOOST_AUTO_TEST_CASE(assert_null_arena_for_lambda)
{
    auto lambda = [](){};
    auto assertTest = [&](){
            Delegate<void(void), DelegateArena> d1(lambda, nullptr);
            };

    FS_REQUIRE_ASSERT(assertTest);

    DelegateVoidVoid d1;
    FS_REQUIRE_ASSERT([&](){d1 = [](){};});
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
