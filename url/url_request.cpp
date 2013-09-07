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

#include "url_request.h"
#include "url/Oauth/oauth.h"
#include "url/Oauth/oauth.c"
#include "url/Oauth/hash.c"
#include "url/Oauth/xmalloc.c"

url_request::url_request(void)
{
}

url_request::~url_request(void)
{
}

url_request* url_request::get_instance()
{
    static url_request instance;
    return &instance;
}

string url_request::get_url(const string& url,
                            HTTP_METHOD method,
                            weibo_param& param,
                            string& post_str)
{
    string custom_key;
    string custom_secrect;
    string token_key;
    string token_secrect;

    param.get_and_remove_key(custom_key,
        custom_secrect,
        token_key,
        token_secrect);

    string base_url = get_base_url(url, param);

    string normalized_url =  normalrize_url(base_url, method,
        custom_key, custom_secrect, token_key, token_secrect,
        post_str);

    return normalized_url;
}

string url_request::get_post_string(weibo_param param)
{
    return param.get_url_param_string();
}

string url_request::get_base_url(const string& url, weibo_param param)
{
    string base_url = url;
    string url_param = param.get_url_param_string();

    base_url += "?";
    base_url += url_param;

    return base_url;
}

string url_request::normalrize_url(const string& url,
                                   HTTP_METHOD method,
                                   const string& custom_key,
                                   const string& custom_secrect,
                                   const string& token_key,
                                   const string& token_secrect,
                                   string& post_str)
{
    const string STR_HTTP_GET  = "GET";
    const string STR_HTTP_POST = "POST";

    char* buff = NULL;
    char* c_custom_key = NULL;
    char* c_custom_secrect = NULL;
    char* c_token_key = NULL;
    char* c_token_secrect = NULL;

    if (!custom_key.empty()) {
        c_custom_key = (char*)custom_key.c_str();
    }

    if (!custom_secrect.empty()) {
        c_custom_secrect = (char*)custom_secrect.c_str();
    }

    if (!token_key.empty()) {
        c_token_key = (char*)token_key.c_str();
    }

    if (!token_secrect.empty()) {
        c_token_secrect = (char*)token_secrect.c_str();
    }

    string method_str;

    if (method == HTTP_METHOD_GET) {
        method_str = STR_HTTP_GET;
    } else {
        method_str = STR_HTTP_POST;
    }

    char* normal_url =
        oauth_sign_url2(url.c_str(), (char**)&buff, OA_HMAC, method_str.c_str(),
                        c_custom_key, c_custom_secrect,
                        c_token_key, c_token_secrect);
    string normarized_url = normal_url;
    free(normal_url);

    post_str = buff;
    free(buff);

    return normarized_url;
}
