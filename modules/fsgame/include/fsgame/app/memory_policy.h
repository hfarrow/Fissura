#ifndef FS_STANDARD_MEMORY_POLICY_H
#define FS_STANDARD_MEMORY_POLICY_H

#include "fscore.h"

namespace fs
{
    class ILayoutPolicy
    {
    public:
        virtual void init() = 0;
        virtual void destroy() = 0;
    };
    
    /*
     * This policy provides a main HeapAllocator and a debug HeapAllocator that are thread 
     * safe and backed by a single PageAllocator
     */
    class StandardLayoutPolicy : public ILayoutPolicy, Uncopyable
    {
    public:
        
        StandardLayoutPolicy() :
            gpGeneralPage(nullptr),
            gpGeneralPageTS(nullptr),
            gpFsMainHeap(nullptr),
            gpFsMainHeapTS(nullptr)
#ifdef _DEBUG
            ,gpFsDebugHeap(nullptr),
            gpFsDebugHeapTS(nullptr)
#endif
            
        {
        
        }

        virtual void init() override
        {
            // Set up checks for memory leaks.
#ifdef WINDOWS
//#ifdef _DEBUG
            int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

            // set this flag to keep memory blocks around
            // tmpDbgFlag |= _CRTDBG_DELAY_FREE_MEM_DF;	       // this flag will cause intermittent pauses in your game and potientially cause it to run out of memory!

            // perform memory check for each alloc/dealloc
            //tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;		   // remember this is VERY VERY SLOW!

            //_CRTDBG_LEAK_CHECK_DF is used at program initialization to force a 
            //   leak check just before program exit. This is important because
            //   some classes may dynamically allocate memory in globally constructed
            //   objects.
            tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;					

            _CrtSetDbgFlag(tmpDbgFlag);
//#endif
#endif
            
            static u8 pAllocatorMemory[sizeof(PageAllocator) + sizeof(ThreadSafeAllocator) + (2 * sizeof(HeapAllocator))];


            u8* pNextAllocation = pAllocatorMemory;
            gpGeneralPage = new(pNextAllocation) PageAllocator("gpGeneralPage");
            pNextAllocation += u8(sizeof(PageAllocator));
            gpGeneralPageTS = new(pNextAllocation) ThreadSafeAllocator("gpGeneralPageTS", *gpGeneralPage);
#ifdef _DEBUG
            pNextAllocation += u8(sizeof(ThreadSafeAllocator));
            gpFsDebugHeap = new(pNextAllocation) HeapAllocator("gpFsDebugHeap", *gpGeneralPageTS);
            gpFsDebugHeapTS = FS_NEW_WITH(ThreadSafeAllocator, gpFsDebugHeap)
                ("gpFsDebugHeapTS", *gpFsDebugHeap);
            Memory::setDefaultDebugAllocator(gpFsDebugHeapTS);
#endif
            pNextAllocation += u8(sizeof(HeapAllocator));
            gpFsMainHeap = new(pNextAllocation) HeapAllocator("gpFsMainHeap", *gpGeneralPageTS);
            gpFsMainHeapTS = FS_NEW_WITH(ThreadSafeAllocator, gpFsMainHeap)
                ("gpFsMainHeapTS", *gpFsMainHeap);
            Memory::setDefaultAllocator(gpFsMainHeapTS);


#ifdef _DEBUG
            Memory::initTracker();
            MemoryTracker* pTracker = Memory::getTracker();
            FS_ASSERT(pTracker);
            // Normally allocators automatically register themselves but these
            // allocators must be created before the tracker is initialized
            // because the tracker init needs to allocate memory from debug heap.
            pTracker->registerAllocator(gpGeneralPage);
            pTracker->registerAllocator(gpGeneralPageTS);
            pTracker->registerAllocator(gpFsDebugHeap);
            pTracker->registerAllocator(gpFsDebugHeapTS);
            pTracker->registerAllocator(gpFsMainHeap);
            pTracker->registerAllocator(gpFsMainHeapTS);
#endif
        }

        virtual void destroy() override
        {
#ifdef _DEBUG
            Memory::destroyTracker();
#endif

            FS_DELETE_WITH(gpFsMainHeapTS, gpFsMainHeap);
            static_cast<HeapAllocator*>(gpFsMainHeap)->~HeapAllocator();
#ifdef _DEBUG
            FS_DELETE_WITH(gpFsDebugHeapTS, gpFsDebugHeap);
            static_cast<HeapAllocator*>(gpFsDebugHeap)->~HeapAllocator();
#endif
            gpGeneralPageTS->~ThreadSafeAllocator();
            gpGeneralPage->~PageAllocator();
        }

    private:
        PageAllocator* gpGeneralPage;
        ThreadSafeAllocator* gpGeneralPageTS;
        Allocator* gpFsMainHeap;
        Allocator* gpFsMainHeapTS;
#ifdef _DEBUG
        Allocator* gpFsDebugHeap;
        Allocator* gpFsDebugHeapTS;
#endif
    };
}

#endif
