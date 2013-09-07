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

#include <assert.h>
#include <pthread.h>

#include <log.h>
#include <defs.h>
#include "request.h"
#include "url/url_request.h"

request::request()
{
    pthread_mutex_init(&m_mutex, NULL);
}

request::~request()
{
    pthread_mutex_destroy(&m_mutex);
}

request* request::get_instance()
{
    static request req;
    return &req;
}

HTTP_CODE request::sync_request(const string& url,
                                HTTP_METHOD method,
                                weibo_param param,
                                string& data)
{
    HTTP_CODE ret_code = HTTP_FAILED;
    string post_str;
    string ret_data_str;
    string oauth_url =
        url_request::get_instance()->get_url(url, method, param, post_str);
    http h;

    if (method == HTTP_METHOD_GET) {
        if (!post_str.empty()) {
            oauth_url += "?";
            oauth_url += post_str;
        }

        ret_code = h.sync_download(oauth_url, ret_data_str);
    } else if (method == HTTP_METHOD_POST) {
        if (param.get_pic_param_map().empty()) {
            ret_code = h.sync_upload(oauth_url, post_str, ret_data_str);
        } else {
            if (!post_str.empty()) {
                oauth_url += "?";
                oauth_url += post_str;
            }

            ret_code = h.sync_upload_form_data(oauth_url, param.get_param_map(),
                param.get_pic_param_map(), ret_data_str);
        }
    }

    data = ret_data_str;
    string format_key = tencent_weibo::param::str_format;
    string format_val = param.get_value(format_key);
    string format;
    format.append(format_val);

    if (format == tencent_weibo::param::str_json ||
        format == tencent_weibo::param::str_xml) {
        string pUrlDecode = util::str::form_url_decode(ret_data_str);
        data = pUrlDecode;
    }

    return ret_code;
}

bool request::async_request(const string& url,
                            HTTP_METHOD method,
                            weibo_param param,
                            weibo_callback* cb)
{
    string post_str;
    bool ret = false;

    string oauth_url =
        url_request::get_instance()->get_url(url, method, param, post_str);

    http* h = new http;

    if (method == HTTP_METHOD_GET) {
        if (!post_str.empty()) {
            oauth_url += "?";
            oauth_url += post_str;
        }

        ret = h->async_download(oauth_url, this);
    } else if (method == HTTP_METHOD_POST) {
        if (param.get_pic_param_map().empty()) {
            ret = h->async_upload(oauth_url, post_str, this);
        } else {
            if (!post_str.empty()) {
                oauth_url += "?";
                oauth_url += post_str;
            }

            ret = h->async_upload_form_data(oauth_url,
                                            param.get_param_map(),
                                            param.get_pic_param_map(),
                                            this);

        }
    } else {
        log("Method neither GET nor POST!");
        ret = false;
    }

    if (ret) {
        wb_request_t req;
        req.url = url;
        req.param = param;
        req.cb = cb;

        pthread_mutex_lock(&m_mutex);
        m_map[h] = req;
        pthread_mutex_unlock(&m_mutex);
    }

    return ret;
}

void request::on_http_complete(http* h, HTTP_CODE ret_code, string& data)
{
    if (h == NULL) {
        assert(h);
        return;
    }

    pthread_mutex_lock(&m_mutex);

    if (m_map.size() == 0) {
        pthread_mutex_unlock(&m_mutex);
        return;
    }

    http_n_request_map::iterator it = m_map.find(h);

    if (it == m_map.end()) {
        pthread_mutex_unlock(&m_mutex);
        delete h;
        h = NULL;
        return;
    }

    pthread_mutex_unlock(&m_mutex);

    wb_request_t& request = it->second;

    if (request.cb == NULL) {
        assert(request.cb);
        pthread_mutex_lock(&m_mutex);
        m_map.erase(h);
        pthread_mutex_unlock(&m_mutex);
        delete h;
        h = NULL;
        return;
    }

    string str_decode = data;
    string format_key = tencent_weibo::param::str_format;
    string format_val = request.param.get_value(format_key);
    string format;
    format.append(format_val);

    if (format ==  tencent_weibo::param::str_json ||
        format ==  tencent_weibo::param::str_xml) {
        string url_decode = util::str::form_url_decode(data);
        str_decode = url_decode;
    }

    request.cb->on_request_complete(ret_code, request.url,
                                    request.param, str_decode);

    pthread_mutex_lock(&m_mutex);
    m_map.erase(h);
    pthread_mutex_unlock(&m_mutex);
    delete h;
    h = NULL;
}