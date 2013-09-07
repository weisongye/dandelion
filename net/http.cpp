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

#include <cstring>
#include <pthread.h>
#include <assert.h>

#include <log.h>
#include "http.h"
#include "message.h"
#include "http_proxy.h"
#include "util_string.h"

using std::map;

HTTP_CODE get_error_code(CURLcode curl_code)
{
    HTTP_CODE err_code;

    switch (curl_code) {
    case CURLE_OK:
        err_code = HTTP_OK;
        break;

    case CURLE_COULDNT_CONNECT:
        err_code = HTTP_COULDNT_CONNECT;
        break;

    case CURLE_OPERATION_TIMEDOUT:
        err_code = HTTP_OPERATION_TIMEDOUT;
        break;

    case CURLE_HTTP_RETURNED_ERROR:
        err_code = HTTP_HTTP_RETURNED_ERROR;
        break;

    case CURLE_COULDNT_RESOLVE_HOST:
        err_code = HTTP_COULDNT_RESOLVE_HOST;
        break;

    default :
        err_code = HTTP_FAILED;
        break;
    }

    return err_code;
}

http::http()
{
    m_ret_data = NULL;
    m_len = 0;
    m_write_pos = 0;
    m_is_running = false;

    m_url.reserve(512);
    m_data.reserve(1024);
}

http::~http()
{
    http_proxy::get_instance()->remove_callback((unsigned long)this);
    reset();
}

void http::reset()
{
    m_url.clear();
    m_data.clear();

    if (m_ret_data) {
        delete [] m_ret_data;
        m_ret_data = NULL;
    }

    m_len = 0;
    m_write_pos = 0;
    m_post_data_map.clear();
    m_post_file_map.clear();
}

HTTP_CODE http::sync_download(const string& url, string& ret_str)
{
    if (url.empty()) {
        return HTTP_FAILED;
    }

    if (m_is_running) {
        return HTTP_FAILED;
    }

    m_is_running = true;
    reset();
    m_url = url;

    CURLcode curl_code = perform(HTTP_METHOD_GET);
    bool b = get_result_data(ret_str);
    m_is_running  = false;

    if (b == true && curl_code == CURLE_OK) {
        return HTTP_OK;
    }

    return get_error_code(curl_code);
}

HTTP_CODE http::sync_upload(const string& url,
                            const string& data,
                            string& ret_str)
{
    if (url.empty()) {
        return HTTP_FAILED;
    }

    if (m_is_running) {
        return HTTP_FAILED;
    }

    m_is_running  = true;
    reset();
    m_url = url;
    m_data = data;

    // Content-type: application/x-www-form-urlencoded
    CURLcode curl_code = perform(HTTP_METHOD_POST);

    bool b = get_result_data(ret_str);

    m_is_running  = false;

    if (b == true && curl_code == CURLE_OK) {
        return HTTP_OK;
    }

    return get_error_code(curl_code);
}

HTTP_CODE http::sync_upload_form_data(const string& url,
                                      const map<string,
                                      string>& data_map,
                                      const map<string, string>& file_map,
                                      string& ret_str)
{
    if (url.empty()) {
        return HTTP_FAILED;
    }

    if (m_is_running) {
        return HTTP_FAILED;
    }

    m_is_running  = true;
    reset();

    m_url = url;
    m_post_data_map = data_map;
    m_post_file_map = file_map;

    // Content-type: multipart/form-data
    CURLcode curl_code = perform(HTTP_METHOD_POST_FROMDATA);
    bool b = get_result_data(ret_str);

    m_is_running  = false;

    if (b == true && curl_code == CURLE_OK) {
        return HTTP_OK;
    }

    return get_error_code(curl_code);
}

void* thread_run_download(void* arg)
{
    http* h = (http*)arg;
    CURLcode curl_code = h->perform(HTTP_METHOD_GET);

    HTTP_CODE ret_code = get_error_code(curl_code);

    message::send_messsage(http_proxy::get_instance()->get_handle(),
        MSG_HTTP_COMPLETE,
        (WPARAM)h,
        (LPARAM)ret_code);

    pthread_detach(pthread_self());
    return NULL;
}

void* thread_run_upload(void* arg)
{
    http* h = (http*)arg;
    CURLcode curl_code = h->perform(HTTP_METHOD_POST);

    HTTP_CODE ret_code = get_error_code(curl_code);

    message::send_messsage(http_proxy::get_instance()->get_handle(),
        MSG_HTTP_COMPLETE,
        (WPARAM)h,
        (LPARAM)ret_code);

    pthread_detach(pthread_self());
    return NULL;
}

void* thread_run_upload_form_data(void* arg)
{
    http* http_ptr = (http*)arg;
    CURLcode curl_code = http_ptr->perform(HTTP_METHOD_POST_FROMDATA);

    HTTP_CODE ret_code = get_error_code(curl_code);

    message::send_messsage(http_proxy::get_instance()->get_handle(),
        MSG_HTTP_COMPLETE,
        (WPARAM)http_ptr,
        (LPARAM)ret_code);

    pthread_detach(pthread_self());
    return NULL;
}

