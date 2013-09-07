/*
 * Copyright (C) 2013 Phoebus Veiz <phoebusveiz@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <ctime>

#include <assert.h>
#include <log.h>
#include <weibo_api.h>
#include "defs.h"
#include "robot.h"
#include "auth.h"
#include "net/net.h"
#include "weibo_param.h"
#include "weibo.h"

using namespace std;

robot::robot()
{
    m_inserted = false;

    m_authorization = new auth();

    if (m_authorization->login() != 0) {
        exit(0);
    }

    m_line.reserve(51200);
    m_weibos.reserve(256);
    m_block_list.reserve(512);
    m_erase_list.reserve(64);
    m_black_list.reserve(32);
    m_rss_list.reserve(16);

    load_block_list();
    load_erase_list();
    load_black_list();
}

robot::~robot()
{
    if (m_authorization != NULL) {
        delete m_authorization;
    }

    m_block_list.clear();
    m_erase_list.clear();
    m_black_list.clear();
    m_rss_list.clear();
}

bool robot::load_list(vector<string>& vec, const char* filename)
{
    ifstream is;

    is.open(filename, ios::in);

    if (!is) {
        log(string(filename) + " not found!");
        return false;
    }

    vec.clear();
    string item;

    while (getline(is, item)) {
        if (item[item.size() - 1] == '\r') {
            item = item.substr(0, item.size() - 1);
        }

        item =  util::str::mbcs_to_utf8(item);

        if (item.size() == 0) {
            break;
        }

        vec.push_back(item);
    }

    is.close();
    return true;
}

bool robot::load_block_list()
{
    return load_list(m_block_list, "config/block.lst");
}

bool robot::load_erase_list()
{
    return load_list(m_erase_list, "config/erase.lst");
}

bool robot::load_black_list()
{
    return load_list(m_black_list, "config/black.lst");
}

bool robot::load_rss_list()
{
    return load_list(m_rss_list, "config/rss.lst");
}

void robot::get_news_list()
{
    load_rss_list();

    m_weibos.clear();

    if (m_inserted == false) {
        load_list(m_inserted_weibo, "config/inserted.txt");

        for (vector<string>::size_type i = 0;
             i < m_inserted_weibo.size(); i++) {
            weibo wb;
            wb.content = m_inserted_weibo[i];
            m_weibos.push_back(wb);
        }

        m_inserted = true;
    }

    parser::parse_xml_group(m_rss_list, m_weibos, m_block_list, m_erase_list);
}

string robot::get_news_content(int i)
{
    return (m_weibos[i].content);
}

string& robot::get_news_url(int i)
{
    return (m_weibos[i].url);
}

void robot::get_longitude(char* l)
{
    // 116.211 -> 116.544, 39.9076 : Beijing
    // 113.908 -> 114.156, 22.538 : Shenzhen
    const double longitude_start = 116.211;
    const double longitude_end = 116.544;
    double longitude_diff = longitude_end - longitude_start;

    time_t timer = time(0);
    struct tm* t = localtime(&timer);
    int hour = t->tm_hour;
    int min = t->tm_min;
    int sec = t->tm_sec;

    double longitude = 0;

    if (hour % 2 == 0) {
        longitude =
            longitude_start + longitude_diff / 60 * (min + double(sec / 60));
    } else {
        longitude =
            longitude_end - longitude_diff / 60 * (min + double(sec / 60));
    }

    sprintf(l, "%f", longitude);
}

string robot::get_pic_url(const string& url)
{
#ifdef WIN32
    const char* local_filename = "webpage.html";
#else
    const char* local_filename = "/tmp/webpage.html";
#endif /* WIN32 */

    net::download(url.c_str(), local_filename);

    ifstream ifs;

    string ret = "";
    ret.reserve(600);

    ifs.open(local_filename, ios::in | ios::binary);

    if (!ifs) {
        log(string(local_filename) + ": file not found.");
    }

    ifs.seekg(0);
    m_line.clear();

    string search_tags[] = {
        "<IMG alt=",
        "<img alt=",
        "<IMG ALT=",
        "<img ALT=",
    };


    while (getline(ifs, m_line)) {
        string::size_type pos;

        pos = m_line.find("center");

        if (pos == string::npos) {
            continue;
        }

        for (int i = 0; i < 4; i++) {
            pos = m_line.find(search_tags[i]);

            if (pos == string::npos) {
                continue;
            } else {
                ret = m_line.substr(pos);

                pos = ret.find("http");

                if (pos == string::npos) {
                    break;
                }

                ret = ret.substr(pos);
                pos = ret.find("\">");

                if (pos == string::npos) {
                    break;
                }

                ret = ret.substr(0, pos);
                goto out;
            }
        }
    }

