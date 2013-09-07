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

#ifndef REQUEST_H_
#define REQUEST_H_

#include <string>
#include <weibo_api.h>
#include <weibo_param.h>
#include <net/http.h>
#include <map>
using std::map;
using std::string;

struct wb_request_t {
    string url;
    weibo_param param;
    weibo_callback* cb;
};


class request : public http_callback
{
public:
    request();
    request(const request& other);
    request& operator=(const request& other);
    ~request();

    static request* get_instance();

    HTTP_CODE sync_request(const string& strUrl,
                                 HTTP_METHOD method,
                                 weibo_param param,
                                 string& data);

    bool async_request(const string& url,
                       HTTP_METHOD method,
                       weibo_param param,
                       weibo_callback* cb);

    virtual void on_http_complete(http* http_ptr,
                                  HTTP_CODE errcode,
                                  string& data);

private:
    typedef map<http*, wb_request_t> http_n_request_map;
    http_n_request_map m_map;
    pthread_mutex_t m_mutex;
};

#endif /* REQUEST_H_ */
