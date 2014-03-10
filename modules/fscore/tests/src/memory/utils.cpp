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

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
