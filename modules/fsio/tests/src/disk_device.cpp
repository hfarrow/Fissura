#include <cstdio>
#include <boost/test/unit_test.hpp>

#include "global_fixture.h"
#include "fstest.h"
#include "fscore.h"
#include "fsmem.h"
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
        arena(area, "FileArena"),
        gf(GlobalFixture::instance())
    {
    }

    ~DiskDeviceFixture()
    {
    }

    HeapArea area;
    FileArena arena;
    GlobalFixture* gf;
};


BOOST_AUTO_TEST_SUITE(io)
BOOST_FIXTURE_TEST_SUITE(disk_device, DiskDeviceFixture)

BOOST_AUTO_TEST_CASE(create_device)
{
    FileSystem<FileArena> filesys(&arena);
    DiskDevice device;
    BOOST_CHECK(device.getType() == "disk");
}

BOOST_AUTO_TEST_CASE(open_file_and_close)
{
    FileSystem<FileArena> filesys(&arena);
    DiskDevice device;

    auto file = device.open(&filesys, nullptr, gf->path("content/empty.bin"), IFileSystem::Mode::READ);
    BOOST_REQUIRE(file);
    BOOST_REQUIRE(file->opened());
    BOOST_CHECK(strcmp(file->getName(), gf->path("content/empty.bin")) == 0);

    filesys.close(file);
    BOOST_REQUIRE(!file->opened());

}

BOOST_AUTO_TEST_CASE(open_with_invalid_device_list)
{
    FileSystem<FileArena> filesys(&arena);
    DiskDevice device;

    auto lambda = [&]()
    {
        auto file = device.open(&filesys, "some:other:devices", gf->path("content/empty.bin"), IFileSystem::Mode::READ);
    };

    FS_REQUIRE_ASSERT(lambda);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

