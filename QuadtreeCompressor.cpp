#include "QuadtreeCompressor.hpp"
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>

QuadtreeCompressor::QuadtreeCompressor() : root(nullptr) {}

QuadtreeCompressor::~QuadtreeCompressor() {
    delete_tree(root);
}

QuadtreeCompressor::Node::~Node() {
    for (int i = 0; i < 4; ++i) {
        delete children[i];
    }
}

void QuadtreeCompressor::delete_tree(Node* node) {
    if (node) {
        delete node; // O destrutor recursivo do nó cuidará da limpeza dos filhos
        root = nullptr;
    }
}

bool QuadtreeCompressor::compress_image(const std::string& input_path, const std::string& output_path, int tolerance, std::string& error_msg, const std::function<void(double)>& on_progress) {
    delete_tree(root);
    if (on_progress) on_progress(0.0);

    // Carregando a imagem original usando OpenCV
    cv::Mat image = cv::imread(input_path);
    if (image.empty()) {
        error_msg = "Não foi possível carregar a imagem: " + input_path;
        return false;
    }

    // --- Lógica de Preenchimento (Padding) ---
    // 1. Armazenando as dimensões originais da imagem. Elas serão salvas no JSON.
    int original_width = image.cols;
    int original_height = image.rows;
    
    // 2. Determinando o tamanho da nova tela quadrada.
    //    Calculando a próxima potência de 2 que seja grande o suficiente para conter a imagem.
    int max_dim = std::max(original_width, original_height);
    int padded_size = 1;
    while (padded_size < max_dim) {
        padded_size *= 2;
    }

    // 3. Criando a nova tela preta (a imagem com preenchimento)
    cv::Mat padded_image = cv::Mat::zeros(padded_size, padded_size, image.type());

    // 4. Copiando a imagem original para o canto superior esquerdo da tela preta.
    //    O resto da tela permanece preto.
    image.copyTo(padded_image(cv::Rect(0, 0, original_width, original_height)));

    if (on_progress) on_progress(0.2);

    // Construindo a árvore a partir da imagem com preenchimento (padded_image)
    root = new Node();
    build_tree(root, padded_image, tolerance);
    
    if (on_progress) on_progress(0.8);

    std::ofstream out(output_path);
    if (!out.is_open()) {
        error_msg = "Não foi possível criar o arquivo de saída JSON.";
        return false;
    }

    // 5. Salvando todas as dimensões necessárias no arquivo JSON.
    json j;
    j["padded"] = padded_size;
    j["largura"] = original_width;
    j["altura"] = original_height;
    j["raiz"] = save_tree_to_json(root);

    // Salvando o JSON de forma compacta (sem indentação) para economizar mais espaço
    out << j.dump();
    
    if (on_progress) on_progress(1.0);

    return true;
}

void QuadtreeCompressor::build_tree(Node* node, const cv::Mat& image_roi, int tolerance) {
    // se o quadrante for metade vermelho e metade azul, a cor média será roxa.
    node->avg_color = cv::mean(image_roi);

    cv::Mat diff;
    cv::absdiff(image_roi, node->avg_color, diff);
    cv::Mat diff_gray;
    cv::cvtColor(diff, diff_gray, cv::COLOR_BGR2GRAY);
    cv::Scalar total_error_scalar = cv::sum(diff_gray);
    double error_per_pixel = total_error_scalar[0] / (image_roi.rows * image_roi.cols);

    if (error_per_pixel > tolerance && image_roi.rows > 1) {
        node->is_leaf = false;
        int half_size = image_roi.rows / 2;
        node->children[0] = new Node();
        build_tree(node->children[0], image_roi(cv::Rect(0, 0, half_size, half_size)), tolerance);
        node->children[1] = new Node();
        build_tree(node->children[1], image_roi(cv::Rect(half_size, 0, half_size, half_size)), tolerance);
        node->children[2] = new Node();
        build_tree(node->children[2], image_roi(cv::Rect(0, half_size, half_size, half_size)), tolerance);
        node->children[3] = new Node();
        build_tree(node->children[3], image_roi(cv::Rect(half_size, half_size, half_size, half_size)), tolerance);
    }
}

json QuadtreeCompressor::save_tree_to_json(Node* node) {
    if (!node) return nullptr;
    
    json j_node;
    if (node->is_leaf) {
        // Salvando as cores
        j_node["r"] = static_cast<int>(node->avg_color[2]);
        j_node["g"] = static_cast<int>(node->avg_color[1]);
        j_node["b"] = static_cast<int>(node->avg_color[0]);
    } else {
        // Usando a chave "filhos"
        j_node["filhos"] = json::array({
            save_tree_to_json(node->children[0]),
            save_tree_to_json(node->children[1]),
            save_tree_to_json(node->children[2]),
            save_tree_to_json(node->children[3])
        });
    }
    return j_node;
}

bool QuadtreeCompressor::decompress_image(const std::string& input_path, const std::string& output_path, std::string& error_msg, const std::function<void(double)>& on_progress) {
    if (on_progress) on_progress(0.0);

    std::ifstream in(input_path);
    if (!in.is_open()) {
        error_msg = "Não foi possível abrir o arquivo de entrada JSON.";
        return false;
    }

    json j;
    try {
        in >> j;
    } catch (json::parse_error& e) {
        error_msg = "Erro de parse no JSON: " + std::string(e.what());
        return false;
    }
    
    if (on_progress) on_progress(0.2);

    // Verificando se o JSON contém todas as chaves
    if (!j.contains("padded") || !j.contains("largura") || !j.contains("altura") || !j.contains("raiz")) {
        error_msg = "Arquivo JSON inválido: faltando campos essenciais.";
        return false;
    }

    // Lendo as dimensões do JSON
    int padded_size = j["padded"];
    int original_width = j["largura"];
    int original_height = j["altura"];

    // Criando a tela preta com o tamanho do preenchimento
    cv::Mat padded_image = cv::Mat::zeros(padded_size, padded_size, CV_8UC3);

    if (on_progress) on_progress(0.4);

    // Reconstruindo a imagem na tela com preenchimento
    reconstruct_image(padded_image, j["raiz"], 0, 0, padded_size);

    if (on_progress) on_progress(0.9);

    // Criando um retângulo com as dimensões originais e recortando essa região
    cv::Mat final_image = padded_image(cv::Rect(0, 0, original_width, original_height));

    try {
        cv::imwrite(output_path, final_image);
    } catch (const cv::Exception& ex) {
        error_msg = "Erro do OpenCV ao salvar a imagem: " + std::string(ex.what());
        return false;
    }

    if (on_progress) on_progress(1.0);
    
    return true;
}

void QuadtreeCompressor::reconstruct_image(cv::Mat& image, const json& j_node, int x, int y, int size) {
    if (j_node.is_null()) return;

    // Lendo a chave "filhos" na descompressão
    if (j_node.contains("filhos")) {
        int half_size = size / 2;
        reconstruct_image(image, j_node["filhos"][0], x, y, half_size);
        reconstruct_image(image, j_node["filhos"][1], x + half_size, y, half_size);
        reconstruct_image(image, j_node["filhos"][2], x, y + half_size, half_size);
        reconstruct_image(image, j_node["filhos"][3], x + half_size, y + half_size, half_size);
    } 
    // Lendo as chaves de cores R, G, B
    else if (j_node.contains("r") && j_node.contains("g") && j_node.contains("b")) {
        // Montando a cor na ordem BGR para o OpenCV
        cv::Scalar color(j_node["b"], j_node["g"], j_node["r"]);
        cv::rectangle(image, cv::Rect(x, y, size, size), color, cv::FILLED);
    }
}