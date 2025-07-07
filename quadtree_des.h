#include<iostream>
#include "json.hpp"
#include <opencv2/opencv.hpp>

using json = nlohmann::json;
using namespace std;


struct no
{
    int x, y;
    int tamanho;
    unsigned char cor;
    bool folha;
    no *filho[4];
    no()
    {
        x = 0;
        y = 0;
        tamanho = 0;
        cor = 0;
        folha = false;
        for (int i = 0; i < 4; i++)
            filho[i] = nullptr;
    }
};

no* reconstruir_quadtree(const json& j) {
    no* node = new no();
    node->x = j["x"];
    node->y = j["y"];
    node->tamanho = j["tamanho"];
    node->folha = j["folha"];

    if (node->folha) {
        node->cor = j["cor"];
    } else {
        for (int i = 0; i < 4; i++) {
            if (!j["filhos"][i].is_null()) {
                node->filho[i] = reconstruir_quadtree(j["filhos"][i]);
            }
        }
    }
    return node;
}

void preencher_imagem(no* raiz, vector<vector<unsigned char>>& imagem) {
    if (raiz->folha) {
        for (int i = raiz->x; i < raiz->x + raiz->tamanho; i++) {
            for (int j = raiz->y; j < raiz->y + raiz->tamanho; j++) {
                imagem[i][j] = raiz->cor;
            }
        }
    } else {
        for (int i = 0; i < 4; i++) {
            preencher_imagem(raiz->filho[i], imagem);
        }
    }
}
vector<vector<unsigned char>> reconstruir_imagem(no* raiz) {
    vector<vector<unsigned char>> imagem(raiz->tamanho, vector<unsigned char>(raiz->tamanho, 0));
    preencher_imagem(raiz, imagem);
    return imagem;
}
void deletar_quadtree(no* raiz) {
    if (raiz == nullptr) return;

    if (!raiz->folha) {
        for (int i = 0; i < 4; i++) {
            deletar_quadtree(raiz->filho[i]);
        }
    }

    delete raiz;
}

