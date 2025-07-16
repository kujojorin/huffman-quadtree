#include "StatusBar.hpp"
#include <filesystem> // CORREÇÃO: Incluindo para usar std::filesystem::path
#include <iomanip>    // CORREÇÃO: Incluindo para usar std::setprecision
#include <sstream>    // Incluindo para formatar a string de porcentagem

StatusBar::StatusBar() {
    set_orientation(Gtk::Orientation::HORIZONTAL);
    set_spacing(10);

    m_label.set_text("Pronto.");
    m_label.set_halign(Gtk::Align::START);
    m_label.set_hexpand(true);
    m_label.set_ellipsize(Pango::EllipsizeMode::END);

    append(m_label);

    m_progress_bar.set_halign(Gtk::Align::END);
    m_progress_bar.set_hexpand(false);
    m_progress_bar.set_size_request(150, -1);
    m_progress_bar.set_visible(false);
    append(m_progress_bar);
}

void StatusBar::set_message(const Glib::ustring& message, bool is_error) {
    m_label.set_text(message);
    m_label.remove_css_class("error");

    if (is_error) {
        m_label.add_css_class("error");
    }

    m_progress_bar.set_visible(false);
}

void StatusBar::update_progress(double percentage, const Glib::ustring& current_file, int total_files) {
    // Marcando o parâmetro como utilizado para evitar avisos do compilador.
    // Poderíamos usá-lo para exibir "Arquivo X de Y".
    (void)total_files;

    m_progress_bar.set_visible(true);
    
    double fraction = percentage / 100.0;
    m_progress_bar.set_fraction(fraction);
    
    // CORREÇÃO: Formatando a string de progresso de forma segura
    // e extraindo o nome do arquivo corretamente.
    
    // Extraindo apenas o nome do arquivo do caminho completo
    std::string filename = std::filesystem::path(current_file.c_str()).filename().string();

    // Formatando a porcentagem para não ter casas decimais
    std::stringstream ss;
    ss << std::fixed << std::setprecision(0) << percentage;
    
    Glib::ustring text = "Processando: " + filename + " (" + ss.str() + "%)";
    m_label.set_text(text);
}