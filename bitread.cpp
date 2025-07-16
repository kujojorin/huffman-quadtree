#include "bitread.hpp"
#include <iostream>

using namespace std;

bitread::bitread(std::ifstream& in) : in(in){} // Implementação do construtor
bitread::~bitread(){} // Implementação do destrutor

bool bitread::leBit(){ // Implementação do método
    if (n == 8)
    {
        int v = in.get();
        if (v == EOF) throw runtime_error("EOF bit read");
        buffer = uint8_t(v);
        n = 0;
    }
    bool bit = (buffer >> (7 -n)) & 1;
    ++n;

    return bit;
}

uint8_t bitread::leByte(){ // Implementação do método
    uint8_t b = 0;
    for (int i = 0; i < 8; i++)
    {
        b = (b << 1) | (leBit() ? 1 : 0);
    }
    return b;
}