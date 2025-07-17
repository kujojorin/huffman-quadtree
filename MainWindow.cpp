#include "MainWindow.hpp"
#include "CompressorLogic.hpp"
#include <iostream>
#include <thread>
#include <filesystem>
#include <glibmm/main.h>
#include <iomanip>
#include <sstream>
#include <cstdlib>

void open_folder_in_explorer(const Glib::ustring& path) {
    std::string command;
#ifdef _WIN32
    command = "explorer \"" + std::string(path) + "\"";
#elif __APPLE__
    command = "open \"" + std::string(path) + "\"";
#else
    command = "xdg-open \"" + std::string(path) + "\"";
#endif
    system(command.c_str());
}

MainWindow::MainWindow() : output_folder_selector(*this) {
    set_title("Compressor GTKmm 4");
    set_default_size(600, 500);

    m_dispatcher.connect(sigc::mem_fun(*this, &MainWindow::on_processing_finished));

    vbox.set_orientation(Gtk::Orientation::VERTICAL);
    vbox.set_spacing(10);
    vbox.set_margin(12);
    set_child(vbox);

    process_controls_box.set_orientation(Gtk::Orientation::VERTICAL);
    process_controls_box.set_spacing(10);
    vbox.append(process_controls_box);

    algorithm_label.set_text("Escolha o algoritmo:");
    algorithm_label.set_halign(Gtk::Align::START);
    process_controls_box.append(algorithm_label);
    
    compression_algorithm_model = Gtk::StringList::create({"Huffman (Texto)", "Quadtree (Imagem)"});
    compression_algorithm_dropdown.set_model(compression_algorithm_model);
    compression_algorithm_dropdown.property_selected().signal_changed().connect(sigc::mem_fun(*this, &MainWindow::on_algorithm_changed));
    process_controls_box.append(compression_algorithm_dropdown);

    mode_switch.set_label("Modo Descompressão");
    mode_switch.set_active(false);
    mode_switch.signal_toggled().connect(sigc::mem_fun(*this, &MainWindow::on_mode_switch_toggled));
    mode_switch.set_halign(Gtk::Align::START);
    process_controls_box.append(mode_switch);

    button_select.set_label("Adicionar Arquivos");
    button_select.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_select_files));
    process_controls_box.append(button_select);

    scrolled_window.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    scrolled_window.set_vexpand(true);

    liststore = Gio::ListStore<FileItem>::create();
    factory = Gtk::SignalListItemFactory::create();
    factory->signal_setup().connect([](const Glib::RefPtr<Gtk::ListItem>& item) {
        auto* label = Gtk::make_managed<Gtk::Label>();
        item->set_child(*label);
    });
    factory->signal_bind().connect([](const Glib::RefPtr<Gtk::ListItem>& item) {
        auto file_item = std::dynamic_pointer_cast<FileItem>(item->get_item());
        auto* label = dynamic_cast<Gtk::Label*>(item->get_child());
        if (label && file_item)
            label->set_text(file_item->path);
    });
    auto selection_model = Gtk::SingleSelection::create(liststore);
    listview.set_model(selection_model);
    listview.set_factory(factory);
    
    scrolled_window.set_child(listview);
    vbox.append(scrolled_window);

    auto remove_buttons_box = Gtk::Box(Gtk::Orientation::HORIZONTAL, 5);
    remove_buttons_box.set_halign(Gtk::Align::END);

    button_remove_selected.set_label("Remover Selecionado");
    button_remove_selected.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_remove_selected_clicked));
    remove_buttons_box.append(button_remove_selected);

    button_remove_all.set_label("Remover Todos");
    button_remove_all.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_remove_all_clicked));
    remove_buttons_box.append(button_remove_all);
    vbox.append(remove_buttons_box);

    scale_box.set_orientation(Gtk::Orientation::HORIZONTAL);
    scale_box.set_spacing(10);
    
    low_quality_label.set_text("Baixa Qualidade");
    high_quality_label.set_text("Alta Qualidade");

    scale.set_hexpand(true);
    scale.set_range(0, 100);
    scale.set_value(90); 
    scale.set_digits(0); 
    scale.set_draw_value(false);
    scale.signal_value_changed().connect(sigc::mem_fun(*this, &MainWindow::on_scale_value_changed));

    scale_value_label.set_width_chars(3); 
    scale_value_label.set_halign(Gtk::Align::CENTER);
    
    scale_box.append(low_quality_label);
    scale_box.append(scale);
    scale_box.append(scale_value_label); 
    scale_box.append(high_quality_label);
    vbox.append(scale_box);

    vbox.append(output_folder_selector);

    button_process.set_label("Iniciar Compressão");
    button_process.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_process_button_clicked));
    vbox.append(button_process);

    vbox.append(status_bar);

    compressor_logic.on_progress = [this](double p, const Glib::ustring& f, int t) {
        Glib::signal_idle().connect([this, p, f, t]() {
            status_bar.update_progress(p, f, t);
            return false;
        });
    };
    
    on_mode_switch_toggled();
    on_algorithm_changed(); 
    on_scale_value_changed();
}

