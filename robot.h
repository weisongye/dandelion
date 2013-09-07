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

#ifndef ROBOT_H_
#define ROBOT_H_

#include <string>
#include <vector>
#include <deque>

#include <weibo_api.h>

#include "auth.h"
#include "parser.h"

using std::string;
using std::vector;
using std::deque;

class robot : public weibo_callback
{
public:
    robot();
    ~robot();
    bool load_list(vector<string>& vec, const char* filename);
    bool load_block_list();
    bool load_erase_list();
    bool load_black_list();
    bool load_rss_list();
    int send_news();
    int handle_response(const string& req_url, const string& response);
    int get_my_fanslist(const string& count);
    int get_trends_t(const string& count);
    int add_to_blacklist(const string& response);
    int re_add_trends_t(const string& response);

protected:
    virtual void on_request_complete(HTTP_CODE ret_code,
                                     const string& url,
                                     weibo_param param,
                                     const string& data);

private:
#ifdef DEBUG_MODE
    const static int DELAY_TIME = 1000;
#else
    const static int DELAY_TIME = 1000 * 60;
#endif
    string m_line;
    bool m_inserted;
    auth* m_authorization;
    vector<weibo> m_weibos;
    deque<string> m_re_added_trends_t;
    vector<string> m_block_list;
    vector<string> m_erase_list;
    vector<string> m_black_list;
    vector<string> m_rss_list;
    vector<string> m_inserted_weibo;
    void get_news_list();
    string get_news_content(int i);
    string& get_news_url(int i);
    void get_longitude(char* l);
    string get_pic_url(const string& url);
};

#endif /* ROBOT_H_ */