#include <boost/test/unit_test.hpp>

#include <SDL.h>
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

struct FileSystemFixture
{
    FileSystemFixture() :
        area(FS_SIZE_OF_MB * 4),
        arena(area, "FileArena")
    {
    }

    ~FileSystemFixture()
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
BOOST_FIXTURE_TEST_SUITE(file_system, FileSystemFixture)

BOOST_AUTO_TEST_CASE(create_file_system)
{
    {
        FileSystem<FileArena> filesys(&arena);
    }
}

BOOST_AUTO_TEST_CASE(mount_disk_device_and_open_file_and_unmount)
{
    FileSystem<FileArena> filesys(&arena);
    DiskDevice<FileArena> device(&filesys);

    BOOST_CHECK(!filesys.isMounted(&device));
    filesys.mount(&device);
    BOOST_CHECK(filesys.isMounted(&device));

    {
        auto file = filesys.open("disk", path("content/small.bin"), IFileSystem::Mode::READ | IFileSystem::Mode::TEXT);
        BOOST_REQUIRE(file);
        BOOST_REQUIRE(file->opened());
    }

    filesys.unmount(&device);
    BOOST_CHECK(!filesys.isMounted(&device));
}

BOOST_AUTO_TEST_CASE(pref_device_piggyback)
{
    char *base_path = SDL_GetPrefPath("Fissura", "fsio-test");
    BOOST_REQUIRE(base_path);

    FileSystem<FileArena> filesys(&arena);
    DiskDevice<FileArena> disk(&filesys);
    PrefDevice pref(&filesys, base_path);

    filesys.mount(&disk);
    filesys.mount(&pref);

    {
        SharedPtr<IFile> file = filesys.open("pref:disk", "prefFile.bin", IFileSystem::Mode::READ | IFileSystem::Mode::WRITE | IFileSystem::Mode::CREATE);
        BOOST_REQUIRE(file);
        BOOST_REQUIRE(file->opened());
    }

    filesys.unmount(&pref);
    filesys.unmount(&disk);

    SDL_free(base_path);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()


