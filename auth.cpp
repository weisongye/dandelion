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
#include <fstream>
#include <cstring>
#include <vector>

#include <weibo_param.h>
#include <log.h>
#include "auth.h"

using namespace std;

auth::auth()
{
    m_app_key.reserve(9);
    m_app_secret.reserve(32);
    m_access_key.reserve(32);
    m_access_secret.reserve(32);

    if (load_auth_config() == false) {

    }
}

string auth::get_app_key()
{
    return m_app_key;
}

string auth::get_app_secret()
{
    return m_app_secret;
}

string auth::get_access_key()
{
    return m_access_key;
}

string auth::get_access_secret()
{
    return m_access_secret;
}

int auth::login()
{
    if (get_app_key().size() > 0 &&
        get_app_secret().size() > 0 &&
        get_access_key().size() > 0 &&
        get_access_secret().size() > 0) {
        return ERR_NO_ERROR;
    }

#ifndef WIN32
    log("Need to create auth config under Microsoft(R) Windows.");
    return ERR_NO_AUTH_FILE;
#endif /* WIN32 */

    cout << "app_key = ";
    // cin >> m_app_key;
    m_app_key = "801058005";

    if (m_app_key.size() == 0) {
        return ERR_APP_KEY_EMPTY;
    }

    cout << "app_secret = ";
    // cin >> m_app_secret;
    m_app_secret = "31cc09205420a004f3575467387145a7";

    if (m_app_secret.size() == 0) {
        return ERR_APP_SECRET_EMPTY;
    }

    if (apply_token() == false) {
        return ERR_GET_TOKEN;
    }

    cout << "oauth_verify = ";
    cin >> m_oauth_verify;

    if (m_oauth_verify.size() == 0) {
        return ERR_OAUTH_VERIFY_EMPTY;
    }

    if (apply_access_key() == false) {
        return ERR_GET_ACCESS_KEY;
    }

    save_auth_config();
    return ERR_NO_ERROR;
}

bool auth::apply_token()
{
    static const string callback_url = "http://www.qq.com";

    weibo_param param;

    string str_custom_key = tencent_weibo::param::str_custom_key;
    string str_custom_secrect = tencent_weibo::param::str_custom_secrect;
    string str_oauth_callback = tencent_weibo::param::str_callback_url;

    param.add_param(str_custom_key, m_app_key);
    param.add_param(str_custom_secrect, m_app_secret);
    param.add_param(str_oauth_callback, callback_url);

    static const string url = "https://open.t.qq.com/cgi-bin/request_token";

    string ret_data;
    HTTP_CODE  ret = sync_request(url, HTTP_METHOD_GET, param, ret_data);

    if (ret != HTTP_OK || ret_data == "") {
        return false;
    }

    string str_ret = ret_data;

    string ot = "oauth_token=";
    string::size_type token_pos = str_ret.find(ot);

    if (token_pos == 0) {
        string ots = "&oauth_token_secret=";
        string::size_type secrect_pos = str_ret.find(ots);

        if ((secrect_pos != string::npos) && (secrect_pos > token_pos)) {
            m_token_key = str_ret.substr(ot.length(), secrect_pos - ot.length());

            string occ = "&oauth_callback_confirmed=";
            string::size_type cb_pos = str_ret.find(occ);

            if (token_pos != string::npos) {
                m_token_secrect = str_ret.substr(secrect_pos + ots.length(),
                    cb_pos - (secrect_pos + ots.length()));
            }
        }
    }

    if (!m_token_key.empty()) {
        string verify = "http://open.t.qq.com/cgi-bin/authorize";
        verify += "?oauth_token=";
        verify += m_token_key;
#ifdef WIN32
        ShellExecuteA(NULL, "open", verify.c_str(), NULL, NULL, SW_SHOWNORMAL);
#else
        // TODO
#endif /* WIN32 */
        return true;
    }

    return false;
}

bool auth::apply_access_key()
{
    weibo_param param;

    string str_custom_key = tencent_weibo::param::str_custom_key;
    string str_custom_secrect = tencent_weibo::param::str_custom_secrect;
    string str_token_key = tencent_weibo::param::str_token_key;
    string str_token_secrect = tencent_weibo::param::str_token_secrect;

    param.add_param(str_custom_key, m_app_key);
    param.add_param(str_custom_secrect, m_app_secret);

    param.add_param(str_token_key, m_token_key);
    param.add_param(str_token_secrect, m_token_secrect);

    param.add_param("oauth_verifier", m_oauth_verify);

    static const string url = "https://open.t.qq.com/cgi-bin/access_token";

    string ret_date = "";
    HTTP_CODE ret = sync_request(url, HTTP_METHOD_GET, param, ret_date);

    if (ret != HTTP_OK || ret_date == "") {
        return false;
    }

    string str_ret = ret_date;

    string ot = "oauth_token=";
    string::size_type token_pos = str_ret.find(ot);

    if (token_pos == 0) {
        string ots = "&oauth_token_secret=";
        string::size_type secrect_pos = str_ret.find(ots);

        if ((secrect_pos != string::npos) && (secrect_pos > token_pos)) {
            m_access_key = str_ret.substr(ot.length(),
                                          secrect_pos - ot.length());

            string n = "&name=";
            string::size_type cb_pos = str_ret.find(n);

            if (token_pos != string::npos) {
                m_access_secret = str_ret.substr(secrect_pos + ots.length(),
                    cb_pos - (secrect_pos + ots.length()));
            }
        }

        return true;
    }

    return false;
}

bool auth::load_auth_config()
{
    ifstream is;
    is.open(AUTH_FILE, ios::in);

    string keys[] = {
        "AppKey=",
        "AppSecret=",
        "AccessKey=",
        "AccessSecret=",
    };

    if (!is) {
        log(string(AUTH_FILE) + " not found!");
        return false;
    }

    string line;
    line.reserve(46);

    getline(is, line); // [Config]
    log(line);

    if (line != "[Config]" && line != "[Config]\r") {
        log(string(AUTH_FILE) + " is corrupt!");
        is.close();
        return false;
    }

    vector<string> values;
    values.reserve(8);

    for (size_t i = 0; i < 4; i++) {
        getline(is, line);
        log(line);

        if (line.at(line.size() - 1) == '\r') {
            line = line.substr(0, line.size() - 1);
        }

        values.push_back(line.substr(keys[i].size()));
    }

    size_t i = 0;
    m_app_key = values[i++];
    m_app_secret = values[i++];
    m_access_key = values[i++];
    m_access_secret = values[i++];

    is.close();

    return true;
}

bool auth::save_auth_config()
{
    ofstream os;
    os.open(AUTH_FILE, ios::out);

    os << "[Config]" << endl;
    os << "AppKey=" << m_app_key << endl;
    os << "AppSecret=" << m_app_secret << endl;
    os << "AccessKey=" << m_access_key << endl;
    os << "AccessSecret=" << m_access_secret << endl;

    os.close();

    return true;
}
