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

json converter(no* quadtree) {
    json j;
    j["x"] = quadtree->x;
    j["y"] = quadtree->y;
    j["tamanho"] = quadtree->tamanho;
    j["folha"] = quadtree->folha;

    if (quadtree->folha) {
        j["cor"] = quadtree->cor;
    } else {
        j["filhos"] = json::array();
        for (int i = 0; i < 4; i++) {
            if (quadtree->filho[i] != nullptr)
                j["filhos"].push_back(converter(quadtree->filho[i]));
            else
                j["filhos"].push_back(nullptr);
        }
    }
    return j;
}
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
/*vector<vector<unsigned char>> reconstruir_imagem(no* raiz){
     if(raiz->folha){


     }
     else{
        for(int i = 0;i < 4; i++)
            reconstruir_imagem (raiz->filho[i]);   

     }

}
*/