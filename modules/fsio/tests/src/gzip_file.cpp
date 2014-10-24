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

struct GzipFileFixture
{
    GzipFileFixture() :
        area(FS_SIZE_OF_MB * 4),
        arena(area, "FileArena")
    {
    }

    ~GzipFileFixture()
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
BOOST_FIXTURE_TEST_SUITE(gzip_file, GzipFileFixture)

BOOST_AUTO_TEST_CASE(open_gzip_small_file_for_read)
{
    FileSystem<FileArena> filesys(&arena);
    DiskDevice disk;
    GzipFile file(disk.open(&filesys, nullptr, path("content/text.txt.gz"), IFileSystem::Mode::READ),
            &filesys, IFileSystem::Mode::READ);

    BOOST_CHECK(file.opened());
    BOOST_CHECK(file.tell() == 0);

    char buffer[1024] = {0};
    auto size = file.read(buffer, sizeof(buffer));

    BOOST_REQUIRE(size > 0);
    BOOST_REQUIRE(size < sizeof(buffer));

    char expectedContents[sizeof(buffer)];
    auto expectedFile = disk.open(&filesys, nullptr, path("content/text.txt"), IFileSystem::Mode::READ);

    BOOST_REQUIRE(expectedFile && expectedFile->opened());
    expectedFile->read(expectedContents, sizeof(expectedContents));
    BOOST_REQUIRE(strncmp(buffer, expectedContents, strlen(expectedContents)) == 0);
}

BOOST_AUTO_TEST_CASE(open_gzip_large_file_for_small_reads)
{
    FileSystem<FileArena> filesys(&arena);
    DiskDevice disk;
    GzipFile file(disk.open(&filesys, nullptr, path("content/large_data.bin.gz"), IFileSystem::Mode::READ),
                  &filesys, IFileSystem::Mode::READ);

    auto expectedFile = disk.open(&filesys, nullptr, path("content/large_data.bin"), IFileSystem::Mode::READ);

    BOOST_CHECK(file.opened());
    BOOST_CHECK(expectedFile->opened());

    char buffer[1024] = {0};
    char expectedContents[sizeof(buffer)];
    BOOST_REQUIRE(sizeof(buffer) == sizeof(expectedContents));

    size_t size;
    do
    {
        size = file.read(buffer, sizeof(buffer));
        auto expectedSize = expectedFile->read(expectedContents, size);

        // This loop runs many times so don't use BOOST_REQUIRE to avoid extra log spam.
        if(size != expectedSize)
        {
            BOOST_FAIL("size != expectedSize");
        }
        if(strncmp(buffer, expectedContents, size) != 0)
        {
            BOOST_FAIL("strncmp(buffer, expectedContents, size) == 0");
        }
    }
    while(size == sizeof(buffer));
}

BOOST_AUTO_TEST_CASE(open_gzip_large_file_for_large_reads)
{
    FileSystem<FileArena> filesys(&arena);
    DiskDevice disk;
    GzipFile file(disk.open(&filesys, nullptr, path("content/large_data.bin.gz"), IFileSystem::Mode::READ),
                  &filesys, IFileSystem::Mode::READ);

    auto expectedFile = disk.open(&filesys, nullptr, path("content/large_data.bin"), IFileSystem::Mode::READ);

    BOOST_CHECK(file.opened());
    BOOST_CHECK(expectedFile->opened());

    static const size_t chunkSize = 128000 * 1.5; // should be greater than _CHUNK_SIZE in gzip_file.h
    char* buffer = (char*)malloc(chunkSize);
    char* expectedContents = (char*)malloc(chunkSize);

    size_t size;
    do
    {
        size = file.read(buffer, chunkSize);
        auto expectedSize = expectedFile->read(expectedContents, size);

        // This loop runs many times so don't use BOOST_REQUIRE to avoid extra log spam.
        if(size != expectedSize)
        {
            BOOST_FAIL("size != expectedSize");
        }
        if(strncmp(buffer, expectedContents, size) != 0)
        {
            BOOST_FAIL("strncmp(buffer, expectedContents, size) == 0");
        }
    }
    while(size == chunkSize);

    // Did we reach the end of the inflated file?
    auto currentPos = expectedFile->tell();
    expectedFile->seekToEnd();
    auto endPos = expectedFile->tell();
    BOOST_CHECK(currentPos == endPos);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()




