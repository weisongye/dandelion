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

#include <log.h>
#include "request.h"
#include "weibo_api.h"

HTTP_CODE sync_request(const string& url,
                              HTTP_METHOD method,
                              weibo_param param,
                              string& data)
{
    string ret_data;
    HTTP_CODE ret =
        request::get_instance()->sync_request(url, method, param, ret_data);

    string temp_data = ret_data;
    data = temp_data;

    return ret;
}

bool async_request(const string& url,
                   HTTP_METHOD mehtod,
                   weibo_param param,
                   weibo_callback* cb)
{
    return request::get_instance()->async_request(url, mehtod, param, cb);
}

static string txwb_get_uri(WEIBO_API_TYPE type)
{

    static const char txwb_uri_t[TXWB_MAX][64] = {
        "base",

        "/api/statuses/home_timeline",
        "/api/statuses/public_timeline",
        "/api/statuses/user_timeline",
        "/api/statuses/mentions_timeline",
        "/api/statuses/ht_timeline",
        "/api/statuses/broadcast_timeline",
        "/api/statuses/special_timeline",
        "/api/statuses/area_timeline",
        "/api/statuses/home_timeline_ids",
        "/api/statuses/user_timeline_ids",
        "/api/statuses/broadcast_timeline_ids",
        "/api/statuses/mentions_timeline_ids",
        "/api/statuses/users_timeline",
        "/api/statuses/users_timeline_ids",

        "/api/t/show",
        "/api/t/add",
        "/api/t/del",
        "/api/t/re_add",
        "/api/t/reply",
        "/api/t/add_pic",
        "/api/t/re_count",
        "/api/t/re_list",
        "/api/t/comment",
        "/api/t/add_music",
        "/api/t/add_video",
        "/api/t/getvideoinfo",
        "/api/t/list",
        "/api/t/add_video_prev",
        "/api/t/add_pic_url",

        "/api/user/info",
        "/api/user/update",
        "/api/user/update_head",
        "/api/user/update_edu",
        "/api/user/other_info",
        "/api/user/infos",

        "/api/friends/fanslist",
        "/api/friends/idollist",
        "/api/friends/blacklist",
        "/api/friends/speciallist",
        "/api/friends/add",
        "/api/friends/del",
        "/api/friends/addspecial",
        "/api/friends/delspecial",
        "/api/friends/addblacklist",
        "/api/friends/delblacklist",
        "/api/friends/check",
        "/api/friends/user_fanslist",
        "/api/friends/user_idollist",
        "/api/friends/user_speciallist",
        "/api/friends/fanslist_s",
        "/api/friends/idollist_s",

        "/api/private/add",
        "/api/private/del",
        "/api/private/recv",
        "/api/private/send",

        "/api/search/user",
        "/api/search/t",
        "/api/search/userbytag",

        "/api/trends/ht",
        "/api/trends/t",

        "/api/info/update",

        "/api/fav/addt",
        "/api/fav/delt",
        "/api/fav/list_t",
        "/api/fav/addht",
        "/api/fav/delht",
        "/api/fav/list_ht",

        "/api/ht/ids",
        "/api/ht/info",

        "/api/tag/add",
        "/api/tag/del",

        "/api/other/kownperson",
        "/api/other/shorturl",
        "/api/other/videokey"
    };

    if (type >= TXWB_MAX || type <= TXWB_BASE) {
        log("type >= TXWB_MAX || type <= TXWB_BASE");
        return "";
    }

    return txwb_uri_t[type];
}

string weibo_get_url(WEIBO_API_TYPE type)
{
    const string TXWB_BASE_URL = "http://open.t.qq.com";
    string ret;
    ret.reserve(38);

    const string uri = txwb_get_uri(type);

    if (uri != "")
    {
        ret = TXWB_BASE_URL;
        ret += uri;
    }

    return ret;
}

HTTP_METHOD weibo_get_http_method(WEIBO_API_TYPE type)
{
    static const HTTP_METHOD txwb_httpmethod_t[TXWB_MAX] = {
        // base
        HTTP_METHOD_GET,

        // statuses
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,

        // t
        HTTP_METHOD_GET,
        HTTP_METHOD_POST,
        HTTP_METHOD_POST,
        HTTP_METHOD_POST,
        HTTP_METHOD_POST,
        HTTP_METHOD_POST,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_POST,
        HTTP_METHOD_POST,
        HTTP_METHOD_POST,
        HTTP_METHOD_POST,
        HTTP_METHOD_GET,
        HTTP_METHOD_POST,
        HTTP_METHOD_POST,

        // user
        HTTP_METHOD_GET,
        HTTP_METHOD_POST,
        HTTP_METHOD_POST,
        HTTP_METHOD_POST,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,

        // friends
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_POST,
        HTTP_METHOD_POST,
        HTTP_METHOD_POST,
        HTTP_METHOD_POST,
        HTTP_METHOD_POST,
        HTTP_METHOD_POST,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,

        // private
        HTTP_METHOD_POST,
        HTTP_METHOD_POST,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,

        // search
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,

        // trends
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,

        // info
        HTTP_METHOD_GET,

        // fav
        HTTP_METHOD_POST,
        HTTP_METHOD_POST,
        HTTP_METHOD_GET,
        HTTP_METHOD_POST,
        HTTP_METHOD_POST,
        HTTP_METHOD_GET,

        // ht
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,

        // tag
        HTTP_METHOD_POST,
        HTTP_METHOD_POST,

        // other
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
        HTTP_METHOD_GET,
    };

    return txwb_httpmethod_t[type];
}

HTTP_CODE sync_request_by_type(WEIBO_API_TYPE type,
                                     weibo_param param,
                                     string& data)
{
    string url;

    url = weibo_get_url(type);

    if (url == "" ) {
        return HTTP_OPTION_ERR;
    }

    return sync_request(url, weibo_get_http_method(type), param, data);
}

bool async_request_by_type(WEIBO_API_TYPE type,
                           weibo_param param,
                           weibo_callback* cb)
{
    string url;

    url = weibo_get_url(type);

    if (url == "") {
        log("Get url return none.");
        return false;
    }

    return async_request(url, weibo_get_http_method(type), param, cb);
}
