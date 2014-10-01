#include <cstdio>
#include <boost/test/unit_test.hpp>

#include "global_fixture.h"
#include "fstest.h"
#include "fscore.h"
#include "fsio.h"

using namespace fs;

using FileArena = MemoryArena<Allocator<HeapAllocator, AllocationHeaderU32>,
                                MultiThread<MutexPrimitive>,
                                SimpleBoundsChecking,
                                FullMemoryTracking,
                                MemoryTagging>;

struct DiskDeviceFixture
{
    DiskDeviceFixture() :
        area(FS_SIZE_OF_MB * 4),
        arena(area, "FileArena")
    {
    }

    ~DiskDeviceFixture()
    {
    }

    const char* path(const char* path)
    {
        _temp = GlobalFixture::instance()->path(path);
        return _temp.c_str();
    }

    HeapArea area;
    FileArena arena;

    std::string _temp;
};


BOOST_AUTO_TEST_SUITE(io)
BOOST_FIXTURE_TEST_SUITE(disk_device, DiskDeviceFixture)

BOOST_AUTO_TEST_CASE(create_device)
{
    FileSystem<FileArena> filesys(&arena);
    DiskDevice<FileArena> device(&filesys);
    BOOST_CHECK(device.getType() == "disk");
}

BOOST_AUTO_TEST_CASE(open_file_and_close)
{
    FileSystem<FileArena> filesys(&arena);
    DiskDevice<FileArena> device(&filesys);

    auto file = device.open(nullptr, path("content/empty.bin"), IFileSystem::Mode::READ);
    BOOST_REQUIRE(file);
    BOOST_REQUIRE(file->opened());

    filesys.close(file);
    BOOST_REQUIRE(!file->opened());

}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

