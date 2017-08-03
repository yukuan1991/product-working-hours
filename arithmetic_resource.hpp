#ifndef ARITHMETIC_RESOURCE_H
#define ARITHMETIC_RESOURCE_H

#include <map>
#include <string>
#include <QString>
#include "Qt-Utils/encryption.h"
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include "Qt-Utils/stl_extension.hpp"
#include <mutex>
#include <QDir>
#include "Qt-Utils/qt.hpp"
#include <QAbstractItemModel>
#include <QModelIndex>

#include "Qt-Utils/json.hpp"
using json = nlohmann::json;

inline bool check_file_position_legality (const QString& path);
inline void read_most_data (std::map<std::string, int>&kv_tmu, std::map<std::string, std::vector<std::pair<std::string, bool>>>& most_data);
inline void read_mod_data (std::map<std::string, int>&kv_tmu,std::map<std::string, std::tuple<std::string, std::string, std::string, std::string, std::string>>& mod_data);
inline void read_mtm_data (std::map<std::string, int>& kv_tmu, std::map<std::string, std::tuple<std::string, std::string, std::string, std::string, unsigned>>& mtm_data);
inline unsigned long long calculation_video_time (const unsigned long long t,  const std::vector<unsigned long long>& invalid_time);
inline QString get_method (const json& file_data);

enum class PTS
{
    mtm, mod, most, left, right, result
};

inline int get_header_col (const QAbstractItemModel* model, const QString& header)
{
    assert (model);
    for (int i = 0; i < model->columnCount (); i ++)
    {
        auto value = model->headerData (i, Qt::Horizontal);
        if (value.toString () == header)
        {
            return i;
        }
    }
    return -1;
}


inline QModelIndex get_header_index (const QAbstractItemModel* model, const QString& header, int row)
{
    return model->index (row, get_header_col (model, header));
}

inline QVariant get_header_data (const QAbstractItemModel* model, const QString& header, int row, int role = Qt::DisplayRole)
{
    assert (model and row >= 0);
    auto col = get_header_col (model, header);
    if (col == -1)
    {
        return {};
    }
    auto index = model->index (row, col);
    return index.data (role);

}
inline QVariant get_header_data (const QAbstractItemModel* model, const QString& header, const QModelIndex& ref, int role = Qt::DisplayRole)
{
    return get_header_data (model, header, ref.row (), role);
}

inline QString get_index_header (QAbstractItemModel* model, const QModelIndex& index)
{
    return model->headerData (index.column (), Qt::Horizontal).toString ();
}

inline auto& current_method ()
{
    static PTS method;
    return method;
}

inline auto read_tmu_data ()
{
    static std::map<std::string, int> kv_tmu;
    static std::map<std::string, std::vector<std::pair<std::string, bool>>> most_data;
    static std::map<std::string, std::tuple<std::string, std::string, std::string, std::string, std::string>> mod_data;
    static std::map<std::string, std::tuple<std::string, std::string, std::string, std::string, unsigned>> mtm_data;

    if (kv_tmu.empty ())
    {
        assert (most_data.empty());
        assert (mod_data.empty());
        assert (mtm_data.empty());

        read_mtm_data (kv_tmu, mtm_data);
        read_mod_data (kv_tmu, mod_data);
        read_most_data (kv_tmu, most_data);
    }

    return make_tuple(kv_tmu, most_data, mod_data, mtm_data);
}


