#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <gtkmm.h>
#include <vector>

#include "FileItem.hpp"
#include "CompressorLogic.hpp"
#include "OutputFolderSelector.hpp"
#include "StatusBar.hpp"

class MainWindow : public Gtk::Window {
public:
    MainWindow();

private:
    // Contêineres
    Gtk::Box vbox;
    Gtk::Box process_controls_box;

    // Widgets de controle
    Gtk::Label algorithm_label;
    Gtk::DropDown compression_algorithm_dropdown;
    Glib::RefPtr<Gtk::StringList> compression_algorithm_model;
    Gtk::CheckButton mode_switch;
    Gtk::Button button_select;

    // Lista de arquivos com scroll
    Gtk::ScrolledWindow scrolled_window;
    Gtk::ListView listview;
    Glib::RefPtr<Gio::ListStore<FileItem>> liststore;
    Glib::RefPtr<Gtk::SignalListItemFactory> factory;

    // Botões de remoção
    Gtk::Button button_remove_selected;
    Gtk::Button button_remove_all;

    // Controles de processamento final
    Gtk::Scale scale;
    Gtk::Button button_process;
    
    // Classes de Lógica
    StatusBar status_bar;
    CompressorLogic compressor_logic;
    OutputFolderSelector output_folder_selector;

    // Métodos
    void on_select_files();
    void on_mode_switch_toggled();
    void on_process_button_clicked();
    void on_remove_selected_clicked();
    void on_remove_all_clicked();
    
    // O método agora recebe o caminho da pasta para poder abri-la
    void show_result_dialog(const ProcessResult& result, const Glib::ustring& output_folder);
};

#endif // MAINWINDOW_HPP