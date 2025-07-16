#include "huffman.hpp"
#include <iostream>
#include <stdexcept> // Incluindo para std::runtime_error

// ... (Construtor, Destrutor, e métodos de compressão e árvore permanecem os mesmos) ...
huffman::huffman() : arvore(nullptr) {}

huffman::~huffman() {
    limpar_arvore(arvore);
}

void huffman::limpar_arvore(No* raiz) {
    if (raiz) {
        limpar_arvore(raiz->esquerda);
        limpar_arvore(raiz->direita);
        delete raiz;
    }
}

void huffman::gerar_codigos(No* raiz, std::string codigo) {
    if (!raiz) return;
    if (!raiz->esquerda && !raiz->direita) {
        codigos[raiz->dado] = codigo;
        codigos_invertidos[codigo] = raiz->dado;
    }
    gerar_codigos(raiz->esquerda, codigo + "0");
    gerar_codigos(raiz->direita, codigo + "1");
}

void huffman::construir_arvore(const std::map<char, int>& frequencias) {
    std::priority_queue<No*, std::vector<No*>, comparar> fila_prioridade;
    for (auto const& [caractere, freq] : frequencias) {
        fila_prioridade.push(new No(caractere, freq));
    }

    while (fila_prioridade.size() > 1) {
        No* esquerda = fila_prioridade.top();
        fila_prioridade.pop();
        No* direita = fila_prioridade.top();
        fila_prioridade.pop();

        No* novo_no = new No('$', esquerda->frequencia + direita->frequencia);
        novo_no->esquerda = esquerda;
        novo_no->direita = direita;
        fila_prioridade.push(novo_no);
    }
    arvore = fila_prioridade.top();
}


void huffman::salvar_arvore(std::ofstream& arquivo_saida, No* raiz) {
    if (!raiz) return;
    if (!raiz->esquerda && !raiz->direita) {
        arquivo_saida << '1' << raiz->dado;
    } else {
        arquivo_saida << '0';
        salvar_arvore(arquivo_saida, raiz->esquerda);
        salvar_arvore(arquivo_saida, raiz->direita);
    }
}

huffman::No* huffman::ler_arvore(std::ifstream& arquivo_entrada) {
    char bit;
    if (!arquivo_entrada.get(bit)) { // Verificação de segurança
        throw std::runtime_error("Fim inesperado do arquivo ao ler a arvore.");
    }

    if (bit == '1') {
        char dado;
        if (!arquivo_entrada.get(dado)) { // Verificação de segurança
             throw std::runtime_error("Fim inesperado do arquivo ao ler folha da arvore.");
        }
        return new No(dado, 0);
    } else {
        No* no_interno = new No('$', 0);
        no_interno->esquerda = ler_arvore(arquivo_entrada);
        no_interno->direita = ler_arvore(arquivo_entrada);
        return no_interno;
    }
}

void huffman::compressao(const std::string& arquivo_entrada, const std::string& arquivo_saida, const std::function<void(double)>& on_progress) {
    std::ifstream entrada(arquivo_entrada, std::ios::binary);
    if (!entrada) throw std::runtime_error("Não foi possível abrir o arquivo de entrada.");

    if (on_progress) on_progress(0.0);

    std::map<char, int> frequencias;
    char caractere;
    long long tamanho_total = 0;
    entrada.seekg(0, std::ios::end);
    tamanho_total = entrada.tellg();
    entrada.seekg(0, std::ios::beg);
    
    if (tamanho_total == 0) { // Lidando com arquivos vazios
        if (on_progress) on_progress(1.0);
        std::ofstream saida(arquivo_saida, std::ios::binary); // Criando arquivo vazio
        return;
    }

    while (entrada.get(caractere)) {
        frequencias[caractere]++;
    }

    if (on_progress) on_progress(0.1);

    construir_arvore(frequencias);
    codigos.clear();
    codigos_invertidos.clear();
    gerar_codigos(arvore, "");

    if (on_progress) on_progress(0.2);

    entrada.clear();
    entrada.seekg(0);
    std::ofstream saida(arquivo_saida, std::ios::binary);
    if (!saida) throw std::runtime_error("Não foi possível criar o arquivo de saída.");
    
    salvar_arvore(saida, arvore);
    if (on_progress) on_progress(0.3);

    long long bytes_processados = 0;
    std::string buffer_escrita;
    while (entrada.get(caractere)) {
        buffer_escrita += codigos[caractere];
        while (buffer_escrita.length() >= 8) {
            char byte = 0;
            for (int i = 0; i < 8; ++i) {
                if (buffer_escrita[i] == '1') {
                    byte |= (1 << (7 - i));
                }
            }
            saida.put(byte);
            buffer_escrita = buffer_escrita.substr(8);
        }
        
        bytes_processados++;
        if (on_progress && bytes_processados % 1000 == 0) {
            on_progress(0.3 + (0.7 * ((double)bytes_processados / tamanho_total)));
        }
    }

    if (!buffer_escrita.empty()) {
        char byte = 0;
        int bits_restantes = buffer_escrita.length();
        for (int i = 0; i < bits_restantes; ++i) {
            if (buffer_escrita[i] == '1') {
                byte |= (1 << (7 - i));
            }
        }
        saida.put(byte);
    }
    
    if (on_progress) on_progress(1.0);
}


void huffman::expandir(const std::string& arquivo_entrada, const std::string& arquivo_saida, const std::function<void(double)>& on_progress) {
    std::ifstream entrada(arquivo_entrada, std::ios::binary);
    if (!entrada) throw std::runtime_error("Não foi possível abrir o arquivo de entrada.");

    long long tamanho_total = 0;
    entrada.seekg(0, std::ios::end);
    tamanho_total = entrada.tellg();
    entrada.seekg(0, std::ios::beg);

    if (tamanho_total == 0) { // Lidando com arquivos vazios
        if (on_progress) on_progress(1.0);
        std::ofstream saida(arquivo_saida, std::ios::binary);
        return;
    }

    if (on_progress) on_progress(0.0);

    limpar_arvore(arvore);
    arvore = ler_arvore(entrada);
    codigos.clear();
    codigos_invertidos.clear();
    gerar_codigos(arvore, "");

    if (on_progress) on_progress(0.25);

    auto pos_dados = entrada.tellg();
    long long bytes_dados = tamanho_total - (long long)pos_dados;

    std::ofstream saida(arquivo_saida);
    if (!saida) throw std::runtime_error("Não foi possível criar o arquivo de saída.");
    
    No* no_atual = arvore;
    char byte;
    long long bytes_processados = 0;
    while (entrada.get(byte)) {
        for (int i = 7; i >= 0; --i) {
            // Verificação de segurança para o nó atual
            if (!no_atual) {
                throw std::runtime_error("Erro de logica: no atual e nulo. O arquivo pode estar corrompido.");
            }
            
            bool bit = (byte >> i) & 1;
            no_atual = bit ? no_atual->direita : no_atual->esquerda;

            if (no_atual && !no_atual->esquerda && !no_atual->direita) {
                saida << no_atual->dado;
                no_atual = arvore; // Reiniciando a busca na árvore
            }
        }
        bytes_processados++;
        if (on_progress && bytes_processados % 1000 == 0) {
             on_progress(0.25 + (0.75 * ((double)bytes_processados / bytes_dados)));
        }
    }
    
    if (on_progress) on_progress(1.0);
}