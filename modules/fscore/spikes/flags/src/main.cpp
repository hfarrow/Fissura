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

using namespace fs;

FS_DECLARE_FLAGS(PlayerState, Dazed, Stunned, Killed, Floating);

using PlayerFlags = Flags<PlayerState>;

void Do(PlayerFlags flags)
{
  PlayerFlags::Description desc = {};
  FS_PRINT("flags: " << flags.toString(desc));
}


int main( int, char **)
{
    PlayerFlags state;

// set and remove some flags - type-safe!
state.set(PlayerState::Dazed);
state.set(PlayerState::Floating);
state.remove(PlayerState::Dazed);
state.set(PlayerState::Killed);
FS_PRINT("is any set: " <<  state.isAnySet());
FS_PRINT("are all set: " <<  state.areAllSet());
FS_PRINT("is Dazed: " << state.isSet(PlayerState::Dazed));
FS_PRINT("is Floating: " << state.isSet(PlayerState::Floating));
FS_PRINT("is Killed: " << state.isSet(PlayerState::Killed));

PlayerFlags::Description desc = {};
FS_PRINT("flags have the following state: " << state.toString(desc));

state.set(PlayerState::Dazed);
state.set(PlayerState::Stunned);

// explicit constructors
Do(PlayerFlags(PlayerState::Dazed) | PlayerFlags(PlayerState::Killed));

// same as enum, works because of operator|
Do(PlayerState::Dazed | PlayerState::Killed);

    return 0;
}

