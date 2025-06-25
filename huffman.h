#include <stdio.h>
#include <string>
#include <vector>
#include <cstdint>
#include <queue>
#include <fstream>

using namespace std;

#include "bitwrite.h"
#include "bitread.h"

class huffman
{
private:
    static constexpr int R = 256;

    // estrutura para os nos da arvore
    struct Node
    {
        unsigned char symbol;
        size_t freq; // acumula a frequencia de repeticao de cada char
        Node *esq;   // ponteiro para a subarvore esquerda
        Node *dir;   // ponteiro p a subarvore direita

        static Node *leTrie(bitread &reader)
        {
            bool ehFolha = reader.leBit();
            if (ehFolha)
            {
                unsigned char c = reader.leByte();
                return new Node(c, 0, nullptr, nullptr);
            }
            else
            {
                Node *l = leTrie(reader);
                Node *r = leTrie(reader);
                return new Node('\0', 0, l, r);
            }
        }

        static Node *constroiTrie(const vector<size_t> &freq)
        {
            auto cmp = [](Node *a, Node *b)
            { return a->freq > b->freq; };
            priority_queue<Node *, vector<Node *>, decltype(cmp)> pq(cmp);

            for (int c = 0; c < huffman::R; c++)
            {
                if (freq[c] > 0)
                    pq.push(new Node((unsigned char)c, freq[c], nullptr, nullptr));
            }
            // Se só tiver 1 símbolo
            if (pq.size() == 1)
                pq.push(new Node('\0', 0, nullptr, nullptr));

            while (pq.size() > 1)
            {
                Node *a = pq.top();
                pq.pop();
                Node *b = pq.top();
                pq.pop();
                pq.push(new Node('\0', a->freq + b->freq, a, b));
            }
            return pq.top();
        }

        Node(unsigned char s, size_t f, Node *esquerda = nullptr, Node *direita = nullptr)
            : symbol(s), freq(f), esq(esquerda), dir(direita) {}

        bool ehFolha() const
        {
            return esq == nullptr && dir == nullptr;
        }

        int Compare(Node outro)
        {
            return this->freq - outro.freq;
        }
    };

    Node *root;
    vector<string> TabelaDeCodigos;
    vector<size_t> TabelaDeFrequencia;
    void escreveTrie(bitwrite &writer, Node *x);
    void constroiCode(string st, Node x, string s);

    void constroiTabelaFrequencia(const vector<unsigned char> &entrada);
    void constroiTabelaCodigo(Node *x, const string &prefix);

public:
    huffman(/* args */);
    ~huffman();
    void compressao(string &arquivoEntrada, string &arquivoSaida);
    void expandir(string &arquivoEntrada, string &arquivoSaida);
};

huffman::huffman(/* args */)
{
}

huffman::~huffman()
{
}

void huffman::compressao(string &arquivoEntrada, string &arquivoSaida)
{
    ifstream in(arquivoEntrada, ios::binary);

    vector<unsigned char> entrada(istreambuf_iterator<char>(in),
                                  istreambuf_iterator<char>());

    TabelaDeFrequencia.assign(R,0);
    for (unsigned char c : entrada)
    {
        TabelaDeFrequencia[c]++;
    }

    root = Node::constroiTrie(TabelaDeFrequencia);

    TabelaDeCodigos.assign(R, "");
    constroiTabelaCodigo(root, "");

    ofstream saida(arquivoSaida, ios::binary);
    bitwrite escritor(saida);

    escreveTrie(escritor, root);

    uint32_t N = entrada.size();
    for (int shift = 24; shift >= 0; shift -= 8)
    {
        escritor.escreveByte((N >> shift) & 0xFF);
    }

    for (unsigned char c : entrada)
    {
        const string &code = TabelaDeCodigos[c];
        for (char bit : code)
        {
            escritor.escreveBit(bit == '1');
        }
    }

    saida.close();
}

void huffman::expandir(string &arquivoEntrada, string &arquivoSaida)
{
    ifstream in(arquivoEntrada, ios::binary);
    bitread leitor(in);

    Node *root = leTrie(leitor);

    uint32_t n = 0;
    for (int i = 0; i < 4; ++i)
    {
        n = (n << 8) | leitor.leByte();
    }

    ofstream saida(arquivoSaida, ios::binary);

    for (uint32_t i = 0; i < n; ++i)
    {
        Node *x = root;

        while (!x->ehFolha())
        {
            bool b = leitor.leBit();
            x = (b ? x->dir : x->esq);
        }
        saida.put(char(x->symbol));
    }
    saida.close();
    in.close();
};

void huffman::escreveTrie(bitwrite &escritor, Node *x)
{
    if (x->ehFolha())
    {
        escritor.escreveBit(true);
        escritor.escreveByte(x->symbol);
    }
    else
    {
        escritor.escreveBit(false);
        escreveTrie(escritor, x->esq);
        escreveTrie(escritor, x->dir);
    }
};

void huffman::constroiCode(string st, Node x, string s)
{
    if (!x.ehFolha())
    {
        constroiCode(st, x.esq, s + '0');
        constroiCode(st, x.dir, s + '1');
    }
    else
    {
        st[x.ch] = s;
    }
}

void huffman::constroiTabelaFrequencia(const vector<unsigned char> &entrada)
{
    
    
}
void huffman::constroiTabelaCodigo(Node *x, const string &prefix)
{
    if (x->ehFolha())
    {
        TabelaDeCodigos[x->symbol] = prefix;
        return;
    }
    constroiTabelaCodigo(x->esq, prefix+ '0');
    constroiTabelaCodigo(x->dir, prefix + '1');
}