void read_most_data (std::map<std::string, int>&kv_tmu, std::map<std::string, std::vector<std::pair<std::string, bool>>>& most_data)
{
    using namespace tinyxml2;
    tinyxml2::XMLDocument doc;

    auto encryption = krys::read_all ("most.data");
    assert (encryption);
    std::string str_decryption;
    krys3des_decryption (*encryption, "123456789012345678901234", str_decryption); assert (!str_decryption.empty ());
    doc.Parse (str_decryption.data ());

    tinyxml2::XMLElement* root = doc.RootElement ();
    assert (root and ::strcmp (root->Value (), "excel") == 0);

    tinyxml2::XMLElement* row = root->FirstChildElement ("row");

    for (; row; row = row->NextSiblingElement ("row"))
    {
        std::vector<std::pair<std::string, bool>> vec_element;
        auto sz_key = row->Attribute ("key"); assert (sz_key);


        for (auto cell = row->FirstChildElement ("cell"); cell; cell = cell->NextSiblingElement ("cell"))
        {
            assert (cell->GetText () and cell->Attribute ("bold"));
            vec_element.emplace_back (std::string {cell->GetText ()}, ::strcmp ("1", cell->Attribute ("bold")) == 0);
        }

        boost::smatch m;
        std::string str_key = sz_key;
        bool found = boost::regex_search (str_key, m, boost::regex {string {"[[:digit:]]+$"}});
        if (!found) assert (false);

        std::string str = m[0].str ();
        QString qstr = str.data ();

        unsigned digit = qstr.toUInt ();
        kv_tmu ["most_"s + sz_key] = static_cast<int>(digit) * 10;

        most_data [str_key] = std::move (vec_element);
    }
}

void read_mod_data (std::map<std::string, int>&kv_tmu,std::map<std::string, std::tuple<std::string, std::string, std::string, std::string, std::string>>& mod_data)
{
    auto encryption = krys::read_all ("mod.data");
    assert (encryption);
    std::string str_decryption;
    krys3des_decryption (*encryption, "123456789012345678901234", str_decryption);

    krys::write_buffer ("mod.xml", str_decryption);

    tinyxml2::XMLDocument doc;
    doc.Parse (str_decryption.data (), str_decryption.length ());

    auto root = doc.RootElement (); assert (root);
    assert (strcmp (root->Value (), "excel") == 0);

    std::string code, action_attr, action_name, action_content, example, remark;
    unsigned tmu;

    for (auto row = root->FirstChildElement ("row"); row != nullptr; row = row->NextSiblingElement ("row"))
    {
        auto cell1 = row->FirstChildElement ("cell"); assert (cell1 and cell1->GetText());
        code = cell1->GetText ();

        auto cell2 = cell1->NextSiblingElement ("cell"); assert (cell2 and cell2->GetText());
        action_attr = cell2->GetText ();

        auto cell3 = cell2->NextSiblingElement ("cell"); assert (cell3 and cell3->GetText());
        action_name = cell3->GetText ();

        auto cell4 = cell3->NextSiblingElement ("cell"); assert (cell4 and cell4->GetText());
        action_content = cell4->GetText ();

        auto cell5 = cell4->NextSiblingElement ("cell"); assert (cell5 and cell5->GetText());
        example = cell5->GetText ();

        auto cell6 = cell5->NextSiblingElement ("cell"); assert (cell5 and cell5->GetText());
        remark = cell6->GetText ();

        auto cell7 = cell6->NextSiblingElement ("cell"); assert (cell6 and cell6->GetText());
        (void) cell7;
        auto ret = cell7->QueryUnsignedText (&tmu);
        assert (ret == tinyxml2::XML_SUCCESS); (void)ret;

        kv_tmu ["mod_" + code] = tmu; (void)tmu;
        mod_data [std::move (code)] = std::make_tuple (std::move (action_attr), std::move (action_name),
                                                        std::move (action_content), std::move (example), std::move (remark));
    }
}


void read_mtm_data (std::map<std::string, int>& kv_tmu, std::map<std::string, std::tuple<std::string, std::string, std::string, std::string, unsigned>>& mtm_data)
{
    auto encryption = krys::read_all ("mtm.data");
    assert (encryption);
    std::string str_decryption;
    krys3des_decryption (*encryption, "123456789012345678901234", str_decryption);

    tinyxml2::XMLDocument doc;
    doc.Parse (str_decryption.data (), str_decryption.length ());

    auto root = doc.RootElement (); assert (root);
    assert (strcmp (root->Value (), "excel") == 0);

    std::string code, description, start, content, end;
    unsigned tmu;

    for (auto row = root->FirstChildElement ("row"); row != nullptr; row = row->NextSiblingElement ("row"))
    {
        auto cell1 = row->FirstChildElement ("cell"); assert (cell1 and cell1->GetText());
        code = cell1->GetText ();

        auto cell2 = cell1->NextSiblingElement ("cell"); assert (cell2 and cell2->GetText());
        description = cell2->GetText ();

        auto cell3 = cell2->NextSiblingElement ("cell"); assert (cell3 and cell3->GetText());
        start = cell3->GetText ();

        auto cell4 = cell3->NextSiblingElement ("cell"); assert (cell4 and cell4->GetText());
        content = cell4->GetText ();

        auto cell5 = cell4->NextSiblingElement ("cell"); assert (cell5 and cell5->GetText());
        end = cell5->GetText ();

        auto cell6 = cell5->NextSiblingElement ("cell"); assert (cell6 and cell6->GetText());
        (void) cell6;
        assert (cell6->QueryUnsignedText (&tmu) == tinyxml2::XML_SUCCESS);

        mtm_data [code] = std::make_tuple (std::move (description), std::move (start), std::move (content), std::move (end), tmu);

        kv_tmu ["mtm_" + code] = tmu;
    }
}