bool http::async_download(const string& url, http_callback* cb)
{
    if (cb == NULL || url.empty()) {
        assert(0);
        return false;
    }

    if ( m_is_running ) {
        log("async_download: m_is_running");
        return false;
    }

    m_is_running = true;
    reset();
    m_url = url;

    http_proxy* http_proxy_ptr = http_proxy::get_instance();

    if (http_proxy_ptr) {
        http_proxy_ptr->add_callback((unsigned long)this, cb);
    }

    pthread_t tid;

    if (pthread_create(&tid, NULL, thread_run_download, (void*)this) != 0)
    {
        log("async_download: pthread_create() failed.");
        return false;
    }

    return true;
}

bool http::async_upload(const string& url,
                        const string& data,
                        http_callback* cb)
{
    // Content-type: application/x-www-form-urlencoded

    if (cb == NULL || url.empty()) {
        assert(0);
        return false;
    }

    if (m_is_running) {
        log("async_upload: m_is_running");
        return false;
    }

    m_is_running = true;
    reset();
    m_url = url;
    m_data = data;
    http_proxy* http_proxy_ptr = http_proxy::get_instance();

    if (http_proxy_ptr) {
        http_proxy_ptr->add_callback((unsigned long)this, cb);
    }

    pthread_t tid;

    if (pthread_create(&tid, NULL, thread_run_upload, (void*)this) != 0)
    {
        log("async_upload: pthread_create() failed.");
        return false;
    }

    return true;
}

bool http::async_upload_form_data(const string& url,
                                  const std::map<string, string>& data_map,
                                  const std::map<string, string>& file_map,
                                  http_callback* cb)
{
    // Content-type: multipart/form-data

    if (cb == NULL || url.empty()) {
        assert(0);
        return false;
    }

    if (m_is_running) {
        log("m_is_running");
        return false;
    }

    m_is_running = true;
    reset();

    m_url = url;
    m_post_data_map = data_map;
    m_post_file_map = file_map;
    http_proxy* pHttproxy = http_proxy::get_instance();

    if (pHttproxy) {
        pHttproxy->add_callback((unsigned long)this, cb);
    }

    pthread_t tid;

    if (pthread_create(&tid, NULL, thread_run_upload_form_data,
                       (void*)this) != 0)
    {
        log("pthread_create() failed in async_upload_form_data()!");
        return false;
    }

    return true;
}

CURLcode http::perform(HTTP_METHOD method)
{
    CURL* curl = curl_easy_init();

    // multi-parts form post
    curl_httppost* post = NULL;
    curl_httppost* last = NULL;

    if (method == HTTP_METHOD_POST_FROMDATA) {
        STR_MAP::iterator data_it = m_post_data_map.begin();

        while (data_it != m_post_data_map.end()) {
            curl_formadd(&post, &last, CURLFORM_COPYNAME,
                (data_it->first).c_str(), CURLFORM_COPYCONTENTS,
                (data_it->second).c_str(), CURLFORM_END);
            data_it++;
        }

        STR_MAP::iterator file_it = m_post_file_map.begin();

        while (file_it != m_post_file_map.end()) {
            curl_formadd(&post, &last, CURLFORM_COPYNAME,
                (file_it->first).c_str(), CURLFORM_FILE,
                (file_it->second).c_str(), CURLFORM_END);
            file_it++;
        }
    }

    if (curl) {
        if (method == HTTP_METHOD_GET) {
            curl_easy_setopt(curl, CURLOPT_HTTPGET, 1 );
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, TRUE);
        } else if (method == HTTP_METHOD_POST) {
            curl_easy_setopt(curl, CURLOPT_POST, 1);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, m_data.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, m_data.length());
        } else if (method == HTTP_METHOD_POST_FROMDATA) {
            curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
        }

        curl_easy_setopt(curl, CURLOPT_URL, m_url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http::handle_result);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

    }

    CURLcode curl_code = curl_easy_perform(curl);

    if (method == HTTP_METHOD_POST_FROMDATA) {
        curl_formfree(post);
    }

    curl_easy_cleanup(curl);

    return curl_code;
}

size_t http::handle_result(void* data, size_t size, size_t nmemb,
                           void* http_ptr)
{
    if (http_ptr == NULL) {
        return 0;
    }

    http* h = (http*)http_ptr;
    h->write_result_data((char*)data, size * nmemb);

    return  size * nmemb;
}

bool http::write_result_data(char* data, unsigned long len)
{
    const int RESULTDATA_INCREASE_LEN  = (10 * 1024);
    const int MAX_RECVDATA_LEN = 102400000;

    if (data == NULL || len== 0) {
        return false;
    }

    if (m_ret_data == NULL) {
        m_len = RESULTDATA_INCREASE_LEN;
        m_write_pos = 0;
        m_ret_data = new char[RESULTDATA_INCREASE_LEN];
    }

    if (m_write_pos + len > m_len) {
        if (m_len + RESULTDATA_INCREASE_LEN > MAX_RECVDATA_LEN) {
            return false;
        }

        char* temp_data = new char[m_len + RESULTDATA_INCREASE_LEN];

        if (temp_data == NULL) {
            return false;
        }

        memcpy(temp_data, m_ret_data, m_len);
        delete [] m_ret_data;
        m_ret_data = temp_data;
        m_len = m_len + RESULTDATA_INCREASE_LEN;
    }

    if (m_write_pos + len > m_len) {
        return false;
    }

    memcpy(m_ret_data + m_write_pos, data, len);
    m_write_pos += len;

    return true;
}

bool http::get_result_data(string& data)
{
    if (m_ret_data && m_write_pos) {
        data.clear();
        data.append(m_ret_data, m_write_pos);

        return true;
    }

    return false;
}
