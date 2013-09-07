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

#include <cstdlib>
#include <string>
#include <iostream>
#include <assert.h>

#include <defs.h>
#include <log.h>
#include <iconv.h>
#include "weibo_api.h"
#include "util_string.h"

using std::string;

string util::str::convert_code(const char* tocode,
                                  const char* fromcode,
                                  string str)
{
    iconv_t cd  = iconv_open(tocode, fromcode);

    if (cd == iconv_t(-1)) {
        log("encode convert not supported!");

        if (errno == EINVAL) {
            log("einval");
        }

        return "";
    }

    size_t insize = str.size();
    char* input = new char[insize + 1];
    char* old_input = input;
    memset(input, 0, insize + 1);
    memcpy(input, str.c_str(), insize);

    size_t outsize = insize * 3;
    char* output = new char[outsize];
    memset(output, 0, outsize);
    char* old_output = output;

    iconv(cd, (const char**)&input, &insize, (char**)&output, &outsize);
    string ret = old_output;
    delete[] old_output;
    delete[] old_input;

    iconv_close(cd);
    return ret;
}

string util::str::mbcs_to_utf8(const string& mbcs)
{
    return convert_code("UTF-8", "GB2312", mbcs);
}

string util::str::utf8_to_mbcs(const string& utf8)
{
    return convert_code("GB2312", "UTF-8", utf8);
}

string util::str::url_encode(const string& source)
{
    string dest;
    dest.reserve(source.length());
    const string digits = "0123456789ABCDEF";
    unsigned char ch;

    int i = 0;

    while (source[i]) {
        ch = source[i];

        if (ch == '&') {
            dest.push_back('%');
            dest.push_back(digits[(ch >> 4) & 0x0F]);
            dest.push_back(digits[ch & 0x0F]);
        } else {
            dest.push_back(ch);
        }

        i++;
    }

    return dest;
}

string util::str::form_url_encode(const string& pEncode)
{
    return url_encode(pEncode);
}

string util::str::form_url_decode(const string& decode)
{
    string buffer = "";
    buffer.reserve(decode.length());

    for (int i = 0; i < decode.length(); i++) {
        int j = i ;
        char ch = decode[j];

        if (ch == '+') {
            buffer += ' ';
        } else if (ch == '%') {
            char tmpstr[] = "0x0__";
            int count;
            tmpstr[3] = decode[j + 1];
            tmpstr[4] = decode[j + 2];
            count = strtol(tmpstr, NULL, 16);
            buffer += count;
            i += 2;
        } else {
            buffer += ch;
        }
    }

    return buffer;
}