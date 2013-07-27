// dlmallocSpike.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"

#include <core/types.h>
#include <dlmalloc/malloc.h>
#include <stdio.h>
#include <malloc.h>

using namespace fissura;

int _tmain(int argc, _TCHAR* argv[])
{
	u8* pMem[32 * 1000];
	mspace space = create_mspace_with_base((void*)pMem, 32 * 1000, 0);
	void* alloc = mspace_malloc(space, 32);
	size_t amount = destroy_mspace(space);

	return 0;
}

