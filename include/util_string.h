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

#ifndef UTILSTRING_H_
#define UTILSTRING_H_

#include <string>
using std::string;

namespace util
{
namespace str
{
string convert_code(const char* tocode,
                    const char* fromcode,
                    string str);
string mbcs_to_utf8(const string& mbcs);
string utf8_to_mbcs(const string& utf8);

string form_url_encode(const string& url);
string form_url_decode(const string& url);
string url_encode(const string& url);
}
}

#endif /* UTILSTRING_H_ */