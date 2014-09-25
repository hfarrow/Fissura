#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"
#include "fsio.h"

using namespace fs;

struct OsFileFixture
{
    OsFileFixture()
    {
    }

    ~OsFileFixture()
    {
    }
};


BOOST_AUTO_TEST_SUITE(io)
BOOST_FIXTURE_TEST_SUITE(osfile, OsFileFixture)

BOOST_AUTO_TEST_CASE(open_empty_file_for_read)
{
    OsFile file("content/empty.bin", FileSystem::Mode::READ, false);
    BOOST_REQUIRE(file.opened());
}

BOOST_AUTO_TEST_CASE(open_small_file_for_read)
{
    OsFile file("content/small.bin", FileSystem::Mode::READ, false);
    BOOST_REQUIRE(file.opened());
}

BOOST_AUTO_TEST_CASE(close_file)
{
    OsFile file("content/empty.bin", FileSystem::Mode::READ, false);
    file.~OsFile();
    BOOST_REQUIRE(!file.opened());
}

BOOST_AUTO_TEST_CASE(empty_file_size_is_zero)
{
    OsFile file("content/empty.bin", FileSystem::Mode::READ, false);
    file.seekToEnd();
    BOOST_CHECK(file.tell() == 0);
}

BOOST_AUTO_TEST_CASE(small_file_size_is_correct)
{
    OsFile file("content/small.bin", FileSystem::Mode::READ, false);
    file.seekToEnd();
    BOOST_CHECK(file.tell() == 1951);
}

BOOST_AUTO_TEST_CASE(open_missing_file_for_read)
{
    auto lambda = [](){OsFile file("content/missing.bin", FileSystem::Mode::READ, false);};
    FS_REQUIRE_ASSERT(lambda);
}

BOOST_AUTO_TEST_CASE(seek_to_position)
{
    OsFile file("content/small.bin", FileSystem::Mode::READ, false);
    file.seek(100);
    BOOST_CHECK(file.tell() == 100);
}

BOOST_AUTO_TEST_CASE(skip_forward)
{
    OsFile file("content/small.bin", FileSystem::Mode::READ, false);
    file.skip(100);
    BOOST_CHECK(file.tell() == 100);
    file.skip(100);
    BOOST_CHECK(file.tell() == 200);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()


