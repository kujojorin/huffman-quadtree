#include <stdio.h>
#include <string>
#include <vector>
#include <cstdint>
#include <queue>
#include <fstream>

using namespace std;

class bitwrite
{
private:
    ofstream &saida;
    uint8_t buffer = 0;
    int n = 0;

public:
    bitwrite(ofstream &saida) : saida(saida) {};
    ~bitwrite() { flush(); };
    void escreveBit(bool bit)
    {
        buffer = (buffer << 1) | (bit ? 1 : 0);
        if (++n == 8)
        {
            flush();
        }
    };
    void escreveByte(uint8_t b)
    {
        if (n == 0)
        {
            saida.put(char(b));
        }
        else
        {
            for (int i = 7; i >= 0; --i)
            {
                escreveBit((b >> i) & 1);
            }
        }
    };
    void flush()
    {
        if (n > 0)
        {
            buffer <<= (8 - n);
            saida.put(char(buffer));
            buffer = 0;
            n = 0;
        }
    };
};
