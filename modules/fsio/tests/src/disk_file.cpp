#include <cstdio>
#include <boost/test/unit_test.hpp>

#include "global_fixture.h"
#include "fstest.h"
#include "fscore.h"
#include "fsio.h"

using namespace fs;

struct DiskFileFixture
{
    DiskFileFixture()
    {
    }

    ~DiskFileFixture()
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
BOOST_FIXTURE_TEST_SUITE(disk_file, DiskFileFixture)

BOOST_AUTO_TEST_CASE(open_osfile)
{
    DiskFile file(path("content/empty.bin"), IFileSystem::Mode::READ);

    BOOST_CHECK(file.opened());
    BOOST_CHECK(file.tell() == 0);
    file.seekToEnd();
    BOOST_CHECK(file.tell() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()



