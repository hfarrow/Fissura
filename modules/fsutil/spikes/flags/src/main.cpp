#include <iostream>
#include <map>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>

#include "fscore.h"
// #include "fscore/types.h"

using namespace fs;

FS_DECLARE_FLAGS(PlayerFlagsEnum, Dazed, Stunned, Dead, Floating);

using PlayerFlags = Flags<PlayerFlagsEnum>;

void Do(PlayerFlags flags)
{
  PlayerFlags::Description desc = {};
  FS_PRINT("flags: " << flags.toString(desc));
}


int main( int, char **)
{
    PlayerFlags state;

    // // set and remove some flags - type-safe!
    // state.set(PlayerFlags::Dazed);
    // state.set(PlayerFlags::Floating);
    // state.remove(PlayerFlags::Dazed);
    // state.set(PlayerFlags::Dead);
    // FS_PRINT("is any set: " <<  state.isAnySet());
    // FS_PRINT("are all set: " <<  state.areAllSet());
    // FS_PRINT("is Dazed: " << state.isSet(PlayerFlags::Dazed));
    // FS_PRINT("is Floating: " << state.isSet(PlayerFlags::Floating));
    // FS_PRINT("is Killed: " << state.isSet(PlayerFlags::Dead));
    //
    // PlayerFlags::Description desc = {};
    // FS_PRINT("flags have the following state: " << state.toString(desc));
    //
    // state.set(PlayerFlags::Dazed);
    // state.set(PlayerFlags::Stunned);
    //
    // // explicit constructors
    // Do(PlayerFlags(PlayerFlags::Dazed) | PlayerFlags(PlayerFlags::Dead));

    // same as enum, works because of operator|
    Do(PlayerFlags::Dazed);

    return 0;
}

