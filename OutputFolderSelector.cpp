#include "OutputFolderSelector.hpp"
#include <iostream>

OutputFolderSelector::OutputFolderSelector(Gtk::Window& parent_window)
: m_parent_window(parent_window) {
    set_orientation(Gtk::Orientation::HORIZONTAL);
    set_spacing(5);

    label_path.set_text("Pasta de Saída: Não Selecionada");
    label_path.set_hexpand(true);
    label_path.set_halign(Gtk::Align::START);
    append(label_path);

    button_select_folder.set_label("Selecionar Pasta");
    button_select_folder.signal_clicked().connect(sigc::mem_fun(*this, &OutputFolderSelector::on_select_folder_clicked));
    append(button_select_folder);
}

Glib::ustring OutputFolderSelector::get_selected_folder_path() const {
    return selected_folder_path;
}

void OutputFolderSelector::on_select_folder_clicked() {
    auto dialog = new Gtk::FileChooserDialog(m_parent_window, "Selecionar Pasta de Saída", Gtk::FileChooser::Action::SELECT_FOLDER);
    dialog->set_modal(true);
    dialog->set_default_size(400, 300);

    dialog->add_button("_Cancelar", Gtk::ResponseType::CANCEL);
    dialog->add_button("_Selecionar", Gtk::ResponseType::OK);

    // CORREÇÃO: Movendo toda a lógica para dentro da lambda e usando 'delete' no final.
    // Isso garante que o sinal seja tratado apenas uma vez e o diálogo seja destruído corretamente.
    dialog->signal_response().connect([this, dialog](int response_id) {
        if (response_id == Gtk::ResponseType::OK) {
            Glib::RefPtr<Gio::File> folder = dialog->get_file();
            if (folder) {
                selected_folder_path = folder->get_path();
                label_path.set_text("Pasta de Saída: " + selected_folder_path);
                
                if (on_folder_selected) {
                    on_folder_selected(selected_folder_path);
                }
                
                std::cout << "Pasta de saída selecionada: " << selected_folder_path << std::endl;
            }
        } else {
            // Esta mensagem agora só aparecerá se o usuário realmente cancelar.
            std::cout << "Seleção de pasta cancelada." << std::endl;
        }

        // Destruindo o diálogo após o tratamento da resposta.
        delete dialog;
    });

    dialog->show();
}