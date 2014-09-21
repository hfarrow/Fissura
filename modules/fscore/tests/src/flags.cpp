#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

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

FS_DECLARE_FLAGS(Test, A, B, C);

using TestFlags = Flags<Test>;

BOOST_AUTO_TEST_SUITE(core)
BOOST_FIXTURE_TEST_SUITE(flags, FlagFixture)

BOOST_AUTO_TEST_CASE(construct_empty_flags)
{
    TestFlags flags;
    BOOST_CHECK(!flags.isAnySet());
    BOOST_CHECK(!flags.areAllSet());
    BOOST_CHECK(!flags.isSet(Test::A));
    BOOST_CHECK(!flags.isSet(Test::B));
    BOOST_CHECK(!flags.isSet(Test::C));
}

BOOST_AUTO_TEST_CASE(constuct_flags_with_operator_or)
{
    TestFlags flags = Test::A | Test::B | Test::C;
    BOOST_CHECK(flags.isAnySet());
    BOOST_CHECK(flags.areAllSet());
    BOOST_CHECK(flags.isSet(Test::A));
    BOOST_CHECK(flags.isSet(Test::B));
    BOOST_CHECK(flags.isSet(Test::C));
}

BOOST_AUTO_TEST_CASE(set_and_unset_flags)
{
    TestFlags flags;
    flags.set(Test::A);
    BOOST_CHECK(flags.isSet(Test::A));

    flags.remove(Test::A);
    BOOST_CHECK(!flags.isSet(Test::A));

    flags.set(Test::A);
    flags.set(Test::B);
    BOOST_CHECK(flags.isSet(Test::A));
    BOOST_CHECK(flags.isSet(Test::B));
}

BOOST_AUTO_TEST_CASE(combine_with_operator_or)
{
    TestFlags flagsA(Test::A);
    TestFlags flagsB(Test::B);

    TestFlags flagsAB = flagsA | flagsB;
    BOOST_CHECK(flagsAB.isSet(Test::A));
    BOOST_CHECK(flagsAB.isSet(Test::B));

    flagsAB.clear();
    flagsAB |= flagsA;
    flagsAB |= flagsB;
    BOOST_CHECK(flagsAB.isSet(Test::A));
    BOOST_CHECK(flagsAB.isSet(Test::B));

    flagsAB.clear();
    flagsAB = Test::A | Test::B;
    BOOST_CHECK(flagsAB.isSet(Test::A));
    BOOST_CHECK(flagsAB.isSet(Test::B));
}

BOOST_AUTO_TEST_CASE(clear_flags)
{
    TestFlags flags = Test::A | Test::B | Test::C;
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
    flags.set(Test::A);
    flags.toString(desc);
    BOOST_CHECK(std::strlen(desc) > 0);
    BOOST_CHECK_MESSAGE(std::strcmp("A", desc) == 0, "desc was '" << desc << "'");

    // Multiple Flags
    flags |= Test::B | Test::C;
    flags.toString(desc);
    BOOST_CHECK(std::strlen(desc) > 0);
    BOOST_CHECK_MESSAGE(std::strcmp("A, B, C", desc) == 0, "desc was '" << desc << "'");
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

