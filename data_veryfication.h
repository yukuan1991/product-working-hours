#ifndef DATA_VERYFICATION_H
#define DATA_VERYFICATION_H

#include "Qt-Utils/json.hpp"
#include "Qt-Utils/stl_extension.hpp"

using json = nlohmann::json;


#define err_pos(str) \
    (std::string ("") + str + "\n检测来自: " __FILE__ " line: " + std::to_string (__LINE__))


optional<json> stdtime_data_veryfication (const string& file_data);

optional<std::string> stdtime_attachment_veryfication (const json& json_attach);

bool stdtime_chart_veryfication (const json& json_chart, const string& method);

bool stdtime_chart_totalinfo_veryfication (const json& json_chart_total, const string& method);

bool stdtime_chart_detailinfo_veryfication (const json& json_chart_detail, const string& method);

bool stdtime_chart_video (const json& json_video);

bool stdtime_chart_pts (const json& json_pts);

bool stdtime_chart_stddata (const json& json_stddata);

bool stdtime_chart_proinfo (const json& json_proinfo);

bool stdtime_chart_stdinfo (const json& json_stdinfo);

bool method_detection (const json& data, const std::string&);



#endif // DATA_VERYFICATION_H
