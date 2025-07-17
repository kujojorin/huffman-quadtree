#include "CompressorLogic.hpp"
#include <thread>
#include <chrono>
#include <iostream>
#include <filesystem>
#include <iomanip>

// Função auxiliar para a barra de progresso do terminal
static void print_terminal_progress(double progress) {
    // Garantindo que o progresso esteja entre 0.0 e 1.0
    progress = std::max(0.0, std::min(1.0, progress));

    int bar_width = 50;
    std::cout << "[";
    int pos = static_cast<int>(bar_width * progress);
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    // Usando formatação para uma exibição consistente
    std::cout << "] " << std::fixed << std::setprecision(1) << progress * 100.0 << " %\r";
    std::cout.flush(); // Garantindo que a saída seja exibida imediatamente
}

CompressorLogic::CompressorLogic() {}

ProcessResult CompressorLogic::process_files(
    CompressorMode mode,
    CompressionAlgorithm algorithm,
    const std::vector<Glib::RefPtr<Gio::File>>& input_files,
    const Glib::ustring& output_folder_path,
    double compression_level
) {
    ProcessResult final_result;
    final_result.success = true;

    // Verificando se há arquivos para processar
    if (input_files.empty()) {
        final_result.message = "Nenhum arquivo para processar.";
        final_result.success = false;
        return final_result;
    }
    // Verificando se uma pasta de saída foi selecionada
    if (output_folder_path.empty()) {
        final_result.message = "Erro: Nenhuma pasta de saída selecionada.";
        final_result.success = false;
        return final_result;
    }

    int total_files = input_files.size();
    for (int i = 0; i < total_files; ++i) {
        const auto& file = input_files[i];
        Glib::ustring current_file_path = file->get_path();
        
        std::filesystem::path p(current_file_path.c_str());
        Glib::ustring original_file_name_hint = p.stem().string();

        // Informando ao terminal qual arquivo está sendo processado
        std::cout << "Processando arquivo " << (i + 1) << " de " << total_files << ": " << p.filename().string() << std::endl;

        // Calculando a faixa de progresso para o arquivo atual
        double progress_base = (static_cast<double>(i) / total_files) * 100.0;
        double progress_range = (100.0 / total_files);

        // Criando o callback que será passado para as funções de compressão
        auto progress_handler = [this, current_file_path, total_files, progress_base, progress_range](double sub_progress) {
            // Atualizando a interface gráfica
            if (this->on_progress) {
                double overall_progress = progress_base + (sub_progress * progress_range);
                this->on_progress(overall_progress, current_file_path, total_files);
            }
            // Atualizando o terminal
            print_terminal_progress(sub_progress);
        };

        ProcessResult single_result;

        try {
            if (mode == CompressorMode::COMPRESS) {
                if (algorithm == CompressionAlgorithm::HUFFMAN_TXT) {
                    single_result = handle_huffman_compression(file, output_folder_path, original_file_name_hint, progress_handler);
                } else if (algorithm == CompressionAlgorithm::QUADTREE_PNG) {
                    single_result = handle_quadtree_compression(file, output_folder_path, static_cast<int>(compression_level), original_file_name_hint, progress_handler);
                }
            } else { // DECOMPRESS
                if (algorithm == CompressionAlgorithm::HUFFMAN_TXT) {
                    single_result = handle_huffman_decompression(file, output_folder_path, original_file_name_hint, progress_handler);
                } else if (algorithm == CompressionAlgorithm::QUADTREE_PNG) {
                    single_result = handle_quadtree_decompression(file, output_folder_path, original_file_name_hint, progress_handler);
                }
            }
        } catch (const std::exception& e) {
            single_result.success = false;
            single_result.message = "Exceção ao processar " + current_file_path + ": " + e.what();
            std::cerr << std::endl << single_result.message << std::endl;
        }

        // Saltando uma linha no terminal após a conclusão da barra de progresso
        std::cout << std::endl;

        // Agregando os resultados
        final_result.initial_size += single_result.initial_size;
        final_result.final_size += single_result.final_size;
        
        // Verificando se a operação do arquivo falhou
        if (!single_result.success) {
            final_result.success = false;
            final_result.message = single_result.message;
            return final_result; // Retornando imediatamente em caso de erro
        }
    }

    if (final_result.success) {
        final_result.message = "Processamento concluído com sucesso!";
        std::cout << "Processamento geral concluído com sucesso!" << std::endl;
    }
    
    return final_result;
}

