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

#include <iostream>

#include <log.h>
#include <assert.h>
#include "weibo_param.h"
#include "weibo_api.h"

weibo_param::weibo_param(void)
{
    m_param_map_ptr = new string_map;
    m_param_map_ptr->clear();
    m_pic_param_map_ptr = new string_map;
    m_pic_param_map_ptr->clear();
}

weibo_param::weibo_param(const weibo_param& other)
{
    m_param_map_ptr = new string_map;
    m_pic_param_map_ptr = new string_map;
    string_map::iterator it = other.m_param_map_ptr->begin();

    for (; it != other.m_param_map_ptr->end(); it++) {
        (*m_param_map_ptr)[it->first] = it->second;
    }

    it = other.m_pic_param_map_ptr->begin();

    for (; it != other.m_pic_param_map_ptr->end(); it++) {
        (*m_pic_param_map_ptr)[it->first] = it->second;
    }

}
weibo_param::~weibo_param(void)
{
    clear();

    if (m_param_map_ptr) {
        delete m_param_map_ptr;
        m_param_map_ptr = NULL;
    }

    if (m_pic_param_map_ptr) {
        delete m_pic_param_map_ptr;
        m_pic_param_map_ptr = NULL;
    }
}

void weibo_param::add_param(const string& key, const string& val)
{
    if (key == "" || val == "" || m_param_map_ptr == NULL) {
        assert(0);
        return ;
    }

    string url_encoded_val = util::str::form_url_encode(val);
    m_param_map_ptr->insert(make_pair(key, url_encoded_val));
}

string weibo_param::get_value(const string& key)
{
    if ( !m_param_map_ptr || key == "") {
        assert(0);
        return "";
    }

    string_map::iterator it = m_param_map_ptr->find(key);

    if (it == (*m_param_map_ptr).end()) {
        return "";
    }

    return it->second;
}

void weibo_param::add_pic_name_param(const string& key, const string& val)
{
    if (!m_pic_param_map_ptr || key == "") {
        assert(0);
        return;
    }

    (*m_pic_param_map_ptr)[key] = val;
}

string weibo_param::get_pic_name_value(const string& key)
{
    if (!m_pic_param_map_ptr || key == "") {
        assert(0);
        return "";
    }

    string_map::iterator it = m_pic_param_map_ptr->find(key);

    if (it == m_pic_param_map_ptr->end()) {
        return "";
    }

    return it->second;
}

void weibo_param::get_and_remove_key(string& custom_key,
                                     string& custom_secrect,
                                     string& token_key,
                                     string& token_secrect)
{
    if (!m_param_map_ptr) {
        assert(0);
        return;
    }

    custom_key = get_value(tencent_weibo::param::str_custom_key);
    custom_secrect = get_value(tencent_weibo::param::str_custom_secrect);
    token_key = get_value(tencent_weibo::param::str_token_key);
    token_secrect = get_value(tencent_weibo::param::str_token_secrect);

    m_param_map_ptr->erase(tencent_weibo::param::str_custom_key);
    m_param_map_ptr->erase(tencent_weibo::param::str_custom_secrect);
    m_param_map_ptr->erase(tencent_weibo::param::str_token_key);
    m_param_map_ptr->erase(tencent_weibo::param::str_token_secrect);
}

string weibo_param::get_url_param_string()
{
    if (!m_param_map_ptr) {
        assert(0);
        return NULL;
    }

    string ret;

    for (string_map::iterator it = m_param_map_ptr->begin();
         it != m_param_map_ptr->end(); ++it) {
        if (it != m_param_map_ptr->begin()) {
            ret += "&";
        }

        ret += it->first;
        ret += "=";
        ret += it->second;
    }

    return ret;
}

void weibo_param::clear()
{
    if (!m_param_map_ptr || !m_pic_param_map_ptr) {
        assert(0);
        return;
    }

    m_param_map_ptr->clear();
    m_pic_param_map_ptr->clear();
}

weibo_param::string_map& weibo_param::get_param_map()
{
    if (!m_param_map_ptr) {
        assert(0);
    }

    return *m_param_map_ptr;
}

weibo_param::string_map& weibo_param::get_pic_param_map()
{
    if (!m_pic_param_map_ptr) {
        assert(0);
    }

    return *m_pic_param_map_ptr;
}

weibo_param& weibo_param::operator=(const weibo_param& other)
{
    string_map::const_iterator it = other.m_param_map_ptr->begin();

    for (; it != other.m_param_map_ptr->end(); it++) {
        (*m_param_map_ptr)[it->first] = it->second;
    }

    it = other.m_pic_param_map_ptr->begin();

    for (; it != other.m_pic_param_map_ptr->end(); it++) {
        (*m_pic_param_map_ptr)[it->first] = it->second;
    }

    return *this;
}