/// unit tested
unsigned long long calculation_video_time (const unsigned long long t,  const std::vector<unsigned long long>& invalid_time)
{
    if (invalid_time.empty ())
    {
        return t;
    }

    assert (invalid_time.size () % 2 == 0);
    for (unsigned i = 0; i < invalid_time.size (); i++)
    {
        if (i != 0)
        {
            assert (invalid_time.at (i) >= invalid_time.at (i - 1));
        }
    }

    unsigned long long total_invalid = 0, total_valid = 0;

    for (unsigned i = 0; i < invalid_time.size (); i += 2)
    {
        unsigned long long valid_increment;
        if (i == 0)
        {
            valid_increment = invalid_time.at (i);
        }
        else
        {
            valid_increment = invalid_time.at (i) - invalid_time.at (i - 1);
        }
        total_valid += valid_increment;

        if (t < total_valid)
        {
            return t + total_invalid;
        }

        assert (i + 1 < invalid_time.size ());

        total_invalid += (invalid_time.at (i + 1) - invalid_time.at (i));
    }

    return t + total_invalid;
}

enum class path_type
{
    master_dir,
    production_dir,
    capp_dir,
    unknown,
};

inline path_type get_path_type (const QString& master_path, const QString& path)
{
    QDir path_dir {path};
    auto lambda_equal = [&] ()
    {
        auto sys_path = unicode_to_system (path_dir.absolutePath ().toStdString ());
        auto sys_master_path = unicode_to_system (master_path.toStdString ());
        try
        {
            return boost::filesystem::equivalent (sys_path, sys_master_path);
        }
        catch (...)
        {
            assert (false);
        }
        return false;
    };
    if (lambda_equal ())
    {
        return path_type::master_dir;
    }

    if (!path_dir.cdUp ()) return path_type::unknown;

    if (lambda_equal ())
    {
        return path_type::production_dir;
    }

    if (!path_dir.cdUp ()) return path_type::unknown;

    if (lambda_equal ())
    {
        return path_type::capp_dir;
    }

    return path_type::unknown;
}

inline QString system_path_to_production_path (const QString& )
{
    //assert (check_file_position_legality (syspath));
    return {};
}

inline bool check_file_position_legality (const QString& path)
{
    QFileInfo info {path};
    assert (info.isFile ());
    auto filename = info.baseName ();

    auto capp_dir = info.dir ();
    auto product_dir = capp_dir;
    bool is_ok;
    is_ok = product_dir.cdUp (); //assert (is_ok);
    if (!is_ok)
    {
        return false;
    }
    auto master_dir = product_dir;
    is_ok = master_dir.cdUp (); //assert (is_ok);
    if (!is_ok)
    {
        return false;
    }

    auto master_gbk = unicode_to_system (master_dir.absolutePath ().toStdString ());
    auto product_gbk = unicode_to_system (PRODUCT_PATH);

    if (boost::filesystem::equivalent (master_gbk, product_gbk))
    {
        return true;
    }
    else
    {
        return false;
    }
}


inline QString get_method (const json& file_data)
{
    auto iter_attachment = file_data.find ("附加信息");
    assert (iter_attachment != file_data.end ());
    assert (iter_attachment->is_object ());

    auto iter_method = iter_attachment->find ("测量方法");
    assert (iter_method != iter_attachment->end ());
    assert (iter_method->is_string ());
    std::string method = *iter_method;

    return method.data ();
}

#endif // ARITHMETIC_RESOURCE_H
