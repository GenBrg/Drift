// Code from 15-466-f19-base1

#pragma once

#include <string>

//construct a path based on the location of the currently-running executable:
// (e.g. if running /home/ix/game0/game.exe will return '/home/ix/game0/' + suffix)
std::string data_path(std::string const &suffix);