void MainWindow::on_algorithm_changed() {
    auto algorithm = static_cast<CompressionAlgorithm>(compression_algorithm_dropdown.get_selected());
    bool is_quadtree = (algorithm == CompressionAlgorithm::QUADTREE_PNG);
    bool is_compress_mode = !mode_switch.get_active();
    scale_box.set_visible(is_quadtree && is_compress_mode);
}

void MainWindow::on_mode_switch_toggled() {
    bool is_decompress = mode_switch.get_active();
    button_process.set_label(is_decompress ? "Iniciar Descompressão" : "Iniciar Compressão");
    on_algorithm_changed();
    status_bar.set_message(Glib::ustring("Modo alterado para ") + (is_decompress ? "Descompressão" : "Compressão") + ".");
}

void MainWindow::on_scale_value_changed() {
    int value = static_cast<int>(scale.get_value());
    scale_value_label.set_text(std::to_string(value));
}

void MainWindow::on_select_files() {
    auto dialog = new Gtk::FileChooserDialog(*this, "Selecionar arquivos", Gtk::FileChooser::Action::OPEN);
    dialog->set_modal(true);
    dialog->set_select_multiple(true);

    auto filter = Gtk::FileFilter::create();
    auto algorithm = static_cast<CompressionAlgorithm>(compression_algorithm_dropdown.get_selected());
    bool is_decompress_mode = mode_switch.get_active();

    if (is_decompress_mode) {
        if (algorithm == CompressionAlgorithm::HUFFMAN_TXT) {
            filter->set_name("Arquivos Huffman (*.huf)");
            filter->add_pattern("*.huf");
        } else if (algorithm == CompressionAlgorithm::QUADTREE_PNG) {
            filter->set_name("Arquivos Quadtree (*.json)");
            filter->add_pattern("*.json");
        }
    } else {
        if (algorithm == CompressionAlgorithm::QUADTREE_PNG) {
            filter->set_name("Imagens (PNG, JPG)");
            filter->add_pattern("*.png");
            filter->add_pattern("*.jpg");
            filter->add_pattern("*.jpeg");
        } else if (algorithm == CompressionAlgorithm::HUFFMAN_TXT) {
            filter->set_name("Textos (TXT)");
            filter->add_pattern("*.txt");
        }
    }
    
    dialog->add_filter(filter);

    dialog->add_button("_Cancelar", Gtk::ResponseType::CANCEL);
    dialog->add_button("_Selecionar", Gtk::ResponseType::OK);

    dialog->signal_response().connect([this, dialog](int response) {
        if (response == Gtk::ResponseType::OK) {
            std::vector<Glib::RefPtr<Gio::File>> selected_files_vector = dialog->get_files2();
            int added_count = 0;
            int duplicate_count = 0;
            int invalid_count = 0;

            for (const auto& new_file : selected_files_vector) {
                Glib::ustring new_path = new_file->get_path();
                bool is_duplicate = false;
                
                std::string ext = std::filesystem::path(new_path.c_str()).extension().string();

                bool valid = false;
                auto algorithm = static_cast<CompressionAlgorithm>(compression_algorithm_dropdown.get_selected());
                bool is_decompress_mode = mode_switch.get_active();

                if (is_decompress_mode) {
                    if (algorithm == CompressionAlgorithm::HUFFMAN_TXT) valid = (ext == ".huf");
                    else if (algorithm == CompressionAlgorithm::QUADTREE_PNG) valid = (ext == ".json");
                } else {
                    if (algorithm == CompressionAlgorithm::QUADTREE_PNG) valid = (ext == ".png" || ext == ".jpg" || ext == ".jpeg");
                    else if (algorithm == CompressionAlgorithm::HUFFMAN_TXT) valid = (ext == ".txt");
                }

                if (!valid) {
                    invalid_count++;
                    continue;
                }
                
                for (guint i = 0; i < liststore->get_n_items(); ++i) {
                    auto existing_item = std::dynamic_pointer_cast<FileItem>(liststore->get_object(i));
                    if (existing_item && existing_item->path == new_path) {
                        is_duplicate = true;
                        break;
                    }
                }

                if (!is_duplicate) {
                    auto ref = Glib::make_refptr_for_instance<FileItem>(new FileItem(new_path));
                    liststore->append(ref);
                    added_count++;
                } else {
                    duplicate_count++;
                }
            }
            
            Glib::ustring status_message;
            if (added_count > 0) status_message = Glib::ustring::format(added_count) + (added_count == 1 ? " arquivo válido adicionado. " : " arquivos válidos adicionados. ");
            if (invalid_count > 0) status_message += Glib::ustring::format(invalid_count) + (invalid_count == 1 ? " arquivo com extensão inválida ignorado. " : " arquivos com extensão inválida ignorados. ");
            if (duplicate_count > 0) status_message += Glib::ustring::format(duplicate_count) + (duplicate_count == 1 ? " duplicado ignorado. " : " duplicados ignorados. ");
            if (status_message.empty()) status_message = "Nenhum arquivo novo foi adicionado. ";
            status_message += "Total: " + Glib::ustring::format(liststore->get_n_items());
            status_bar.set_message(status_message);
        } else {
            status_bar.set_message("Seleção de arquivos cancelada.");
        }
        delete dialog;
    });

    dialog->show();
}

