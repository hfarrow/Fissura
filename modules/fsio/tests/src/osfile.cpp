#include <cstdio>
#include <boost/test/unit_test.hpp>

#include "global_fixture.h"
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

    const char* path(const char* path)
    {
        _temp = GlobalFixture::instance()->path(path);
        return _temp.c_str();
    }

    std::string _temp;
};


BOOST_AUTO_TEST_SUITE(io)
BOOST_FIXTURE_TEST_SUITE(osfile, OsFileFixture)

BOOST_AUTO_TEST_CASE(open_empty_file_for_read)
{
    OsFile file(path("content/empty.bin"), FileSystem::Mode::READ, false);
    BOOST_REQUIRE(file.opened());
}

BOOST_AUTO_TEST_CASE(open_small_file_for_read)
{
    OsFile file(path("content/small.bin"), FileSystem::Mode::READ, false);
    BOOST_REQUIRE(file.opened());
}

BOOST_AUTO_TEST_CASE(close_file)
{
    OsFile file(path("content/empty.bin"), FileSystem::Mode::READ, false);
    file.~OsFile();
    BOOST_REQUIRE(!file.opened());
}

BOOST_AUTO_TEST_CASE(empty_file_size_is_zero)
{
    OsFile file(path("content/empty.bin"), FileSystem::Mode::READ, false);
    file.seekToEnd();
    BOOST_CHECK(file.tell() == 0);
}

BOOST_AUTO_TEST_CASE(small_file_size_is_correct)
{
    OsFile file(path("content/small.bin"), FileSystem::Mode::READ, false);
    file.seekToEnd();
    BOOST_CHECK(file.tell() == 1951);
}

BOOST_AUTO_TEST_CASE(open_missing_file_for_read)
{
    auto lambda = [&](){OsFile file(path("content/missing.bin"), FileSystem::Mode::READ, false);};
    FS_REQUIRE_ASSERT(lambda);
}

BOOST_AUTO_TEST_CASE(seek_to_position)
{
    OsFile file(path("content/small.bin"), FileSystem::Mode::READ, false);
    file.seek(100);
    BOOST_CHECK(file.tell() == 100);
}

BOOST_AUTO_TEST_CASE(skip_forward)
{
    OsFile file(path("content/small.bin"), FileSystem::Mode::READ, false);
    file.skip(100);
    BOOST_CHECK(file.tell() == 100);
    file.skip(100);
    BOOST_CHECK(file.tell() == 200);
}

BOOST_AUTO_TEST_CASE(open_new_file_for_write)
{
    const char* fileName = path("content/new.bin");
    std::remove(fileName);
    {
        OsFile file(fileName, FileSystem::Mode::WRITE | FileSystem::Mode::CREATE, false);
    }

    // If the file was created, we should be able to delete it.
    BOOST_REQUIRE(std::remove(fileName) == 0);
}

BOOST_AUTO_TEST_CASE(write_and_read)
{
    const char* fileName = path("content/new.bin");
    std::remove(fileName);

    {
        OsFile file(fileName, FileSystem::Mode::WRITE | FileSystem::Mode::CREATE, false);

        char data[512] = {0};
        std::memset(data, 'a', sizeof(data));
        file.write(data, sizeof(data));
    }

    {
        OsFile file(fileName, FileSystem::Mode::READ, false);

        char data[512] = {0};
        file.read(data, sizeof(data));

        BOOST_CHECK(data[0] == 'a'); // start
        BOOST_CHECK(data[sizeof(data)-1] == 'a'); // end
        BOOST_CHECK(data[sizeof(data)/2] == 'a'); // middle
    }
}

BOOST_AUTO_TEST_CASE(write_then_append)
{
    const char* fileName = path("content/new.bin");
    std::remove(fileName);

    {
        OsFile file(fileName, FileSystem::Mode::WRITE | FileSystem::Mode::CREATE, false);
        char data[256] = {0};
        std::memset(data, 'a', sizeof(data));
        file.write(data, sizeof(data));
    }

    {
        OsFile file(fileName, FileSystem::Mode::WRITE | FileSystem::Mode::APPEND | FileSystem::Mode::READ, false);
        char data[256] = {0};
        std::memset(data, 'b', sizeof(data));
        file.write(data, sizeof(data));

        file.seekToEnd();
        BOOST_CHECK(file.tell() == 512);
        file.seek(0);

        char allData[512] = {0};
        file.read(allData, 512);
        BOOST_CHECK(allData[0] = 'a');
        BOOST_CHECK(allData[255] = 'a');
        BOOST_CHECK(allData[256] = 'b');
        BOOST_CHECK(allData[511] = 'b');
    }
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()


