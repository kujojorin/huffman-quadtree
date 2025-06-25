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
    int R = 256;
    // estrutura para os nos da arvore
    struct Node
    {
        unsigned char symbol;
        size_t freq; // acumula a frequencia de repeticao de cada char
        Node *esq;   // ponteiro para a subarvore esquerda
        Node *dir;   // ponteiro p a subarvore direita

        static Node *leTrie(ifstream &arquivoEntrada)
        {
            bool ehFolha = bitwrite.readBoolean();
            if (ehFolha)
            {
             return new Node(bitwrite.readChar(), -1, nullptr, nullptr);
            }else{
                return new Node('\0', -1, leTrie(arquivoEntrada), leTrie(arquivoEntrada));
            }
            
        }

        static Node constroiTrie(int freq)
        {
            // inicializando fila de prioridade
            MinPQ<Node> pq = new MinPQ<Node>();

            for (char c = 0; c < huffman::R; c++)
            {
               if (freq[c] > 0)
               {
                pq.insert(new Node(c, freq[c], nullptr, nullptr));
               }
            }

            while (pq.size() > 1)
            {
               Node esq = pq.delMin();
               Node dir = pq.delMin();
               Node pai = new Node('\0', esq.freq + dir.freq, esq, dir);
               pq.insert(pai);
            }
            
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
    void escreveTrie(Node x);
    void constroiTabelaCodigo(Node *x, const string &prefix);
    void constroiTabelaFrequencia(const string &arquivoEntrada);
    void escreveDadoComprimido(const string &arquivoEntrada, ofstream &arquivoSaida);
    void leDadoComprimido(ifstream &arquivoEntrada, ofstream &arquivoSaida);
    void deleteTrie(Node *x);
    void constroiCode(string st, Node x, string s);
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

    in.close();

    constroiTabelaFrequencia(entrada);
    constroiTrie();
    constroiTabelaCodigo(root, "");

    ofstream saida(arquivoSaida, ios::binary);
    bitwrite escritor(saida);

    escreverTrie(escritor, root);

    uint32_t N = entrada.size();
    for (int shift = 24; shift >= 0; shift -= 8)
    {
        escritor.escreveByte((N >> shift) && 0xFF);
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

void huffman::escreveTrie(Node x) {
    if (x.ehFolha())
    {
      bitwrite.write(true);
      bitwrite.write(x.ch, 8);
      return;
    }

    bitwrite.write(false);
    escreveTrie(x.esq);
    escreveTrie(x.dir);
};

void huffman::constroiCode(string st, Node x, string s){
    if (!x.ehFolha())
    {
        constroiCode(st, x.esq, s + '0');
        constroiCode(st, x.dir, s + '1');
    }else{
        st[x.ch] = s;
    }
    
}