void MainWindow::on_remove_selected_clicked() {
    if(auto selection = std::dynamic_pointer_cast<Gtk::SingleSelection>(listview.get_model())) {
        guint selected_item_idx = selection->get_selected();
        if (selected_item_idx != GTK_INVALID_LIST_POSITION) {
            liststore->remove(selected_item_idx);
            status_bar.set_message("Arquivo removido da lista.");
        } else {
            status_bar.set_message("Nenhum arquivo selecionado para remover.");
        }
    }
}

void MainWindow::on_remove_all_clicked() {
    liststore->remove_all();
    status_bar.set_message("Todos os arquivos foram removidos da lista.");
}

void MainWindow::on_process_button_clicked() {
    std::vector<Glib::RefPtr<Gio::File>> files_to_process;
    for (guint i = 0; i < liststore->get_n_items(); ++i) {
        auto item = std::dynamic_pointer_cast<FileItem>(liststore->get_object(i));
        if (item) {
            files_to_process.push_back(Gio::File::create_for_path(item->path));
        }
    }

    if (files_to_process.empty()) {
        status_bar.set_message("Nenhum arquivo para processar. Selecione arquivos primeiro.", true);
        return;
    }

    Glib::ustring output_folder = output_folder_selector.get_selected_folder_path();
    if (output_folder.empty()) {
        status_bar.set_message("Selecione uma pasta de saída primeiro.", true);
        return;
    }

    CompressorMode mode = mode_switch.get_active() ? CompressorMode::DECOMPRESS : CompressorMode::COMPRESS;
    CompressionAlgorithm algo = static_cast<CompressionAlgorithm>(compression_algorithm_dropdown.get_selected());
    
    // Convertendo o valor de "Qualidade" da barra (0-100) para "Tolerância" (255-0)
    double quality_setting = scale.get_value();
    // Qualidade 0 -> Tolerância 255. Qualidade 100 -> Tolerância 0.
    double tolerance_level = 255.0 * (1.0 - (quality_setting / 100.0));
    
    button_process.set_sensitive(false);
    button_select.set_sensitive(false);
    mode_switch.set_sensitive(false);
    output_folder_selector.set_sensitive(false);
    compression_algorithm_dropdown.set_sensitive(false);
    
    status_bar.set_message("Iniciando processamento...");

    std::thread([this, mode, algo, files_to_process, output_folder, tolerance_level]() {
        // 1. Executando o trabalho principal
        m_processing_result = compressor_logic.process_files(mode, algo, files_to_process, output_folder, tolerance_level);

        // 2. Armazenando os resultados em variáveis membro para a UI thread acessar
        m_output_folder_for_result = output_folder;
        m_mode_for_result = mode;

        // 3. Notificando a UI thread de forma segura para fazer as atualizações
        m_dispatcher.emit();
    }).detach();
}

// Este método é executado na thread da UI, chamado pelo dispatcher.
void MainWindow::on_processing_finished() {
    // Reativando todos os botões
    button_process.set_sensitive(true);
    button_select.set_sensitive(true);
    mode_switch.set_sensitive(true);
    output_folder_selector.set_sensitive(true);
    compression_algorithm_dropdown.set_sensitive(true);
    on_mode_switch_toggled();

    // Exibindo o diálogo de resultado com os dados que foram armazenados
    show_result_dialog(m_processing_result, m_output_folder_for_result, m_mode_for_result);
}

void MainWindow::show_result_dialog(const ProcessResult& result, const Glib::ustring& output_folder, CompressorMode mode) {
    Glib::ustring title;
    std::stringstream message_stream;

    if (result.success) {
        title = "Processamento Concluído";
        
        if (mode == CompressorMode::COMPRESS) {
            double taxa = 0.0;
            if (result.initial_size > 0) {
                taxa = 100.0 * (1.0 - (double)result.final_size / result.initial_size);
            }
            message_stream << "Tamanho Inicial: " << result.initial_size << " bytes\n"
                           << "Tamanho Final:   " << result.final_size << " bytes\n\n"
                           << "Taxa de compressão: " << std::fixed << std::setprecision(2) << taxa << "%";
        } else {
             message_stream << "Arquivo(s) descomprimido(s) com sucesso.\n\n"
                           << "Tamanho do arquivo compactado: " << result.initial_size << " bytes\n"
                           << "Tamanho do arquivo restaurado: " << result.final_size << " bytes";
        }
        
        liststore->remove_all();
        status_bar.set_message("Pronto.");
        open_folder_in_explorer(output_folder);
        
    } else {
        title = "Erro no Processamento";
        message_stream << result.message;
        status_bar.set_message("Ocorreu uma falha.", true);
    }

    auto dialog = new Gtk::MessageDialog(*this, title, false, Gtk::MessageType::INFO, Gtk::ButtonsType::OK, true);
    dialog->set_secondary_text(message_stream.str());
    
    dialog->signal_response().connect([dialog](int) {
        dialog->hide();
        delete dialog;
    });
    
    dialog->show();
}