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

#ifndef WEIBO_H_
#define WEIBO_H_

#include <string>
using std::string;

class weibo
{
public:
    string content;
    string url;
    string pic_url;

    weibo() {
        url.reserve(33);
        pic_url.reserve(60);
    }

    friend bool operator < (const weibo& x, const weibo& y) {
        return (x.content < y.content);
    }

    friend bool operator > (const weibo& x, const weibo& y) {
        return (x.content > y.content);
    }

    friend bool operator == (const weibo& x, const weibo& y) {
        return (x.content == y.content);
    }
};

#endif /* WEIBO_H_ */
