#ifndef BITWRITE_H
#define BITWRITE_H

#include <fstream>
#include <cstdint> // Para uint8_t

class bitwrite
{
private:
    std::ofstream &saida;
    uint8_t buffer = 0;
    int n = 0;

public:
    // declaração do construtor
    bitwrite(std::ofstream &saida);
    // declaração do destrutor
    ~bitwrite();
    // declaração dos métodos
    void escreveBit(bool bit);
    void escreveByte(uint8_t b);
    void flush();
};

#endif // BITWRITE_H