out:
    ifs.close();
    return ret;
}

int robot::send_news()
{
start:
    log("Download RSS xml in the list...");
    get_news_list();

    if (m_weibos.size() == 0) {
        log("No new news!");
        get_trends_t("1");
    } else {
        load_block_list();
        load_erase_list();
        load_black_list();
    }

    for (size_t i = 0; i < m_weibos.size(); ++i) {
        time_t timer = time(0);
        struct tm* t = localtime(&timer);
        int hour = t->tm_hour;

        weibo_param param;

        param.add_param(tencent_weibo::param::str_custom_key,
            m_authorization->get_app_key());
        param.add_param(tencent_weibo::param::str_custom_secrect,
            m_authorization->get_app_secret());
        param.add_param(tencent_weibo::param::str_token_key,
            m_authorization->get_access_key());
        param.add_param(tencent_weibo::param::str_token_secrect,
            m_authorization->get_access_secret());

        WEIBO_API_TYPE option = TXWB_T_ADD_PIC_URL;

        param.add_param("format", "xml");
        string content = get_news_content(i);
        param.add_param("content", content);

        char longitude[64] = {0};

        if ((hour >= 9 && hour < 12) || (hour >= 14 && hour < 18)) {
            get_longitude(longitude);
            param.add_param("longitude", longitude);
            param.add_param("latitude", "39.9076");

            param.add_param("syncflag", "0");
        } else {
            param.add_param("syncflag", "1");

            string pic_url = get_pic_url(get_news_url(i));

            if (pic_url.size() > 0) {
                param.add_param("pic_url", pic_url);
            }
        }

        if (async_request_by_type(option, param, this)) {
            log("Sent " + content);
        } else {
            log("ERROR: Send news FAILED.");
            return -1;
        }

#if (defined(USE_WGET) || defined(USE_CURL))
        delay(DELAY_TIME);
#else
        delay(DELAY_TIME / 2);
#endif
        get_my_fanslist("1");
        delay(DELAY_TIME);
    }

#ifndef DEBUG_MODE
    goto start;
#endif
}

int robot::get_my_fanslist(const string& count)
{
    int ret;

    weibo_param param;

    param.add_param(tencent_weibo::param::str_custom_key,
        m_authorization->get_app_key());
    param.add_param(tencent_weibo::param::str_custom_secrect,
        m_authorization->get_app_secret());
    param.add_param(tencent_weibo::param::str_token_key,
        m_authorization->get_access_key());
    param.add_param(tencent_weibo::param::str_token_secrect,
        m_authorization->get_access_secret());

    WEIBO_API_TYPE option = TXWB_FRIENDS_USER_FANSLIST_S;

    param.add_param("format", "xml");
    param.add_param("reqnum", count);
    param.add_param("startindex", "0");
    param.add_param("mode", "0");

    ret = async_request_by_type(option, param, this);

    if (ret) {
        log("Sent get fanslist requst.");
    } else {
        log("Send get fanslist request failed.");
    }

    return ret;
}

int robot::get_trends_t(const string& count)
{
    int ret;

    weibo_param param;

    param.add_param(tencent_weibo::param::str_custom_key,
        m_authorization->get_app_key());
    param.add_param(tencent_weibo::param::str_custom_secrect,
        m_authorization->get_app_secret());
    param.add_param(tencent_weibo::param::str_token_key,
        m_authorization->get_access_key());
    param.add_param(tencent_weibo::param::str_token_secrect,
        m_authorization->get_access_secret());

    WEIBO_API_TYPE option = TXWB_TRENDS_T;

    param.add_param("format", "xml");
    param.add_param("reqnum", count);
    param.add_param("pos", "0");
    param.add_param("type", "0");

    ret = async_request_by_type(option, param, this);

    if (ret) {
        log("Sent get trends_t request.");
    } else {
        log("Send get trends_t request failed.");
    }

    return ret;
}

