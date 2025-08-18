#include "utility.hpp"

bool hasBit(int a, int b) { return (a & b) == b; }
void setBit(int& a, int b) { a |= b; }
void removeBit(int& a, int b) { a &= ~b; }
void excludeBits(int& a, int b) { a &= b; }
