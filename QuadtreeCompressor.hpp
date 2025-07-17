#ifndef QUADTREECOMPRESSOR_HPP
#define QUADTREECOMPRESSOR_HPP

#include <string>
#include <functional>
#include <opencv2/opencv.hpp>
#include "json.hpp"

// Utilizando um alias para a biblioteca JSON, tornando o código mais legível
using json = nlohmann::json;

class QuadtreeCompressor {
private:
    // A estrutura de um nó da árvore, representando um quadrante da imagem.
    struct Node {
        bool is_leaf;          // Indicando se este nó é uma folha (não tem filhos)
        cv::Scalar avg_color;  // Armazenando a cor média deste quadrante
        Node* children[4];     // Ponteiros para os 4 filhos (sub-quadrantes)

        Node() : is_leaf(true) {
            for (int i = 0; i < 4; ++i) children[i] = nullptr;
        }
        // O destrutor é implementado para limpar a memória recursivamente
        ~Node();
    };

    Node* root; // O nó raiz da nossa árvore Quadtree

    // --- Métodos Privados (Lógica Interna) ---

    // Construindo a árvore recursivamente a partir de uma região da imagem
    void build_tree(Node* node, const cv::Mat& image_roi, int tolerance);

    // Liberando a memória de todos os nós da árvore
    void delete_tree(Node* node);

    // Convertendo a árvore de nós em um objeto JSON para ser salvo
    json save_tree_to_json(Node* node);

    // Reconstruindo a imagem recursivamente a partir da estrutura JSON
    void reconstruct_image(cv::Mat& image, const json& j_node, int x, int y, int size);

public:
    QuadtreeCompressor();
    ~QuadtreeCompressor();

    // --- Métodos Públicos (Interface da Classe) ---

    // Comprimindo uma imagem de um arquivo de entrada para um arquivo de saída JSON
    bool compress_image(const std::string& input_path, const std::string& output_path, int tolerance, std::string& error_msg, const std::function<void(double)>& on_progress = nullptr);
    
    // Descomprimindo um arquivo JSON para gerar uma imagem PNG
    bool decompress_image(const std::string& input_path, const std::string& output_path, std::string& error_msg, const std::function<void(double)>& on_progress = nullptr);
};

#endif // QUADTREECOMPRESSOR_HPP