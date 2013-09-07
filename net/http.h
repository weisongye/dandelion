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

#ifndef HTTP_H_
#define HTTP_H_

#include <map>

#include "curl/curl.h"
#include "http_proxy.h"
#include "weibo_api.h"

class http;

class http_callback
{
public:
    http_callback() {}

    virtual ~http_callback() {
        http_proxy* proxy = http_proxy::get_instance();

        if (proxy) {
            proxy->remove_callback(this);
        }
    }

public:
    virtual void on_http_complete(http* h,
                                  HTTP_CODE ret_code,
                                  string& data) = 0;
};


class http
{
public:
    http();
    ~http();

public:
    HTTP_CODE sync_download(const string& url, string& ret_str);
    bool async_download(const string& url, http_callback* cb);
    HTTP_CODE sync_upload(const string& url, const string& data,
                          string& ret_str);
    bool async_upload(const string& url, const string& data, http_callback* cb);
    HTTP_CODE sync_upload_form_data(const string& url,
                                    const std::map<string, string>& data_map,
                                    const std::map<string, string>& file_map,
                                    string& ret_str);
    bool async_upload_form_data(const string& url,
                                const std::map<string, string>& data_map,
                                const std::map<string, string>& file_map,
                                http_callback* cb);

    static size_t handle_result(void* data,
                                size_t size,
                                size_t nmemb,
                                void* http_ptr);

    void reset();

    CURLcode perform(HTTP_METHOD method);

    bool write_result_data(char* data, unsigned long len);
    bool get_result_data(string& data);

    bool m_is_running;

private:
    string m_url;
    string m_data;

    typedef std::map<string, string> STR_MAP;
    STR_MAP m_post_file_map;
    STR_MAP m_post_data_map;

    char* m_ret_data;
    unsigned long m_len;
    unsigned long m_write_pos;
};

#endif /* HTTP_H_ */