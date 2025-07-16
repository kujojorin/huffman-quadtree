#ifndef STATUSBAR_HPP
#define STATUSBAR_HPP

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/progressbar.h>
#include <glibmm/ustring.h>

class StatusBar : public Gtk::Box {
public:
    StatusBar();

    void set_message(const Glib::ustring& message, bool is_error = false);
    void update_progress(double percentage, const Glib::ustring& current_file, int total_files);

private:
    Gtk::Label m_label;
    Gtk::ProgressBar m_progress_bar;
};

#endif // STATUSBAR_HPP