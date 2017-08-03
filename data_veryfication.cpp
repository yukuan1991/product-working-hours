#include "data_veryfication.h"
#include <boost/lexical_cast.hpp>
#include <QDate>
#include "Qt-Utils/qt.hpp"
#include <QMessageBox>
#include "boost/algorithm/string.hpp"
#include "Qt-Utils/stl_extension.hpp"
#include <iostream>


std::string file_source;

static bool pts_left_verify (const json& left_hand);
static bool pts_right_verify (const json& right_hand);
static bool pts_result_verify (const json& result);
static bool pts_total_verify (const json& total);
static bool video_task_verify (const json& task);
static bool video_data_verify (const json& data);
static bool video_result_verify (const json& result);
static bool video_total_verify (const json& total);
static bool stddata_total_verify (const json& total_data);
static bool video_attribute_verify (const json& attribute);

optional<json> stdtime_data_veryfication(const string& file_data)
try
{
    json data = json::parse (file_data.data ());

    file_source = data["文件来源"];

    if (file_source == "产品工时")
    {
        if (stdtime_chart_proinfo (data))
        {
            return data;
        }
    }
    else if (file_source == "工时资料库")
    {
        if (stdtime_chart_stdinfo (data))
        {
            return data;
        }
    }

    return data;
}
catch (...)
{
    return none;
}




optional<std::string> stdtime_attachment_veryfication(const json &json_attach)
{
    if (!json_attach.is_object ())
    {
        return none;
    }

    /// 检查每一个字段
    /*--------------------------------------------------*/
    auto iter = json_attach.find ("产品");
    if (iter == json_attach.end ())
    {
        return none;
    }
    if (!iter->is_string ())
    {
        return none;
    }

    /*--------------------------------------------------*/
    iter = json_attach.find ("作业员");
    if (iter == json_attach.end ())
    {
        return none;
    }
    if (!iter->is_string ())
    {
        return none;
    }


    /*--------------------------------------------------*/
    iter = json_attach.find ("工站号");
    if (iter == json_attach.end ())
    {
        return none;
    }
    if (!iter->is_string ())
    {
        return none;
    }

    /*--------------------------------------------------*/
    iter = json_attach.find ("数据单位");
    if (iter == json_attach.end ())
    {
        return none;
    }
    if (!iter->is_string ())
    {
        return none;
    }
    std::string unit = *iter;
    if (unit!= "秒" and unit!="分钟" and unit!="小时")
    {
        return none;
    }

    /*--------------------------------------------------*/
    iter = json_attach.find ("测量人");
    if (iter == json_attach.end ())
    {
        return none;
    }
    if (!iter->is_string ())
    {
        return none;
    }

    /*--------------------------------------------------*/
    iter = json_attach.find ("测量日期");
    if (iter == json_attach.end ())
    {
        return none;
    }
    if (!iter->is_string ())
    {
        return none;
    }
    std::string str_date = *iter;

    auto date_from_str = QDate::fromString (str_date.data (), Qt::ISODate);

    if (!date_from_str.isValid ())
    {
        return none;
    }


    /*--------------------------------------------------*/
    iter = json_attach.find ("测量方法");
    if (iter == json_attach.end ())
    {
        return none;
    }
    if (!iter->is_string ())
    {
        return none;
    }
    std::string method_str = *iter;


    return method_str;
}


bool stdtime_chart_veryfication(const json &json_chart, const string& method)
{
    assert (!method.empty ());

    if (!json_chart.is_object ())
    {
        return false;
    }

    auto total_iter = json_chart.find ("信息总览");
    assert ((*total_iter).is_array());
    if (total_iter == json_chart.end ())
    {
        return false;
    }
    if (!stdtime_chart_totalinfo_veryfication (*total_iter, method.data ()))
    {
        return false;
    }

    auto detail_iter = json_chart.find ((method.data () + std::string ("详细信息")).data ());
    if (detail_iter == json_chart.end())
    {
        return false;
    }
    if (!stdtime_chart_detailinfo_veryfication (*detail_iter, method))
    {
        return false;
    }

    return true;
}


bool stdtime_chart_totalinfo_veryfication(const json &json_chart_total, const string& method)
{
    if (method == "预定工时法")
    {
        if (!pts_total_verify (json_chart_total))
        {
            return false;
        }
    }
    else if (method == "视频分析法")
    {
        if (!video_total_verify (json_chart_total))
        {
            return false;
        }
    }
    else if (method == "标准资料法")
    {
        if (!stddata_total_verify (json_chart_total))
        {
            return false;
        }
    }

    return true;
}


