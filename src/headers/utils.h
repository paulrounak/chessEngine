#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "move.h"

std::string squareToString(int square);
std::string moveToUCI(const Move& move);

#endif