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
#include <string>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <deque>

#include <util_string.h>
#include <log.h>
#include "defs.h"
#include "net/net.h"
#include "parser.h"

using namespace std;

bool parser::need_to_block(const string& str, const vector<string>& block_list)
{
    for (size_t i = 0; i < block_list.size(); i++) {
        if (str.find(block_list[i]) != string::npos) {
            log("Block [" + block_list[i] + "] in:" + str);
            return true;
        }
    }

    return false;
}

int parser::parse_xml_group(const vector<string>& urls,
                            vector<weibo>& vec,
                            const vector<string>& block_list,
                            const vector<string>& erase_list)
{
#ifdef WIN32
    const char* local_filename = "rss.xml";
#else
    const char* local_filename = "/tmp/rss.xml";
#endif /* WIN32 */

    for (size_t i = 0; i < urls.size(); i++) {
        net::download(urls.at(i).c_str(), local_filename);
        parser::get_qq_news(local_filename, vec, block_list, erase_list);
    }

    std::sort(vec.begin(), vec.end());
    return 0;
}


deque<string> published;

string parser::remove_rest_of_last_punctuation(const string& line)
{
    char punctuations[][4] = {
        { -29, -128, -126, 0}, /* . */
        { -17, -68, -97, 0},   /* ? */
        { -17, -68, -127, 0},  /* ! */
        { -17, -68, -116, 0},  /* , */
    };

    string::size_type pos;

    for (size_t i = 0;
         i < sizeof(punctuations) / sizeof(punctuations[0]);
         i++) {
        pos = line.rfind(punctuations[i]);

        if (pos != string::npos) {
            break;
        }
    }

    return (pos == string::npos ? line : line.substr(0, pos + 3));
}

string parser::erase_news_source(const string& line)
{
    const char source[][8] = {
        {-26, -105, -91, -25, -108, -75, 0},    /* ridian */
        {-26, -105, -91, -24, -82, -81, 0},     /* rixun */
    };

    const char square_bracket_l[] = {40, 0};     /* ( */
    const char square_bracket_r[] = {41, 0};     /* ) */

    const char square_bracket_lf[] = { -17, -68, -120, 0};  /* ( */
    const char square_bracket_rf[] = { -17, -68, -119, 0};   /* ) */

    string::size_type pos = 0;
    string ret;

    for (int i = 0; i < sizeof(source) / sizeof(source[0]); i++)
    {
        pos = line.find(source[i]);
        if (pos != string::npos && line[pos - 1] >= '0' && line[pos - 1] <= '9')
        {
            log("Erase: " + line.substr(0, pos + strlen(source[i])));
            ret = line.substr(pos + strlen(source[i]));

            if (ret.find(square_bracket_l) == 0) {
                ret = ret.substr(ret.find(square_bracket_r) +
                                 strlen(square_bracket_r));
            }

            if (ret.find(square_bracket_lf) == 0) {
                ret = ret.substr(ret.find(square_bracket_rf) +
                                 strlen(square_bracket_rf));
            }

            return ret;
        }
    }

    return line;
}

string parser::erase_parentheses(const string& line)
{
    string ret = line;

    const char contains[][8] = {
        { -24, -82, -80, -24, -128, -123, 0},   /* jizhe */
        { -26, -106, -121, 0},  /* wen */
        { -27, -101, -66, 0},   /* tu */
        { -25, -82, -128, -25, -89, -80, 0},    /* jiancheng */
    };

    for (int i = 0; i < sizeof(contains) / sizeof(contains[0]); i++) {
        string contain = contains[i];
        ret = erase_parenthes(ret, contain);
    }

    return ret;
}

string parser::erase_parenthes(const string& line, const string& contain)
{
    string ret;
    ret.reserve(600);

    enum bracket_type {
        NONE,
        SQUARE_BRACKET,
        SQUARE_BRACKET_FULL,
        ANGLE_BRACKET,
    } type;

    const char square_bracket_l[] = {40, 0};     /* ( */
    const char square_bracket_r[] = {41, 0};     /* ) */

    const char square_bracket_lf[] = { -17, -68, -120, 0};  /* ( */
    const char square_bracket_rf[] = { -17, -68, -119, 0};   /* ( */

    const char angle_bracket_l[] = { -29, -128, -112, 0};  /* [ */
    const char angle_bracket_r[] = { -29, -128, -111, 0};  /* ] */

    string::size_type contain_pos = 0;
    contain_pos = line.find(contain);

    if (contain_pos == string::npos) {
        return line;
    }

    string sub = line.substr(contain_pos);

    string::size_type bracket_right_pos = 0;
    string::size_type bracket_left_pos = 0;
    string::size_type temp_pos = 0;

    type = NONE;
    bracket_right_pos = sub.find(square_bracket_r);

    if (bracket_right_pos != string::npos) {
        type = SQUARE_BRACKET;
    }

    temp_pos = sub.find(square_bracket_rf);

    if (temp_pos < bracket_right_pos) {
        bracket_right_pos = temp_pos;
        type = SQUARE_BRACKET_FULL;
    }

    temp_pos = sub.find(angle_bracket_r);

    if (temp_pos < bracket_right_pos) {
        bracket_right_pos = temp_pos;
        type = ANGLE_BRACKET;
    }

    if (type == NONE) {
        return line;
    }

    if (type == SQUARE_BRACKET) {
        temp_pos = sub.find(square_bracket_l);
    } else if (type == SQUARE_BRACKET_FULL) {
        temp_pos = sub.find(square_bracket_lf);
    } else if (type == ANGLE_BRACKET) {
        temp_pos = sub.find(angle_bracket_l);
    }

    if (temp_pos < bracket_right_pos) {
        return line;
    }

    string::size_type start = (type == SQUARE_BRACKET ? 1 : 3);
    ret = sub.substr(bracket_right_pos + start);

    sub = line.substr(0, contain_pos);

    if (type == SQUARE_BRACKET) {
        bracket_left_pos = sub.rfind(square_bracket_l);
    } else if (type == SQUARE_BRACKET_FULL) {
        bracket_left_pos = sub.rfind(square_bracket_lf);
    } else if (type == ANGLE_BRACKET) {
        bracket_left_pos = sub.rfind(angle_bracket_l);
    }

    if (bracket_left_pos == string::npos || bracket_left_pos == 0) {
        return line;
    }

    sub = sub.substr(0, bracket_left_pos);

    ret = sub + " " + ret;

    string erased = line.substr(bracket_left_pos, line.size() - ret.size() + 1);
    log("Erase: " + erased);

    return ret;
}

