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
    // callback on_progress
    std::function<void(double, const Glib::ustring&, int)> on_progress;

    CompressorLogic();

    // retorna a struct de resultado
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

    ProcessResult handle_huffman_compression(
        const Glib::RefPtr<Gio::File>& input_file,
        const Glib::ustring& output_folder_path,
        const Glib::ustring& original_file_name_hint,
        const std::function<void(double)>& on_progress
    );
    ProcessResult handle_huffman_decompression(
        const Glib::RefPtr<Gio::File>& input_file,
        const Glib::ustring& output_folder_path,
        const Glib::ustring& original_file_name_hint,
        const std::function<void(double)>& on_progress
    );
    ProcessResult handle_quadtree_compression(
        const Glib::RefPtr<Gio::File>& input_file,
        const Glib::ustring& output_folder_path,
        int tolerance,
        const Glib::ustring& original_file_name_hint,
        const std::function<void(double)>& on_progress
    );
};

#endif // COMPRESSORLOGIC_HPP