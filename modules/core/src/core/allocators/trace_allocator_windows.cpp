#include "stdafx.h"

#include <Windows.h>

#include "core/allocators/trace_allocator.h"
#include "windows/allocators/trace_allocator.h"
#include "core/allocators/heap_allocator.h"
#include "core/globals.h"
#include "core/assert.h"
#include "core/trace.h"
#include "core/util.h"

// windows includes
#include <DbgHelp.h>
#pragma comment(lib,"dbghelp.lib")

using namespace fs;

TraceAllocator::TraceAllocator(const fschar* const  pName, Allocator& allocator)
	: ProxyAllocator(pName, allocator)
{
	// gpDebugHeap must have been provided by application.
	FS_ASSERT(gpDebugHeap != nullptr);

	_pAllocationMap = AllocationMapPointer(FS_NEW(AllocationMap, gpDebugHeap)(*gpDebugHeap),
		[](AllocationMap* p){FS_DELETE(p, gpDebugHeap);});

	SymInitialize(GetCurrentProcess(), NULL, TRUE);
}

TraceAllocator::~TraceAllocator()
{
	if(_pAllocationMap->size() > 0)
	{
		for(auto it = _pAllocationMap->begin(); it != _pAllocationMap->end(); ++it)
		{
			FS_TRACE_ERR_FORMATTED("%s: %s", "TraceAllocator memory leak", getCaller(&it->second));
			FS_ASSERT(!"TraceAllocator memory leak. Please see above trace");
		}
	}
	_pAllocationMap->clear();
}

void* TraceAllocator::allocate(size_t size, u8 alignment)
{
	void* pAllocation = ProxyAllocator::allocate(size, alignment);

	if(pAllocation != nullptr)
	{
		AllocationInfo info;
		getStackTrace(&info, pAllocation);
		_pAllocationMap->insert(std::make_pair((uptr)pAllocation, info));
	}

	return pAllocation;
}

bool TraceAllocator::deallocate(void* p)
{	
	if(!ProxyAllocator::deallocate(p))
		return false;

	_pAllocationMap->erase((uptr)p);
	return true;
}

void TraceAllocator::getStackTrace(AllocationInfo* pInfo, void* pAllocation)
{
	CONTEXT ctx;
	RtlCaptureContext(&ctx);

	STACKFRAME64 stackFrame;
	memset(&stackFrame, 0, sizeof(stackFrame));

#ifdef _M_IX86
	DWORD machineType = IMAGE_FILE_MACHINE_I386;
	stackFrame.AddrPC.Offset = ctx.Eip;
	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Offset = ctx.Ebp;
	stackFrame.AddrFrame.Mode = AddrModeFlat;
	stackFrame.AddrStack.Offset = ctx.Esp;
	stackFrame.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
	DWORD machineType = IMAGE_FILE_MACHINE_AMD64;
	stackFrame.AddrPC.Offset = ctx.Rip;
	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Offset = ctx.Rsp;
	stackFrame.AddrFrame.Mode = AddrModeFlat;
	stackFrame.AddrStack.Offset = ctx.Rsp;
	stackFrame.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
	DWORD machineType = IMAGE_FILE_MACHINE_IA64;
	stackFrame.AddrPC.Offset = ctx.StIIP;
	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Offset = ctx.IntSp;
	stackFrame.AddrFrame.Mode = AddrModeFlat;
	stackFrame.AddrBStore.Offset = ctx.RsBSP;
	stackFrame.AddrBStore.Mode = AddrModeFlat;
	stackFrame.AddrStack.Offset = ctx.IntSp;
	stackFrame.AddrStack.Mode = AddrModeFlat;
#else
	#error "Platform not supported by TraceAllocator!"
#endif

	memset(pInfo->offsets, 0, sizeof(pInfo->offsets));
	for(u32 i = 0; i < AllocationInfo::maxStackFrames; ++i)
	{
		pInfo->offsets[i] = stackFrame.AddrPC.Offset;
		if(!StackWalk64(
			machineType,
			GetCurrentProcess(),
			GetCurrentThread(),
			&stackFrame,
			&ctx,
			NULL,
			SymFunctionTableAccess64,
			SymGetModuleBase64,
			NULL))
		{
			break;
		}
	}

	//UNREFERENCED_PARAMETER(pInfo);
}

const char* TraceAllocator::getCaller(const AllocationInfo* const pInfo) const
{
	const size_t bufferSize = 512;
	char pFileName[bufferSize];
	char pFuncName[bufferSize];
	u32 lineNumber;
	static char pBuffer[bufferSize * 3];

	strcpy_s(pFileName, "??");
	lineNumber = 0;

	// Resolve PC to function names
	DWORD64 offset;
	for(u32 i = 0; i < AllocationInfo::maxStackFrames; ++i)
	{
		// check for end of stack walk
		offset = pInfo->offsets[i];
		if(offset == 0)
			break;

		// get function name
		uchar byBuffer[sizeof(IMAGEHLP_SYMBOL64) + bufferSize];
		IMAGEHLP_SYMBOL64* pSymbol = (IMAGEHLP_SYMBOL64*)byBuffer;
		DWORD64 displacement;
		
		memset(pSymbol, 0, sizeof(IMAGEHLP_SYMBOL64) + bufferSize);
		pSymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
		pSymbol->MaxNameLength = bufferSize;

		if(!SymGetSymFromAddr64(GetCurrentProcess(), offset, &displacement, pSymbol))
		{
			strcpy_s(pFuncName, "??");
		}
		else
		{
			pSymbol->Name[bufferSize-1] = '\0';

			// See if we need to go further up the stack
			if(strncmp(pSymbol->Name, "PMemory::", 9) == 0 ||
			   strncmp(pSymbol->Name, "operator new", 12) == 0 ||
			   strncmp(pSymbol->Name, "std::", 5) == 0 ||
			   strncmp(pSymbol->Name, "fs::internal::TraceAllocator", 28) == 0 ||
			   strncmp(pSymbol->Name, "fs::TraceAllocator", 23) == 0)
			{
				// keep going...
			}
			else
			{
				strcpy_s(pFuncName, pSymbol->Name);
				break;
			}
		}
	}

	// get file/line number
	if(offset != 0)
	{
		IMAGEHLP_LINE64 line;
		DWORD displacement;
		memset(&line, 0, sizeof(line));
		line.SizeOfStruct = sizeof(line);

		if(!SymGetLineFromAddr64(GetCurrentProcess(), offset, &displacement, &line))
		{
			strcpy_s(pFileName, "??");
			lineNumber = 0;
		}
		else
		{
			const char* pFile = strrchr(line.FileName, '\\');
			if(!pFile) pFile = line.FileName;
			else ++pFile;

			strncpy_s(pFileName, pFile, bufferSize);
			lineNumber = line.LineNumber;
		}
	}

	// format into buffer and return
	sprintf_s(pBuffer, "%s:%d(%s)", pFileName, lineNumber, pFuncName);
	return pBuffer;
}
