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

#ifndef PARSER_H_
#define PARSER_H_

#include <vector>
#include <string>

#include "weibo.h"

using std::vector;
using std::string;

class parser
{
public:
    parser();
    ~parser();

    static void get_qq_news(const char* xml_file,
                            vector<weibo>& vec,
                            const vector<string>& block_list,
                            const vector<string>& erase_list);
    static int parse_xml_group(const vector<string>& urls,
                               vector<weibo>& vec,
                               const vector<string>& block_list,
                               const vector<string>& erase_list);
    static string remove_rest_of_last_punctuation(const string& line);
    static void erase_string(string& str, const string& erase);
    static void erase_redundant_string(string& str,
                                       const vector<string>& erase_list);
    static string erase_parenthes(const string& line, const string& contain);
    static string erase_parentheses(const string& line);
    static string erase_news_source(const string& line);
    static bool need_to_block(const string& str,
                              const vector<string>& block_list);
};

#endif /* PARSER_H_ */
