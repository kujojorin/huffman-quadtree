#ifndef QUADTREECOMPRESSOR_HPP
#define QUADTREECOMPRESSOR_HPP

#include <string>
#include <functional>
#include <opencv2/opencv.hpp>
#include "json.hpp"

using json = nlohmann::json;

class QuadtreeCompressor {
private:
    struct Node {
        bool is_leaf;
        cv::Scalar avg_color;
        Node* children[4];

        Node() : is_leaf(true) {
            for (int i = 0; i < 4; ++i) children[i] = nullptr;
        }
        ~Node();
    };

    Node* root;

    void build_tree(Node* node, const cv::Mat& image_roi, int tolerance);
    void delete_tree(Node* node);
    json save_tree_to_json(Node* node);

public:
    QuadtreeCompressor();
    ~QuadtreeCompressor();

    bool compress_image(const std::string& input_path, const std::string& output_path, int tolerance, std::string& error_msg, const std::function<void(double)>& on_progress = nullptr);
};

#endif // QUADTREECOMPRESSOR_HPP