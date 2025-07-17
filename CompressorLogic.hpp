#ifndef COMPRESSORLOGIC_HPP
#define COMPRESSORLOGIC_HPP

#include <functional>
#include <vector>
#include <string>
#include <glibmm/ustring.h>
#include <glibmm/refptr.h>
#include <giomm/file.h>
#include <filesystem>
#include "huffman.hpp"
#include "QuadtreeCompressor.hpp"

enum class CompressorMode { COMPRESS, DECOMPRESS };
enum class CompressionAlgorithm { HUFFMAN_TXT, QUADTREE_PNG };

struct ProcessResult {
    bool success = false;
    Glib::ustring message;
    long long initial_size = 0;
    long long final_size = 0;
};

class CompressorLogic {
public:
    std::function<void(double, const Glib::ustring&, int)> on_progress;
    std::function<void(const ProcessResult&)> on_finished;

    CompressorLogic();

    ProcessResult process_files(
        CompressorMode mode,
        CompressionAlgorithm algorithm,
        const std::vector<Glib::RefPtr<Gio::File>>& input_files,
        const Glib::ustring& output_folder_path,
        double compression_level
    );

private:
    huffman huffman_compressor;
    QuadtreeCompressor quadtree_compressor;

    // handlers para huffman
    ProcessResult handle_huffman_compression(const Glib::RefPtr<Gio::File>&, const Glib::ustring&, const Glib::ustring&, const std::function<void(double)>&);
    ProcessResult handle_huffman_decompression(const Glib::RefPtr<Gio::File>&, const Glib::ustring&, const Glib::ustring&, const std::function<void(double)>&);

    // handlers para Quadtree
    ProcessResult handle_quadtree_compression(const Glib::RefPtr<Gio::File>&, const Glib::ustring&, int, const Glib::ustring&, const std::function<void(double)>&);
    ProcessResult handle_quadtree_decompression(const Glib::RefPtr<Gio::File>&, const Glib::ustring&, const Glib::ustring&, const std::function<void(double)>&);
};

#endif // COMPRESSORLOGIC_HPP