bool stdtime_chart_video(const json &json_video)
{
    if (!json_video.is_object ())
    {
        return false;
    }

    auto iter_task = json_video.find ("作业内容");
    if (iter_task == json_video.end ())
    {
        return false;
    }
    if (!video_task_verify (*iter_task))
    {
        return false;
    }

    auto iter_data = json_video.find ("观测时间");
    if (iter_data == json_video.end ())
    {
        return false;
    }
    if (!video_data_verify (*iter_data))
    {
        return false;
    }

    auto iter_result = json_video.find ("结果");
    if (iter_result == json_video.end ())
    {
        return false;
    }
    if (!video_result_verify (*iter_result))
    {
        return false;
    }

    auto iter_path = json_video.find ("视频路径");
    if (iter_path == json_video.end ())
    {
        return false;
    }
    if (!iter_path->is_string ())
    {
        return false;
    }

    auto iter_circulation = json_video.find ("循环");
    if (iter_circulation == json_video.end ())
    {
        return false;
    }
    if (!iter_circulation->is_number_integer ())
    {
        return false;
    }

    auto iter_invalid = json_video.find ("无效时间段");
    if (iter_invalid == json_video.end ())
    {
        return false;
    }
    if (!iter_invalid->is_array ())
    {
        return false;
    }

    return true;
}

bool stdtime_chart_pts(const json &json_pts)
{
    if (!json_pts.is_object())
    {
        return false;
    }

    auto item = json_pts.find ("方法");
    if (item == json_pts.end ())
    {
        return false;
    }
    if (!item->is_string())
    {
        return false;
    }

    item = json_pts.find ("左手");
    if (item == json_pts.end ())
    {
        return false;
    }
    else if (!pts_left_verify (*item))
    {
        return false;
    }

    item = json_pts.find ("右手");
    if (item == json_pts.end ())
    {
        return false;
    }
    else if (!pts_right_verify (*item))
    {
        return false;
    }

    item = json_pts.find ("总计");
    if (item == json_pts.end ())
    {
        return false;
    }
    else if (!pts_result_verify (*item))
    {
        return false;
    }

    return true;
}


bool stdtime_chart_stdinfo(const json &data)
{
    auto iter = data.find ("附加信息");
    if (iter == data.end ())
    {
        return false;
    }
    auto optional_method = stdtime_attachment_veryfication (*iter);
    if (!optional_method)
    {
        return false;
    }

    iter = data.find ("表");
    if (iter == data.end ())
    {
        return false;
    }
    if (!stdtime_chart_veryfication (*iter,optional_method->data ()))
    {
        return false;
    }

    //if (*optional_method == "视频分析法")
    //{
    //    auto iter_cir = iter->find ("视频属性");
    //    if (iter_cir == iter->end ())
    //    {
    //        return false;
    //    }
    //    if (!iter_cir->is_object ())
    //    {
    //        return false;
    //    }
    //}

    //auto total_iter = iter->find ("信息总览");
    //if (total_iter == iter->end ())
    //{
    //    return false;
    //}
    //if (!total_iter->is_array ())
    //{
    //    return false;
    //}

    //if (!stdtime_chart_totalinfo_veryfication (*total_iter, optional_method->data ()))
    //{
    //    return false;
    //}

    return true;
}


