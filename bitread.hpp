#ifndef BITREAD_H
#define BITREAD_H

#include <stdio.h>
#include <string>
#include <vector>
#include <cstdint> // Para uint8_t
#include <queue>
#include <fstream>
#include <stdexcept> // Para std::runtime_error

class bitread
{
private:
   std::ifstream& in; // std::explicitamente
   uint8_t buffer = 0;
   int n = 8;
public:
    // declaração do construtor
    bitread(std::ifstream& in);
    // declaração do destrutor
    ~bitread(); 
    // declaração dos métodos
    bool leBit();
    uint8_t leByte();
};

#endif // BITREAD_H