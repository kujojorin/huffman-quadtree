#include "QuadtreeCompressor.hpp"
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm> // Para std::min

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
        delete node; // O destrutor do Node é recursivo
        root = nullptr;
    }
}

bool QuadtreeCompressor::compress_image(const std::string& input_path, const std::string& output_path, int tolerance, std::string& error_msg, const std::function<void(double)>& on_progress) {
    delete_tree(root);

    if (on_progress) on_progress(0.0);

    cv::Mat image = cv::imread(input_path);
    if (image.empty()) {
        error_msg = "Não foi possível carregar a imagem: " + input_path;
        return false;
    }
    
    // Tornando a imagem quadrada para simplificar a lógica da Quadtree
    int original_rows = image.rows;
    int original_cols = image.cols;
    int size = std::min(original_rows, original_cols);

    // Encontrando a maior potência de 2 que é menor ou igual ao tamanho
    int final_size = 1;
    while ((final_size * 2) <= size) {
        final_size *= 2;
    }
    if (final_size == 0) final_size = 1; // Para imagens muito pequenas

    // Cortando e redimensionando a imagem
    cv::Mat cropped_image = image(cv::Rect(0, 0, size, size));
    cv::Mat final_image;
    cv::resize(cropped_image, final_image, cv::Size(final_size, final_size));
    
    if (on_progress) on_progress(0.2);

    root = new Node();
    build_tree(root, final_image, tolerance);
    
    if (on_progress) on_progress(0.8);

    std::ofstream out(output_path);
    if (!out.is_open()) {
        error_msg = "Não foi possível criar o arquivo de saída JSON.";
        return false;
    }

    json j;
    j["size"] = final_size;
    j["root"] = save_tree_to_json(root);

    out << j.dump(2);
    
    if (on_progress) on_progress(1.0);

    return true;
}

void QuadtreeCompressor::build_tree(Node* node, const cv::Mat& image_roi, int tolerance) {
    node->avg_color = cv::mean(image_roi);

    cv::Mat diff;
    cv::absdiff(image_roi, node->avg_color, diff);
    // Convertendo a matriz de diferença para um único canal para calcular a soma
    cv::Mat diff_gray;
    cv::cvtColor(diff, diff_gray, cv::COLOR_BGR2GRAY);
    cv::Scalar total_error_scalar = cv::sum(diff_gray);
    double total_error = total_error_scalar[0];

    double error_per_pixel = total_error / (image_roi.rows * image_roi.cols);

    // A condição de parada para a recursão
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
    // Se o erro for menor que a tolerância, o nó já é uma folha por padrão
}

json QuadtreeCompressor::save_tree_to_json(Node* node) {
    if (!node) return nullptr;
    
    json j_node;
    if (node->is_leaf) {
        // Cores em OpenCV são BGR, arredondando para inteiros para o JSON
        j_node["b"] = static_cast<int>(node->avg_color[0]);
        j_node["g"] = static_cast<int>(node->avg_color[1]);
        j_node["r"] = static_cast<int>(node->avg_color[2]);
    } else {
        j_node["children"] = json::array({
            save_tree_to_json(node->children[0]),
            save_tree_to_json(node->children[1]),
            save_tree_to_json(node->children[2]),
            save_tree_to_json(node->children[3])
        });
    }
    return j_node;
}