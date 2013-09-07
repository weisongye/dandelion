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

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <queue>
#include <pthread.h>
#include <defs.h>

using std::queue;

enum message_type {
    MSG_EMPTY = 0,
    MSG_HTTP_COMPLETE,
    MSG_EXIT,
    MSG_END,
};

struct msg_blob {
    int handle;
    message_type type;
    WPARAM w_param;
    LPARAM l_param;
};

class message
{
public:
    static int init();
    static int deinit();
    static int send_messsage(int handle,
                             message_type type,
                             WPARAM w_param,
                             LPARAM l_param);

private:
    static bool running;
    const static int SLEEP_TIME = 100;
    static queue<msg_blob> msg_queue;
    static pthread_mutex_t queue_mutex;
    static message_type handle_message();
    static void* loop(LPVOID lp_param);
};

#endif /* MESSAGE_H_ */