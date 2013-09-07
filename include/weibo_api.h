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

#ifndef WEIBO_API_H_
#define WEIBO_API_H_

#include <string>
#include <defs.h>
using std::string;

enum HTTP_METHOD {
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_POST_FROMDATA,
};

enum HTTP_CODE {
    HTTP_OK = 0,
    HTTP_OPERATION_TIMEDOUT,
    HTTP_COULDNT_RESOLVE_HOST,
    HTTP_COULDNT_CONNECT,
    HTTP_HTTP_RETURNED_ERROR,
    HTTP_FAILED,
    HTTP_TOOLAGRE,
    HTTP_OPTION_ERR
};

namespace  tencent_weibo
{
namespace param
{
const string str_custom_key         = "str_custom_key";
const string str_custom_secrect     = "str_custom_secrect";
const string str_token_key          = "str_token_key";
const string str_token_secrect      = "str_token_secrect";
const string strVerifyCode          = "oauth_verifier";
const string str_callback_url       = "oauth_callback";

const string str_format     = "format";
const string str_json       = "json";
const string str_xml        = "xml";
}
}


// For more information, see http://wiki.open.t.qq.com/index.php
enum WEIBO_API_TYPE {
    TXWB_BASE = 0,

    // TXWB_STATUSES_
    TXWB_STATUSES_HOME_TIMELINE,
    TXWB_STATUSES_PUBLIC_TIMELINE,
    TXWB_STATUSES_USER_TIMELINE,
    TXWB_STATUSES_MENTIONS_TIMELINE,
    TXWB_STATUSES_HT_TIMELINE,
    TXWB_STATUSES_BROADCASET_TIMELINE,
    TXWB_STATUSES_SPECIAL_TIMELINE,
    TXWB_STATUSES_AREA_TIMELINE,
    TXWB_STATUSES_HOME_TIMELINE_IDS,
    TXWB_STATUSES_USER_TIMELINE_IDS,
    TXWB_STATUSES_BROADCASET_TIMELINE_IDS,
    TXWB_STATUSES_MENTIONS_TIMELINE_IDS,
    TXWB_STATUSES_USERS_TIMELINE,
    TXWB_STATUSES_USERS_TIMELINE_IDS,

    // TXWB_T_
    TXWB_T_SHOW,
    TXWB_T_ADD,
    TXWB_T_DEL,
    TXWB_T_RE_ADD,
    TXWB_T_REPLY,
    TXWB_T_ADD_PIC,
    TXWB_T_RE_COUNT,
    TXWB_T_RE_LIST,
    TXWB_T_COMMENT,
    TXWB_T_ADD_MUSIC,
    TXWB_T_ADD_VIDEO,
    TXWB_T_GETVIDEOINFO,
    TXWB_T_LIST,
    TXWB_T_ADD_VIDEO_PREV,
    TXWB_T_ADD_PIC_URL,

    // TXWB_USER_
    TXWB_USER_INFO,
    TXWB_USER__UPDATE,
    TXWB_USER_UPDATE_HEAD,
    TXWB_USER_UPDATE_EDU,
    TXWB_USER_OTHER_INFO,
    TXWB_USER_INFOS,

    // TXWB_FRIENDS_
    TXWB_FRIENDS_FANSLIST,
    TXWB_FRIENDS_IDOLLIST,
    TXWB_FRIENDS_BLACKLIST,
    TXWB_FRIENDS_SPECIALLIST,
    TXWB_FRIENDS_ADD,
    TXWB_FRIENDS_DEL,
    TXWB_FRIENDS_ADDSPECIAL,
    TXWB_FRIENDS_DELSPECIAL,
    TXWB_FRIENDS_ADD_BLACKLIST,
    TXWB_FRIENDS_DELBLACKLIST,
    TXWB_FRIENDS_CHECK,
    TXWB_FRIENDS_USER_FANSLIST,
    TXWB_FRIENDS_USER_IDOLLIST,
    TXWB_FRIENDS_USER_SPECIALLIST,
    TXWB_FRIENDS_USER_FANSLIST_S,
    TXWB_FRIENDS_USER_IDOLIST_S,

    // TXWB_PRIVATE_
    TXWB_PRIVATE_ADD,
    TXWB_PRIVATE_DEL,
    TXWB_PRIVATE_RECV,
    TXWB_PRIVATE_SEND,

    // TXWB_SEARCH_
    TXWB_SEARCH_USER,
    TXWB_SEARCH_T,
    TXWB_SEARCH_USERBYTAG,

    // TXWB_TRENDS_
    TXWB_TRENDS_HT,
    TXWB_TRENDS_T,

    // TXWB_INFO_
    TXWB_INFO_UPDATE,

    // TXWB_FAV_
    TXWB_FAV_ADDT,
    TXWB_FAV_DELT,
    TXWB_FAV_LIST_T,
    TXWB_FAV_ADDHT,
    TXWB_FAV_DELHT,
    TXWB_FAV_LIST_HT,

    // TXWB_HT_
    TXWB_HT_IDS,
    TXWB_HT_INFO,

    // TXWB_TAG_
    TXWB_TAG_ADD,
    TXWB_TAG_DEL,

    // TXWB_OTHER_
    TXWB_OTHER_KNOWNPERSON,
    TXWB_OTHER_SHORTURL,
    TXWB_OTHER_VIDEOKEY,

    TXWB_MAX
};

class weibo_param;

class weibo_callback
{
public:
    virtual void on_request_complete(HTTP_CODE ret,
                                     const string& url,
                                     weibo_param param,
                                     const string& data) = 0;
};

HTTP_CODE sync_request(const string& url, HTTP_METHOD method,
                       weibo_param param, string& data);

bool async_request(const string& url, HTTP_METHOD method,
                   weibo_param param, weibo_callback* cb);

HTTP_CODE sync_request_by_type(WEIBO_API_TYPE type,
                               weibo_param param, string& data , int& len);

bool async_request_by_type(WEIBO_API_TYPE type,
                           weibo_param param,
                           weibo_callback* cb);

string weibo_get_url(WEIBO_API_TYPE type);

#endif /* WEIBO_API_H_ */