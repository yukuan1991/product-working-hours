#ifndef QT_H
#define QT_H

#include <QTextCodec>
#include "stl_extension.hpp"
#include <QDebug>
#include <QObject>
#include <string>
#include <boost/utility/string_ref.hpp>

using boost::string_ref;

inline std::string system_to_unicode (string_ref src)
{
    auto codec = QTextCodec::codecForLocale ();
    return  codec->toUnicode (src.data ()).toStdString ();
}
inline std::string unicode_to_system (string_ref src)
{
    auto codec = QTextCodec::codecForLocale ();
    return std::string {codec->fromUnicode (src.data ()).data ()};
}

inline void children_recursive_helper (QObject* obj_current, QObject* obj_master)
{
    auto list = obj_current->children ();
    for (auto& it : list)
    {
        it->installEventFilter (obj_master);
        children_recursive_helper (it, obj_master);
    }
}


inline void set_children_filter (QObject* obj)
{
    assert (obj);
    children_recursive_helper (obj, obj);
}

#define debug_info \
qDebug () << "debug info @ " << __LINE__ << " " << __FILE__
#endif // QT_H
