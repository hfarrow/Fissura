#include <iostream>

#include "fscore.h"
#include "fsgame.h"

#include "basic_game.h"

using namespace std;
using namespace fs;

int main( int, char **)
{
    Game game;
    GameAppRunner runner;
    return runner.runGameApp();
}
