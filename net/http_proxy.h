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

#ifndef HTTP_PROXY_H_
#define HTTP_PROXY_H_

#include <map>

#include <defs.h>
#include "message.h"

class http_callback;

class  http_proxy
{
public:
    static http_proxy* get_instance();

    int get_handle();

    void add_callback(unsigned long key, http_callback* cb);
    http_callback* get_callback(unsigned long key);
    void remove_callback(unsigned long key);
    void remove_callback(http_callback* cb);

    void on_http_complete(WPARAM w_param, LPARAM l_param);

private:
    http_proxy();
    virtual ~http_proxy();
    bool init();

    int m_handle;

    typedef std::map<unsigned long, http_callback*> CALLBACK_MAP;
    CALLBACK_MAP m_cb_map;
    pthread_mutex_t m_cb_map_mutex;
};

#endif /* HTTP_PROXY_H_ */