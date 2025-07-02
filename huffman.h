#include <stdio.h>
#include <string>
#include <vector>
#include <cstdint>
#include <queue>
#include <fstream>
#include <iterator>

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
        unsigned char simbolo;
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
            : simbolo(s), freq(f), esq(esquerda), dir(direita) {}

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

    //criando iteradores
    istreambuf_iterator<char> it(in);
    istreambuf_iterator<char> end;

    vector<unsigned char> entrada(it, end);
                                  
    in.close(); //fechando arquivo de entrada pois nao precisa mais dele
    TabelaDeFrequencia.assign(R,0);

    for (unsigned char c : entrada)
    {
        TabelaDeFrequencia[c]++;
    }

    root = Node::constroiTrie(TabelaDeFrequencia);

    TabelaDeCodigos.assign(R, "");
    constroiTabelaCodigo(root, "");

    //abre o arquivo de saida e inicia o escritor de bits
    ofstream saida(arquivoSaida, ios::binary);
    bitwrite escritor(saida);

    //serializa a trie no inicio do arquivo
    escreveTrie(escritor, root);

    //escreve o tamanho original
    uint32_t N = entrada.size();
    for (int shift = 24; shift >= 0; shift -= 8)
    {
        escritor.escreveByte((N >> shift) & 0xFF);
    }

    //escreve os dados comprimidos bit a bit
    for (unsigned char c : entrada)
    {
        const string &code = TabelaDeCodigos[c];
        for (char bit : code)
        {
            escritor.escreveBit(bit == '1');
        }
    }

    escritor.flush();
}

void huffman::expandir(string &arquivoEntrada, string &arquivoSaida)
{
    //abre o arquivo ja comprimido
    ifstream in(arquivoEntrada, ios::binary);
    bitread leitor(in);

    Node *root = Node::leTrie(leitor);

    //le o tamanho original de 4 bytes
    uint32_t n = 0;
    for (int i = 0; i < 4; ++i)
    {
        n = (n << 8) | leitor.leByte();
    }

    //abre o arquivo de saida
    ofstream saida(arquivoSaida, ios::binary);

    //percorre a arvore bit a bit
    for (uint32_t i = 0; i < n; ++i)
    {
        Node *x = root;

        while (!x->ehFolha())
        {
            bool b = leitor.leBit();
            x = (b ? x->dir : x->esq);
        }
        saida.put(char(x->simbolo));
    }
    saida.close();
    in.close();
};

void huffman::escreveTrie(bitwrite &escritor, Node *x)
{
    if (x->ehFolha())
    {
        escritor.escreveBit(true);
        escritor.escreveByte(x->simbolo);
    }
    else
    {
        escritor.escreveBit(false);
        escreveTrie(escritor, x->esq);
        escreveTrie(escritor, x->dir);
    }
};

void huffman::constroiTabelaFrequencia(const vector<unsigned char> &entrada)
{
    //garante que TabelaDeFrequencia tem tamanho R e todos os zero
    TabelaDeFrequencia.assign(R, 0);

    // conta as ocorrências de cada símbolo (0..255)
    for (unsigned char c : entrada) {
        TabelaDeFrequencia[c]++;
    }
    
}
void huffman::constroiTabelaCodigo(Node *x, const string &prefix)
{
    if (x->ehFolha())
    {
        TabelaDeCodigos[x->simbolo] = prefix;
        return;
    }
    constroiTabelaCodigo(x->esq, prefix+ '0');
    constroiTabelaCodigo(x->dir, prefix + '1');
}