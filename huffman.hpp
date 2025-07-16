#ifndef HUFFMAN_HPP
#define HUFFMAN_HPP

#include <string>
#include <vector>
#include <queue>
#include <map>
#include <fstream>
#include <functional> // Incluindo a biblioteca para std::function

class huffman {
private:
    struct No {
        char dado;
        int frequencia;
        No *esquerda, *direita;

        No(char dado, int frequencia) : dado(dado), frequencia(frequencia), esquerda(nullptr), direita(nullptr) {}
    };

    struct comparar {
        bool operator()(No* a, No* b) {
            return a->frequencia > b->frequencia;
        }
    };

    No* arvore;
    std::map<char, std::string> codigos;
    std::map<std::string, char> codigos_invertidos;

    void gerar_codigos(No* raiz, std::string codigo);
    void construir_arvore(const std::map<char, int>& frequencias);
    void salvar_arvore(std::ofstream& arquivo_saida, No* raiz);
    No* ler_arvore(std::ifstream& arquivo_entrada);
    void limpar_arvore(No* raiz);

public:
    huffman();
    ~huffman();

    // Alterando os métodos para aceitarem um callback de progresso
    void compressao(const std::string& arquivo_entrada, const std::string& arquivo_saida, const std::function<void(double)>& on_progress = nullptr);
    void expandir(const std::string& arquivo_entrada, const std::string& arquivo_saida, const std::function<void(double)>& on_progress = nullptr);
};

#endif // HUFFMAN_HPP