ProcessResult CompressorLogic::handle_huffman_compression(
    const Glib::RefPtr<Gio::File>& input_file,
    const Glib::ustring& output_folder_path,
    const Glib::ustring& original_file_name_hint,
    const std::function<void(double)>& on_progress
) {
    ProcessResult result;
    std::string input_path_str = input_file->get_path();
    std::string output_path_str = (std::filesystem::path(output_folder_path.c_str()) / (original_file_name_hint.c_str() + std::string(".huf"))).string();

    try {
        result.initial_size = std::filesystem::file_size(input_path_str);
        huffman_compressor.compressao(input_path_str, output_path_str, on_progress);
        result.final_size = std::filesystem::file_size(output_path_str);
        result.success = true;
    } catch (const std::exception& e) {
        result.success = false;
        result.message = "Erro Huffman (Compressão): " + std::string(e.what());
    }
    return result;
}

ProcessResult CompressorLogic::handle_huffman_decompression(
    const Glib::RefPtr<Gio::File>& input_file,
    const Glib::ustring& output_folder_path,
    const Glib::ustring& original_file_name_hint,
    const std::function<void(double)>& on_progress
) {
    ProcessResult result;
    std::string input_path_str = input_file->get_path();
    std::string output_path_str = (std::filesystem::path(output_folder_path.c_str()) / (original_file_name_hint.c_str() + std::string("_descompactado.txt"))).string();

    try {
        result.initial_size = std::filesystem::file_size(input_path_str);
        huffman_compressor.expandir(input_path_str, output_path_str, on_progress);
        result.final_size = std::filesystem::file_size(output_path_str);
        result.success = true;
    } catch (const std::exception& e) {
        result.success = false;
        result.message = "Erro Huffman (Descompressão): " + std::string(e.what());
    }
    return result;
}

ProcessResult CompressorLogic::handle_quadtree_compression(
    const Glib::RefPtr<Gio::File>& input_file,
    const Glib::ustring& output_folder_path,
    int tolerance,
    const Glib::ustring& original_file_name_hint,
    const std::function<void(double)>& on_progress
) {
    ProcessResult result;
    std::string input_path_str = input_file->get_path();
    std::string output_path_str = (std::filesystem::path(output_folder_path.c_str()) / (original_file_name_hint.c_str() + std::string("_quadtree.json"))).string();

    std::string error_msg;
    result.initial_size = std::filesystem::file_size(input_path_str);
    result.success = quadtree_compressor.compress_image(input_path_str, output_path_str, tolerance, error_msg, on_progress);
    
    if (result.success) {
        result.final_size = std::filesystem::file_size(output_path_str);
    } else {
        result.message = "Erro Quadtree: " + error_msg;
    }
    return result;
}

ProcessResult CompressorLogic::handle_quadtree_decompression(
    const Glib::RefPtr<Gio::File>& input_file,
    const Glib::ustring& output_folder_path,
    const Glib::ustring& original_file_name_hint,
    const std::function<void(double)>& on_progress
) {
    ProcessResult result;
    std::string input_path_str = input_file->get_path();
    std::string output_path_str = (std::filesystem::path(output_folder_path.c_str()) / (original_file_name_hint.c_str() + std::string("_descompactado.png"))).string();

    std::string error_msg;
    result.initial_size = std::filesystem::file_size(input_path_str);
    result.success = quadtree_compressor.decompress_image(input_path_str, output_path_str, error_msg, on_progress);

    if(result.success) {
        result.final_size = std::filesystem::file_size(output_path_str);
    } else {
        result.message = "Erro Quadtree (Descompressão): " + error_msg;
    }
    return result;
}