static bool pts_left_verify (const json& left_hand)
{
    if (left_hand.is_array() != true)
    {
        return false;
    }

    /// 读取每一列数据
    for (int i=0; i<static_cast<int>(left_hand.size ()); ++i)
    {
        int legal_colume = 0;
        const json& row_data = left_hand [i];
        if (row_data.is_object () != true)
        {
            return false;
        }
        /*------------------------------------------------*/
        auto item = row_data.find ("作业内容"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_string () != true)
        {
            return false;
        }

        /*------------------------------------------------*/

        item = row_data.find ("代码"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_array () != true)
        {
            return false;
        }

        /*------------------------------------------------*/

        item = row_data.find ("数量*频次"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_number_integer () != true)
        {
            return false;
        }

        /*------------------------------------------------*/
        item = row_data.find ("MOD/TMU"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_number_integer () != true)
        {
            return false;
        }

        /*------------------------------------------------*/
        item = row_data.find ("评比系数"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (!item->is_number ())
        {
            return false;
        }

        /*------------------------------------------------*/
        item = row_data.find ("基本时间"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (!item->is_number ())
        {
            return false;
        }

    }

    return true;
}



static bool pts_right_verify (const json& right_hand)
{
    if (right_hand.is_array() != true)
    {
        return false;
    }

    /// 每一列数据
    for (int i=0; i < static_cast<int>(right_hand.size ()); ++i)
    {
        int legal_colume = 0;
        /*------------------------------------------------*/
        const json& row_data = right_hand [i];
        if (row_data.is_object () != true)
        {
            return false;
        }

        /*------------------------------------------------*/
        auto item = row_data.find ("作业内容"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_string () != true)
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("代码"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_array () != true)
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("数量*频次"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_number_integer () != true)
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("MOD/TMU"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_number_integer () != true)
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("评比系数"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (!item->is_number ())
        {
            return false;
        }

        /*------------------------------------------------*/

        item = row_data.find ("基本时间"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (!item->is_number ())
        {
            return false;
        }
    }

    return true;
}



static bool pts_result_verify (const json& result)
{
    if (result.is_array() != true)
    {
        return false;
    }

    /// 每一列数据
    for (unsigned i=0; i<result.size (); ++i)
    {
        int legal_colume = 0;
        /*------------------------------------------------*/
        const json& row_data = result [i];
        if (row_data.is_object () != true)
        {
            return false;
        }


        /*------------------------------------------------*/
        auto item = row_data.find ("作业内容"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_string () != true)
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("代码"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_array () != true)
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("数量*频次"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_number_integer () != true)
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("MOD/TMU"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_number_integer () != true)
        {
            return false;
        }


       /*------------------------------------------------*/
        item = row_data.find ("评比系数"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (!item->is_number ())
        {
            return false;
        }

        /*------------------------------------------------*/

        item = row_data.find ("基本时间"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (!item->is_number ())
        {
            return false;
        }



        /*------------------------------------------------*/
        item = row_data.find ("宽放率"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_string () != true)
        {
            return false;
        }
        else
        {
            std::string awn_str = *item;
            auto pos = awn_str.find_first_of ('%');
            if (pos == std::string::npos)
            {
                return false;
            }

            try
            {
                auto before_percent = awn_str.substr(0,pos);
                boost::trim (before_percent);
                boost::lexical_cast <float> (before_percent.data ());
            }
            catch (...)
            {
                return false;
            }

            auto after_percent = awn_str.substr(pos+1,awn_str.size () - (pos+1));
            pos = after_percent.find_first_not_of(' ',0);
            if (pos != std::string::npos)
            {
                return false;
            }
        }


        /*------------------------------------------------*/
        item = row_data.find ("标准时间"); ++legal_colume;
        if (item == row_data.end ())
        {
            return false;
        }
        if (item->is_number () != true)
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("增值/非增值"); ++legal_colume;
        if (item == row_data.end ())
        {
            return false;
        }
        if (!item->is_string ())
        {
            return false;
        }

        std::string val = (*item);

        if (val!="N/A" and val!="增值" and val!="非增值")
        {
            return false;
        }

        val = (*item);

        if (val!="加工" and val!="检查" and val!="搬运" and val!="等待" and val!="")
        {
            return false;
        }

    }

    return true;
}




static bool pts_total_verify (const json& total)
{
    if (total.is_array() != true)
    {
        return false;
    }


    /// 每一列数据
    for (unsigned i=0; i<total.size (); ++i)
    {
        int legal_colume = 0;
        /*------------------------------------------------*/
        const json& row_data = total [i];
        if (row_data.is_object () != true)
        {
            return false;
        }


        /*------------------------------------------------*/
        if (file_source == "工时资料库")
        {
            auto iter_company_code = row_data.find ("公司内部代码");
            if (iter_company_code == row_data.end ())
            {
                return false;
            }
            if (!iter_company_code->is_string ())
            {
                return false;
            }

            auto iter_history_source = row_data.find ("历史来源");
            if (iter_history_source == row_data.end ())
            {
                return false;
            }
            if (!iter_history_source->is_string ())
            {
                return false;
            }

            auto iter_std_task = row_data.find ("标准操作单元作业内容");
            if (iter_std_task == row_data.end ())
            {
                return false;
            }
            if (!iter_std_task->is_string ())
            {
                return false;
            }

            auto iter_std_basic_time = row_data.find ("标准操作单元基本时间");
            if (iter_std_basic_time == row_data.end ())
            {
                return false;
            }
            if (!iter_std_basic_time->is_number ())
            {
                return false;
            }
        }


        /*------------------------------------------------*/
        auto item = row_data.find ("作业内容"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_string () != true)
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("预定工时法总代码"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_string () != true)
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("数量*频次"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_number_integer () != true)
        {
            return false;
        }



       /*------------------------------------------------*/
        item = row_data.find ("评比系数"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (!item->is_number ())
        {
            return false;
        }

        /*------------------------------------------------*/

        item = row_data.find ("基本时间"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (!item->is_number ())
        {
            return false;
        }



        /*------------------------------------------------*/
        item = row_data.find ("宽放率"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_string () != true)
        {
            return false;
        }
        else
        {
            std::string awn_str = *item;
            auto pos = awn_str.find_first_of ('%');
            if (pos == std::string::npos)
            {
                return false;
            }

            try
            {
                auto before_percent = awn_str.substr(0,pos);
                boost::trim (before_percent);
                boost::lexical_cast <float> (before_percent.data ());
            }
            catch (...)
            {
                return false;
            }

            auto after_percent = awn_str.substr(pos+1,awn_str.size () - (pos+1));
            pos = after_percent.find_first_not_of(' ',0);
            if (pos != std::string::npos)
            {
                return false;
            }
        }


        /*------------------------------------------------*/
        item = row_data.find ("标准时间"); ++legal_colume;
        if (item == row_data.end ())
        {
            return false;
        }
        if (item->is_number () != true)
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("增值/非增值"); ++legal_colume;
        if (item == row_data.end ())
        {
            return false;
        }
        if (!item->is_string ())
        {
            return false;
        }

        std::string val = (*item);

        if (val!="N/A" and val!="增值" and val!="非增值")
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("操作分类"); ++legal_colume;
        if (item == row_data.end ())
        {
            return false;
        }
        if (!item->is_string ())
        {
            return false;
        }

        val = (*item);

        if (val!="加工" and val!="检查" and val!="搬运" and val!="等待" and val!="")
        {
            return false;
        }
    }
    return true;
}


static bool video_total_verify (const json& total)
{
    if (total.is_object())
    {
        QDialog dlg;
        dlg.exec();
        return true;
    }
    if (total.is_array() != true)
    {
        return false;
    }


    /// 每一列数据
    for (unsigned i=0; i<total.size (); ++i)
    {
        int legal_colume = 0;
        /*------------------------------------------------*/
        const json& row_data = total [i];
        if (row_data.is_object () != true)
        {
            return false;
        }

        /*------------------------------------------------*/
        if (file_source == "工时资料库")
        {
            auto iter_company_code = row_data.find ("公司内部代码");
            if (iter_company_code == row_data.end ())
            {
                return false;
            }
            if (!iter_company_code->is_string ())
            {
                return false;
            }
            auto iter_history_source = row_data.find ("历史来源");
            if (iter_history_source == row_data.end ())
            {
                return false;
            }
            if (!iter_history_source->is_string ())
            {
                return false;
            }

            auto iter_std_task = row_data.find ("标准操作单元作业内容");
            if (iter_std_task == row_data.end ())
            {
                return false;
            }
            if (!iter_std_task->is_string ())
            {
                return false;
            }

            auto iter_std_basic_time = row_data.find ("标准操作单元基本时间");
            if (iter_std_basic_time == row_data.end ())
            {
                return false;
            }
            if (!iter_std_basic_time->is_number ())
            {
                return false;
            }

        }

        /*------------------------------------------------*/
        auto item = row_data.find ("作业内容"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_string () != true)
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("评比系数"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (!item->is_number ())
        {
            return false;
        }

        /*------------------------------------------------*/
        item = row_data.find ("测量时间");
        if (item == row_data.end ())
        {
            return false;
        }
        if (!item->is_number ())
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("基本时间"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (!item->is_number ())
        {
            return false;
        }



        /*------------------------------------------------*/
        item = row_data.find ("宽放率"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_string () != true)
        {
            return false;
        }
        else
        {
            std::string awn_str = *item;
            auto pos = awn_str.find_first_of ('%');
            if (pos == std::string::npos)
            {
                return false;
            }

            try
            {
                auto before_percent = awn_str.substr(0,pos);
                boost::trim (before_percent);
                boost::lexical_cast <float> (before_percent.data ());
            }
            catch (...)
            {
                return false;
            }

            auto after_percent = awn_str.substr(pos+1,awn_str.size () - (pos+1));
            pos = after_percent.find_first_not_of(' ',0);
            if (pos != std::string::npos)
            {
                return false;
            }
        }


        /*------------------------------------------------*/
        item = row_data.find ("标准时间"); ++legal_colume;
        if (item == row_data.end ())
        {
            return false;
        }
        if (item->is_number () != true)
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("增值/非增值"); ++legal_colume;
        if (item == row_data.end ())
        {
            return false;
        }
        if (!item->is_string ())
        {
            return false;
        }

        std::string val = (*item);

        if (val!="N/A" and val!="增值" and val!="非增值")
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("操作分类"); ++legal_colume;
        if (item == row_data.end ())
        {
            return false;
        }
        if (!item->is_string ())
        {
            return false;
        }

        val = (*item);

        if (val!="加工" and val!="检查" and val!="搬运" and val!="等待" and val!="")
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("视频路径"); ++legal_colume;
        if (item == row_data.end ())
        {
            return false;
        }
        if (!item->is_string ())
        {
            return false;
        }
    }

    return true;
}


static bool video_task_verify (const json& task)
{
    if (!task.is_array ())
    {
        return false;
    }

    for (unsigned i=0; i< task.size (); ++i)
    {
        auto part = task [i];
        if (!part.is_string ())
        {
            return false;
        }

        std::string task_data = part;
        if (task_data.empty ())
        {
            return false;
        }
    }

    return true;
}


static bool video_data_verify (const json& data)
{
    if (!data.is_array())
    {
        return false;
    }

    for (size_t i=0; i<data.size (); ++i)
    {
        auto& cir = data [i];
        if (!cir.is_array ())
        {
            return false;
        }
        for (size_t j = 0; j<cir.size (); ++j)
        {
            auto& time_quantum = cir [j];
            if (!time_quantum.is_object ())
            {
                return false;
            }

            auto iter_T = time_quantum.find ("T");
            if (iter_T == time_quantum.end ())
            {
                return false;
            }
            if (!iter_T->is_number ())
            {
                return false;
            }

            auto iter_R = time_quantum.find ("R");
            if (iter_R == time_quantum.end ())
            {
                return false;
            }
            if (!iter_R->is_number ())
            {
                return false;
            }

            //if (i == 0)
            //{
            //    //double T = *iter_T;
            //    //double R = *iter_R;
            //    //if (T != R)  //// bug such as :   1.4 != 1.3000000443534
            //    //{
            //    //    return false;
            //    //}
            //}
            //else
            //{
            //    /// if ((T - last_T) != R)
            //    /// {
            //    ///     ......
            //    /// }
            //}
        }
    }

    return true;
}

static bool video_result_verify (const json& result)
{
    if (result.is_array() != true)
    {
        return false;
    }

    /// 每一列数据
    for (size_t i=0; i<result.size (); ++i)
    {
        int legal_colume = 0;
        /*------------------------------------------------*/
        const json& row_data = result [i];
        if (row_data.is_object () != true)
        {
            return false;
        }

       /*------------------------------------------------*/
        auto item = row_data.find ("平均时间"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (!item->is_number ())
        {
            return false;
        }

       /*------------------------------------------------*/
        item = row_data.find ("评比系数"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (!item->is_number ())
        {
            return false;
        }

        /*------------------------------------------------*/

        item = row_data.find ("基本时间"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (!item->is_number ())
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("宽放率"); ++legal_colume;
        if (item == row_data.end())
        {
            return false;
        }
        if (item->is_string () != true)
        {
            return false;
        }
        else
        {
            std::string awn_str = *item;
            auto pos = awn_str.find_first_of ('%');
            if (pos == std::string::npos)
            {
                return false;
            }

            try
            {
                auto before_percent = awn_str.substr(0,pos);
                boost::trim (before_percent);
                boost::lexical_cast <float> (before_percent.data ());
            }
            catch (...)
            {
                return false;
            }

            auto after_percent = awn_str.substr(pos+1,awn_str.size () - (pos+1));
            pos = after_percent.find_first_not_of(' ',0);
            if (pos != std::string::npos)
            {
                return false;
            }
        }


        /*------------------------------------------------*/
        item = row_data.find ("标准时间"); ++legal_colume;
        if (item == row_data.end ())
        {
            return false;
        }
        if (item->is_number () != true)
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("增值/非增值"); ++legal_colume;
        if (item == row_data.end ())
        {
            return false;
        }
        if (!item->is_string ())
        {
            return false;
        }

        std::string val = (*item);

        if (val!="N/A" and val!="增值" and val!="非增值")
        {
            return false;
        }


        /*------------------------------------------------*/
        item = row_data.find ("操作分类"); ++legal_colume;
        if (item == row_data.end ())
        {
            return false;
        }
        if (!item->is_string ())
        {
            return false;
        }

        val = (*item);

        if (val!="加工" and val!="检查" and val!="搬运" and val!="等待" and val!="")
        {
            return false;
        }
    }

    return true;
}


bool method_detection(const json &data, const std::string& method)
{
    auto iter1 = data.find ("附加信息");
    assert (iter1 != data.end () and iter1->is_object ());
    auto iter2 = iter1->find ("测量方法");
    assert (iter2 != iter1->end () and iter2->is_string ());

    return *iter2 == method;
}

bool stdtime_chart_proinfo(const json &data)
{
    auto iter = data.find ("附加信息");
    if (iter == data.end ())
    {
        return false;
    }
    auto attachment_data = stdtime_attachment_veryfication (*iter);
    if (!attachment_data)
    {
        return false;
    }

    iter = data.find ("表");
    if (iter == data.end ())
    {
        return false;
    }
    if (!stdtime_chart_veryfication (*iter,attachment_data->data ()))
    {
        return false;
    }

    return true;
}

bool video_attribute_verify (const json& attribute)
{
    if (!attribute.is_object ())
    {
        return false;
    }

    auto iter_section = attribute.find ("视频播放时间段");
    if (iter_section == attribute.end ())
    {
        return false;
    }
    if (!iter_section->is_array ())
    {
        return false;
    }
    auto& sections = *iter_section;
    for (size_t i=0; i<sections.size (); ++i)
    {
        auto& section = sections [i];
        if (!section.is_object ())
        {
            return false;
        }
        auto iter_start_time = section.find ("开始时间");
        if (iter_start_time == section.end ())
        {
            return false;
        }
        if (!iter_start_time->is_number ())
        {
            return false;
        }
        auto iter_end_time = section.find ("结束时间");
        if (iter_end_time == section.end ())
        {
            return false;
        }
        if (!iter_end_time->is_number ())
        {
            return false;
        }
    }

    auto iter_invalid = attribute.find ("视频无效时间段");
    if (iter_invalid == attribute.end ())
    {
        return false;
    }
    if (!iter_invalid->is_array ())
    {
        return false;
    }

    return true;
}

bool stdtime_chart_stddata(const json &json_stddata)
{
    auto iter_detail = json_stddata.find ("详细信息");
    if (iter_detail == json_stddata.end ())
    {
        return false;
    }
    if (!iter_detail->is_array ())
    {
        return false;
    }
    auto& detail = *iter_detail;

    if (detail.size () == 0)
    {
        return false;
    }

    for (size_t i = 0; i<detail.size (); ++i)
    {
        auto& row_data = detail [i];
        if (!row_data.is_object ())
        {
            return false;
        }
        if (row_data.size () == 0)
        {
            return false;
        }

        auto iter_task = row_data.find ("作业内容");
        if (iter_task == row_data.end ())
        {
            return false;
        }
        if (!iter_task->is_string ())
        {
            return false;
        }

        auto iter_company_code = row_data.find ("公司内部代码");
        if (iter_company_code == row_data.end ())
        {
            return false;
        }
        if (!iter_company_code->is_string ())
        {
            return false;
        }

        auto iter_history_source = row_data.find ("历史来源");
        if (iter_history_source == row_data.end ())
        {
            return false;
        }
        if (!iter_history_source->is_string ())
        {
            return false;
        }

        auto iter_basic_time = row_data.find ("基本时间");
        if (iter_basic_time == row_data.end ())
        {
            return false;
        }
        if (!iter_basic_time->is_number ())
        {
            return false;
        }

        auto iter_added = row_data.find ("增值/非增值");
        if (iter_added == row_data.end ())
        {
            return false;
        }
        if (!iter_added->is_string ())
        {
            return false;
        }

        auto iter_allowance = row_data.find ("宽放率");
        if (iter_allowance == row_data.end ())
        {
            return false;
        }
        if (!iter_allowance->is_string ())
        {
            return false;
        }

        auto iter_opt = row_data.find ("操作分类");
        if (iter_opt == row_data.end ())
        {
            return false;
        }
        if (!iter_opt->is_string ())
        {
            return false;
        }
        std::string val = (*iter_opt);
        if (val!="加工" and val!="检查" and val!="搬运" and val!="等待" and val!="")
        {
            return false;
        }

        auto iter_count = row_data.find ("数量*频次");
        if (iter_count == row_data.end ())
        {
            return false;
        }
        if (iter_count->is_number_integer () != true)
        {
            return false;
        }

        auto iter_std_task = row_data.find ("标准操作单元作业内容");
        if (iter_std_task == row_data.end ())
        {
            return false;
        }
        if (!iter_std_task->is_string ())
        {
            return false;
        }

        auto iter_std_basic_time = row_data.find ("标准操作单元基本时间");
        if (iter_std_basic_time == row_data.end ())
        {
            return false;
        }
        if (!iter_std_basic_time->is_number ())
        {
            return false;
        }

        auto iter_stdtime = row_data.find ("标准时间");
        if (iter_stdtime == row_data.end ())
        {
            return false;
        }
        if (!iter_stdtime->is_number ())
        {
            return false;
        }

        auto iter_video_path = row_data.find ("视频路径");
        if (iter_video_path == row_data.end ())
        {
            return false;
        }
        if (!iter_video_path->is_string ())
        {
            return false;
        }

        auto iter_pts_code = row_data.find ("预定工时法总代码");
        if (iter_pts_code == row_data.end ())
        {
            return false;
        }
        if (!iter_pts_code->is_string ())
        {
            return false;
        }
    }

    return true;
}


bool stdtime_chart_detailinfo_veryfication(const json &json_chart, const std::string &method)
{
    if (file_source == "工时资料库")
    {
        if (method == "视频分析法")
        {
            auto iter_attribute = json_chart.find ("视频属性");
            if (iter_attribute == json_chart.end ())
            {
                return false;
            }
            if (!video_attribute_verify (*iter_attribute))
            {
                return false;
            }
        }
        else if (method == "标准资料法")
        {
            if (!stdtime_chart_stddata (json_chart))
            {
                return false;
            }
        }
    }
    else if (file_source == "产品工时")
    {
        if (method == "预定工时法")
        {
            if (!stdtime_chart_pts (json_chart))
            {
                return false;
            }
        }
        else if (method == "视频分析法")
        {
            if (!stdtime_chart_video (json_chart))
            {
                return false;
            }
        }
    }

    return true;
}



static bool stddata_total_verify (const json& total_data)
{
    if (!total_data.is_array ())
    {
        return false;
    }
    if (total_data.size () == 0)
    {
        return false;
    }

    for (size_t i = 0; i<total_data.size (); ++i)
    {
        auto& row_data = total_data [i];
        if (!row_data.is_object ())
        {
            return false;
        }
        if (row_data.empty ())
        {
            return false;
        }

        auto iter_task = row_data.find ("作业内容");
        if (iter_task == row_data.end ())
        {
            return false;
        }
        if (!iter_task->is_string ())
        {
            return false;
        }

        auto iter_company_code = row_data.find ("公司内部代码");
        if (iter_company_code == row_data.end ())
        {
            return false;
        }
        if (!iter_company_code->is_string ())
        {
            return false;
        }

        auto iter_basic_time = row_data.find ("基本时间");
        if (iter_basic_time == row_data.end ())
        {
            return false;
        }
        if (!iter_basic_time->is_number ())
        {
            return false;
        }

        auto iter_allowance = row_data.find ("宽放率");
        if (iter_allowance == row_data.end ())
        {
            return false;
        }
        if (!iter_allowance->is_string ())
        {
            return false;
        }

        auto iter_count = row_data.find ("数量*频次");
        if (iter_count == row_data.end ())
        {
            return false;
        }
        if (iter_count->is_number_integer () != true)
        {
            return false;
        }

        auto iter_stdtime = row_data.find ("标准时间");
        if (iter_stdtime == row_data.end ())
        {
            return false;
        }
        if (!iter_stdtime->is_number ())
        {
            return false;
        }
    }

     return true;
}
