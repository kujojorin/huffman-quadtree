#ifndef OUTPUTFOLDERSELECTOR_HPP
#define OUTPUTFOLDERSELECTOR_HPP

#include <gtkmm.h>
#include <glibmm/ustring.h>
#include <functional> // Usando a biblioteca padrão do C++

class OutputFolderSelector : public Gtk::Box {
public:
    // Callback padrão usando std::function
    std::function<void(const Glib::ustring&)> on_folder_selected;

    OutputFolderSelector(Gtk::Window& parent_window);

    Glib::ustring get_selected_folder_path() const;

private:
    Gtk::Window& m_parent_window;
    Gtk::Label label_path;
    Gtk::Button button_select_folder;
    Glib::ustring selected_folder_path;

    void on_select_folder_clicked();
    void on_folder_dialog_response(int response_id, Gtk::FileChooserDialog* dialog);
};

#endif // OUTPUTFOLDERSELECTOR_HPP