void robot::on_request_complete(HTTP_CODE ret_code,
                                const string& url,
                                weibo_param param,
                                const string& data)
{
    const string err_string[] = {
        "OK",
        "Request timeout",
        "Cannot resolve address",
        "Cannot connect",
        "Error",
        "Large received data",
        "Failure",
        "Type error, cannot generate available url"
    };

    if (ret_code != HTTP_OK && ret_code <= HTTP_FAILED) {
        log(err_string[ret_code]);
        return;
    }

    if (ret_code == HTTP_OK) {
        log(data);
        if (data.find("<ret>0</ret>") == string::npos) {
            log("[FAILED]\n");
        } else {
            log("[OK]\n");
        }
        handle_response(url, data);
    } else {
    }
}

int robot::handle_response(const string& req_url, const string& response)
{
    if (req_url.find(weibo_get_url(TXWB_FRIENDS_USER_FANSLIST_S)) !=
        string::npos) {
        add_to_blacklist(response);
    } else if (req_url.find(weibo_get_url(TXWB_TRENDS_T)) != string::npos) {
        re_add_trends_t(response);
    }

    return 0;
}

int robot::add_to_blacklist(const string& response)
{
    bool is_black = false;

    for (int i = 0; i < m_black_list.size(); i++) {
        if (response.find(m_black_list[i]) != string::npos) {
            is_black = true;
            break;
        }
    }

    if (is_black == false) {
        return 0;
    }

    string start_str = "<name>";
    string end_str = "</name>";
    string::size_type start = response.find(start_str) + start_str.length();
    string::size_type end = response.find(end_str);
    string::size_type len = end - start;
    string name = response.substr(start, len);

    int ret;

    weibo_param param;

    param.add_param(tencent_weibo::param::str_custom_key,
        m_authorization->get_app_key());
    param.add_param(tencent_weibo::param::str_custom_secrect,
        m_authorization->get_app_secret());
    param.add_param(tencent_weibo::param::str_token_key,
        m_authorization->get_access_key());
    param.add_param(tencent_weibo::param::str_token_secrect,
        m_authorization->get_access_secret());

    WEIBO_API_TYPE option = TXWB_FRIENDS_ADD_BLACKLIST;

    param.add_param("format", "xml");
    param.add_param("name", name);

    ret = async_request_by_type(option, param, this);

    if (ret) {
        log(name + "\thas been added to blacklist!");
    } else {
        cout << "Request failed." << endl;
    }

    return ret;
}

int robot::re_add_trends_t(const string& response)
{
    string start_str = "<id>";
    string end_str = "</id>";
    string::size_type start = response.find(start_str) + start_str.length();
    string::size_type end = response.find(end_str);
    string::size_type len = end - start;
    string reid = response.substr(start, len);

    if (find(m_re_added_trends_t.begin(), m_re_added_trends_t.end(), reid) !=
            m_re_added_trends_t.end()) {
        log(reid + "\thas been sent, no need to send.");
        return 0;
    }

    start_str = "<origtext>";
    end_str = "</origtext>";
    start = response.find(start_str) + start_str.length();
    end = response.find(end_str);
    len = end - start;
    string origtext = response.substr(start, len);

    if (parser::need_to_block(origtext, m_block_list) == true) {
        return -1;
    }

    int ret;

    weibo_param param;

    param.add_param(tencent_weibo::param::str_custom_key,
        m_authorization->get_app_key());
    param.add_param(tencent_weibo::param::str_custom_secrect,
        m_authorization->get_app_secret());
    param.add_param(tencent_weibo::param::str_token_key,
        m_authorization->get_access_key());
    param.add_param(tencent_weibo::param::str_token_secrect,
        m_authorization->get_access_secret());

    WEIBO_API_TYPE option = TXWB_T_RE_ADD;

    param.add_param("format", "xml");
    param.add_param("reid", reid);
    param.add_param("syncflag", "1");

    ret = async_request_by_type(option, param, this);

    if (ret) {
        log("Re Add: " + origtext);

        if (m_re_added_trends_t.size() > 1024) {
            m_re_added_trends_t.pop_front();
        }

        m_re_added_trends_t.push_back(reid);
    } else {
        cout << "Request failed." << endl;
    }

    return ret;
}