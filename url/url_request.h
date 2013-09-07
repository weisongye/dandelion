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

#ifndef REQUEST_URL_H_
#define REQUEST_URL_H_

#include <map>
#include <weibo_param.h>

class url_request
{
public:
    ~url_request(void);

    static url_request* get_instance();

    string get_url(const string& url, HTTP_METHOD method,
                   weibo_param& param, string& post_str);

    string get_post_string(weibo_param param);

private:
    string get_base_url(const string& url, weibo_param param);

    string normalrize_url(const string& url,
                          HTTP_METHOD method,
                          const string& custom_key,
                          const string& custom_secrect,
                          const string& token_key,
                          const string& token_secrect,
                          string& post_str);
private:
    url_request();
    url_request(const url_request& other);
    url_request& operator=(const url_request& other);

private:
    typedef std::map<int, string> URL_MAP;
    URL_MAP m_url_map;
};

#endif /* REQUEST_URL_H_ */