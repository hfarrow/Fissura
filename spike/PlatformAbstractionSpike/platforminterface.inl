template<int PlatformID>
void PlatformInterface<PlatformID>::function1()
{
	FS_TRACE("DEFAULT function1()");
}

template<int PlatformID>
int PlatformInterface<PlatformID>::function2(int arg)
{
	FS_TRACE_FORMATTED("DEFAULT function2(%i)", arg);
	return arg;
}

/*
template<int PlatformID>
void PlatformInterface<PlatformID>::function3()
{
	FS_TRACE("DEFAULT function3()");
}
*/