void parser::erase_string(string& str, const string& erase)
{
    string::size_type pos;

    while ((pos = str.find(erase)) != string::npos) {
        str.erase(pos, erase.length());
        log("Erase: " + erase);
    }
}

void parser::erase_redundant_string(string& str,
                                    const vector<string>& erase_list)
{
    for (size_t i = 0; i < erase_list.size(); i++) {
        erase_string(str, erase_list.at(i));
    };
}

void parser::get_qq_news(const char* xml_file,
                         vector<weibo>& vec,
                         const vector<string>& block_list,
                         const vector<string>& erase_list)
{
    const string title_prefix = "      <title>";
    const string title_suffix = "</title>";
    const string desc_prefix = "      <description>";
    const string desc_suffix = "</description>";

    bool is_gb2312 = false;

    ifstream ifs;
    string line;
    line.reserve(600);
    string title;
    title.reserve(64);
    string desc;
    desc.reserve(600);
    string content;
    content.reserve(600);

    weibo wb;
    string::size_type title_pos;
    string::size_type desc_pos;

    ifs.open(xml_file, ios::in | ios::binary);

    if (!ifs) {
        log(string(xml_file) + ": file not found.");
        return;
    }

    ifs.seekg(0);

#if (!defined(USE_WGET) && !defined(USE_CURL))
    getline(ifs, line);
#endif

    getline(ifs, line); // encoding

    if (line.find("gb2312") < line.size()) {
        is_gb2312 = true;
    }

start:

    while (getline(ifs, line)) {
        if (line == "    <item>") {
            break;
        }
    }

    if (line == "") {
        ifs.close();
        return;
    }

    if (!getline(ifs, line))    // title
    {
        ifs.close();
        return;
    }

    if (is_gb2312 == true) {
        line = util::str::mbcs_to_utf8(line);

        if (line == "") {
            log("GB2312 -> UTF8 for title failed!");
            goto start;
        }
    }

    title_pos = line.find(title_prefix);

    if (title_pos == string::npos)
    {
        goto start;
    }

    title = line.substr(title_pos +
                        title_prefix.length(),
                        line.length() -
                        title_prefix.length() -
                        title_suffix.length() -
                        title_pos);

    while (title.find(' ') == 0) {
        title = title.substr(1, title.size() - 1);
    }

    getline(ifs, line); // link
    string::size_type link_start = line.find("http");
    if (link_start == string::npos) {
        log("Error: http not found in " + line);
    }
    string::size_type link_end = line.find("</link>");
    if (link_end == string::npos) {
        log("Error: </link> not found in " + line);
    }

    line = line.substr(link_start, link_end - link_start);

    string pub_link = line;

    if (find(published.begin(), published.end(), pub_link) != published.end()) {
        log("PASS " + pub_link);
        goto start;
    } else {
        published.push_back(pub_link);
        log("PUSH " + pub_link);

        if (published.size() > 1024) {
            published.pop_front();
        }
    }

    getline(ifs, line); // author
    getline(ifs, line); // category

    getline(ifs, line); // pubDate

    getline(ifs, line); // comments or description

    if (line.find("<comments/>") < line.size()) {
        getline(ifs, line); // description
    }

    if (line == "") {
        log("File end!");
        goto start;
    }

    if (is_gb2312 == true) {
        line = util::str::mbcs_to_utf8(line);

        if (line == "") {
            log("GB2312 -> UTF8 for desc failed!");
            goto start;
        }
    }

    desc_pos = line.find(desc_prefix);

    if (desc_pos == string::npos)
    {
        goto start;
    }

    desc = line.substr(desc_pos +
                       desc_prefix.length(),
                       line.length() -
                       desc_prefix.length() -
                       desc_suffix.length() -
                       desc_pos);

    desc = erase_news_source(desc);

    static const char br[][4] = {
        { -29, -128, -112, 0},  /* [ */
        { -29, -128, -111, 0},  /* ] */
    };

    content = br[0] + title + br[1] + desc;

    erase_redundant_string(content, erase_list);
    content = erase_parentheses(content);
    content = content.substr(0, (140 - 14) * 3);
    content = remove_rest_of_last_punctuation(content);

    if (content.size() > 40 && need_to_block(content, block_list) == false) {
        wb.url = pub_link;
        content = content  +  wb.url;
        wb.content = content;
        vec.push_back(wb);
    }

    goto start;
}