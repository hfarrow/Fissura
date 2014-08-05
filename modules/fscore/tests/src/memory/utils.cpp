#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(memory)
BOOST_AUTO_TEST_SUITE(utils)

BOOST_AUTO_TEST_CASE(align_top)
{
    BOOST_CHECK(0 == pointerUtil::alignTop(0, 2));
	BOOST_CHECK(2 == pointerUtil::alignTop(1, 2));
	BOOST_CHECK(0 == pointerUtil::alignTop(0, 4));
	BOOST_CHECK(4 == pointerUtil::alignTop(2, 4));
	BOOST_CHECK(0 == pointerUtil::alignTop(0, 16));
	BOOST_CHECK(16 == pointerUtil::alignTop(2, 16));
	BOOST_CHECK(16 == pointerUtil::alignTop(4, 16));
	BOOST_CHECK(16 == pointerUtil::alignTop(13, 16));
	BOOST_CHECK(16 == pointerUtil::alignTop(16, 16));
}

BOOST_AUTO_TEST_CASE(align_top_amount)
{
	BOOST_CHECK( 0 == pointerUtil::alignTopAmount(0, 2));
	BOOST_CHECK( 1 == pointerUtil::alignTopAmount(1, 2));
	BOOST_CHECK( 0 == pointerUtil::alignTopAmount(0, 4));
	BOOST_CHECK( 3 == pointerUtil::alignTopAmount(1, 4));
	BOOST_CHECK( 2 == pointerUtil::alignTopAmount(2, 4));
	BOOST_CHECK( 1 == pointerUtil::alignTopAmount(3, 4));
	BOOST_CHECK( 15 == pointerUtil::alignTopAmount(1, 16));
	BOOST_CHECK( 3 == pointerUtil::alignTopAmount(13, 16));

}

BOOST_AUTO_TEST_CASE(align_bottom)
{
    const uptr base = 512;
    BOOST_CHECK(base == pointerUtil::alignBottom(base + 0, 2));
	BOOST_CHECK(base == pointerUtil::alignBottom(base + 1, 2));
	BOOST_CHECK(base == pointerUtil::alignBottom(base + 0, 4));
	BOOST_CHECK(base == pointerUtil::alignBottom(base + 2, 4));
	BOOST_CHECK(base == pointerUtil::alignBottom(base + 0, 16));
	BOOST_CHECK(base == pointerUtil::alignBottom(base + 2, 16));
	BOOST_CHECK(base == pointerUtil::alignBottom(base + 4, 16));
	BOOST_CHECK(base == pointerUtil::alignBottom(base + 13, 16));
	BOOST_CHECK(base + 16 == pointerUtil::alignBottom(base + 16, 16));
	BOOST_CHECK(base + 16 == pointerUtil::alignBottom(base + 17, 16));
	BOOST_CHECK(base + 16 == pointerUtil::alignBottom(base + 31, 16));
	BOOST_CHECK(base + 32 == pointerUtil::alignBottom(base + 32, 16));
}

BOOST_AUTO_TEST_CASE(align_bottom_amount)
{
    const uptr base = 512;
	BOOST_CHECK( 0 == pointerUtil::alignBottomAmount(base + 0, 2));
	BOOST_CHECK( 1 == pointerUtil::alignBottomAmount(base + 1, 2));
	BOOST_CHECK( 0 == pointerUtil::alignBottomAmount(base + 0, 4));
	BOOST_CHECK( 1 == pointerUtil::alignBottomAmount(base + 1, 4));
	BOOST_CHECK( 2 == pointerUtil::alignBottomAmount(base + 2, 4));
	BOOST_CHECK( 3 == pointerUtil::alignBottomAmount(base + 3, 4));
	BOOST_CHECK( 1 == pointerUtil::alignBottomAmount(base + 1, 16));
	BOOST_CHECK( 13 == pointerUtil::alignBottomAmount(base + 13, 16));

}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
