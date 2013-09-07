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

#ifndef WEIBO_PARAM_H_
#define WEIBO_PARAM_H_

#include <map>
#include <string>
#include "weibo_api.h"
#include "util_string.h"
using std::string;
using std::map;

class weibo_param
{
public:
     weibo_param(void);
     weibo_param(const weibo_param& other);
     weibo_param& operator=(const weibo_param& other);
     ~weibo_param(void);

     void add_param(const string& key, const string& val);
     string get_value(const string& key);

     void add_pic_name_param(const string& key, const string& val);
     string get_pic_name_value(const string& key);
     string get_url_param_string();
     void clear();

public:
    void get_and_remove_key(string& custom_key,
                            string& custom_secrect,
                            string& token_key,
                            string& token_secrect);

    typedef map<string, string>  string_map;


    string_map& get_param_map();
    string_map& get_pic_param_map();

protected:
    string_map* m_param_map_ptr;
    string_map* m_pic_param_map_ptr;
};

#endif /* WEIBO_PARAM_H_ */