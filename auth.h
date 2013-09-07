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

#ifndef AUTH_H_
#define AUTH_H_

#include <string>

using std::string;

#define AUTH_FILE   "config/auth.conf"

class auth
{
public:
    enum LOGIN_CODE {
        ERR_NO_ERROR = 0,
        ERR_APP_KEY_EMPTY,
        ERR_APP_SECRET_EMPTY,
        ERR_GET_TOKEN,
        ERR_OAUTH_VERIFY_EMPTY,
        ERR_GET_ACCESS_KEY,
        ERR_NO_AUTH_FILE,
    };

    auth();

    bool apply_token();
    bool apply_access_key();
    bool load_auth_config();
    bool save_auth_config();

    string get_app_key();
    string get_app_secret();
    string get_access_key();
    string get_access_secret();

    int login();

private:
    string m_app_key;
    string m_app_secret;
    string m_access_key;
    string m_access_secret;

    string m_oauth_verify;
    string m_token_key;
    string m_token_secrect;
};

#endif /* AUTH_H_ */