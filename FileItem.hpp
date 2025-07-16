#ifndef FILEITEM_HPP
#define FILEITEM_HPP

#include <glibmm/ustring.h>
#include <glibmm/object.h>

class FileItem : public Glib::Object {
public:
    Glib::ustring path;
    FileItem(const Glib::ustring& p);
};

#endif // FILEITEM_HPP