#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"
#include "fsmem.h"
#include "fsutil.h"

using namespace fs;

struct FlagFixture
{
    FlagFixture()
    {
    }

    ~FlagFixture()
    {
    }
};

FS_DECLARE_FLAGS(TestFlagsEnum, A, B, C);

using TestFlags = Flags<TestFlagsEnum>;

BOOST_AUTO_TEST_SUITE(core)
BOOST_FIXTURE_TEST_SUITE(flags, FlagFixture)

BOOST_AUTO_TEST_CASE(construct_empty_flags)
{
    TestFlags flags;
    BOOST_CHECK(!flags.isAnySet());
    BOOST_CHECK(!flags.areAllSet());
    BOOST_CHECK(!flags.isSet(TestFlags::A));
    BOOST_CHECK(!flags.isSet(TestFlags::B));
    BOOST_CHECK(!flags.isSet(TestFlags::C));
}

BOOST_AUTO_TEST_CASE(constuct_flags_with_operator_or)
{
    TestFlags flags = TestFlags::A | TestFlags::B | TestFlags::C;
    BOOST_CHECK(flags.isAnySet());
    BOOST_CHECK(flags.areAllSet());
    BOOST_CHECK(flags.isSet(TestFlags::A));
    BOOST_CHECK(flags.isSet(TestFlags::B));
    BOOST_CHECK(flags.isSet(TestFlags::C));
}

BOOST_AUTO_TEST_CASE(set_and_unset_flags)
{
    TestFlags flags;
    flags.set(TestFlags::A);
    BOOST_CHECK(flags.isSet(TestFlags::A));

    flags.remove(TestFlags::A);
    BOOST_CHECK(!flags.isSet(TestFlags::A));

    flags.set(TestFlags::A);
    flags.set(TestFlags::B);
    BOOST_CHECK(flags.isSet(TestFlags::A));
    BOOST_CHECK(flags.isSet(TestFlags::B));
}

BOOST_AUTO_TEST_CASE(combine_with_operator_or)
{
    TestFlags flagsA(TestFlags::A);
    TestFlags flagsB(TestFlags::B);

    TestFlags flagsAB = flagsA | flagsB;
    BOOST_CHECK(flagsAB.isSet(TestFlags::A));
    BOOST_CHECK(flagsAB.isSet(TestFlags::B));

    flagsAB.clear();
    flagsAB |= flagsA;
    flagsAB |= flagsB;
    BOOST_CHECK(flagsAB.isSet(TestFlags::A));
    BOOST_CHECK(flagsAB.isSet(TestFlags::B));

    flagsAB.clear();
    flagsAB = TestFlags::A | TestFlags::B;
    BOOST_CHECK(flagsAB.isSet(TestFlags::A));
    BOOST_CHECK(flagsAB.isSet(TestFlags::B));
}

BOOST_AUTO_TEST_CASE(clear_flags)
{
    TestFlags flags = TestFlags::A | TestFlags::B | TestFlags::C;
    BOOST_CHECK(flags.isAnySet());

    flags.clear();
    BOOST_CHECK(!flags.isAnySet());
}

BOOST_AUTO_TEST_CASE(valid_description)
{
    TestFlags flags;

    // Empty Flags
    TestFlags::Description desc;
    flags.toString(desc);
    auto length = std::strlen(desc);
    BOOST_CHECK_MESSAGE(length == 0, "length was " << length << " and desc was '" << desc << "'");

    // Single Flags
    flags.set(TestFlags::A);
    flags.toString(desc);
    BOOST_CHECK(std::strlen(desc) > 0);
    BOOST_CHECK_MESSAGE(std::strcmp("A", desc) == 0, "desc was '" << desc << "'");

    // Multiple Flags
    flags |= TestFlags::B | TestFlags::C;
    flags.toString(desc);
    BOOST_CHECK(std::strlen(desc) > 0);
    BOOST_CHECK_MESSAGE(std::strcmp("A, B, C", desc) == 0, "desc was '" << desc << "'");
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

