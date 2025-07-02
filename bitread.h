#include <stdio.h>
#include <string>
#include <vector>
#include <cstdint>
#include <queue>
#include <fstream>

using namespace std;

class bitread
{
private:
   ifstream& in;
   uint8_t buffer = 0;
   int n = 8;
public:
    bitread(ifstream& in) : in(in){};
    ~bitread(){};
    bool leBit(){
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
    };
    uint8_t leByte(){
        uint8_t b = 0;
        for (int i = 0; i < 8; i++)
        {
            b = (b << 1) | (leBit() ? 1 : 0);
        }
        
        return b;
    };
};
