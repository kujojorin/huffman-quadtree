#include "bitwrite.hpp"
#include <iostream>

using namespace std;

bitwrite::bitwrite(std::ofstream &saida) : saida(saida) {}

bitwrite::~bitwrite() {
    flush();
}

void bitwrite::escreveBit(bool bit) {
    buffer = (buffer << 1) | (bit ? 1 : 0);
    if (++n == 8) {
        flush();
    }
}

void bitwrite::escreveByte(uint8_t b) {
    if (n == 0) {
        saida.put(char(b));
    } else {
        for (int i = 7; i >= 0; --i) {
            escreveBit((b >> i) & 1);
        }
    }
}

void bitwrite::flush() {
    if (n > 0) {
        buffer <<= (8 - n);
        saida.put(char(buffer));
        buffer = 0;
        n = 0;
    }
}