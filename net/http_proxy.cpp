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

#include <ctime>
#include <cstdlib>
#include <pthread.h>

#include "curl/curl.h"
#include "http.h"
#include "util_string.h"
#include "http_proxy.h"

http_proxy* g_http_proxy_ptr = NULL;

http_proxy* http_proxy::get_instance()
{
    if (g_http_proxy_ptr == NULL) {
        g_http_proxy_ptr = new http_proxy;

        if (!g_http_proxy_ptr->init()) {
            delete g_http_proxy_ptr;
            g_http_proxy_ptr = NULL;
        }
    }

    return g_http_proxy_ptr;
}

http_proxy::http_proxy()
{
    pthread_mutex_init(&m_cb_map_mutex, NULL);
}

http_proxy::~http_proxy()
{
    if (g_http_proxy_ptr != NULL) {
        delete g_http_proxy_ptr;
    }

    pthread_mutex_destroy(&m_cb_map_mutex);
}

bool http_proxy::init()
{
    srand((unsigned)time(NULL));
    m_handle = rand();
    return true;
}

int http_proxy::get_handle()
{
    return m_handle;
}

void http_proxy::on_http_complete(WPARAM w_param, LPARAM l_param)
{
    http_callback* cb = NULL;
    cb = get_callback((unsigned int)w_param);

    if (cb) {
        HTTP_CODE ret_code = (HTTP_CODE)l_param;
        http* h = (http*)w_param;

        string data;
        data.reserve(600);

        if (!h->get_result_data(data)) {
            ret_code = HTTP_TOOLAGRE;
        }

        if (NULL != h) {
            h-> m_is_running = false;
        }

        cb->on_http_complete(h, ret_code, data);
    }
}

void http_proxy::add_callback(unsigned long key, http_callback* cb)
{
    if (key == 0 || cb == NULL) {
        return;
    }

    pthread_mutex_lock(&m_cb_map_mutex);
    m_cb_map[key] = cb;
    pthread_mutex_unlock(&m_cb_map_mutex);
}

void http_proxy::remove_callback(unsigned long key)
{
    pthread_mutex_lock(&m_cb_map_mutex);
    m_cb_map.erase(key);
    pthread_mutex_unlock(&m_cb_map_mutex);
}

void http_proxy::remove_callback(http_callback* cb)
{
    pthread_mutex_lock(&m_cb_map_mutex);

#ifdef _WIN32
    CALLBACK_MAP::iterator it = m_cb_map.begin();

    while (it != m_cb_map.end() ) {
        if (it->second == cb) {
            it = m_cb_map.erase(it); // TODO
        } else {
            it++;
        }
    }
#else
start:
    CALLBACK_MAP::iterator it = m_cb_map.begin();

    while ( it != m_cb_map.end() ) {
        if (it->second == cb) {
            m_cb_map.erase(it);
            goto start;
        } else {
            it++;
        }
    }
#endif /* _WIN32 */

    pthread_mutex_unlock(&m_cb_map_mutex);
}

http_callback* http_proxy::get_callback(unsigned long key)
{
    http_callback* cb = NULL;

    pthread_mutex_lock(&m_cb_map_mutex);

    CALLBACK_MAP::const_iterator iter = m_cb_map.find(key);

    if (iter != m_cb_map.end()) {
        cb = iter->second;
    }

    pthread_mutex_unlock(&m_cb_map_mutex);

    return cb;
}
