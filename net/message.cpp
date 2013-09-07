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

#include <defs.h>
#include <log.h>
#include "http.h"
#include "message.h"

bool message::running = false;
queue<msg_blob> message::msg_queue;
pthread_mutex_t message::queue_mutex;

int message::init()
{
    pthread_t tid;

    if (pthread_create(&tid, NULL, loop, NULL) != 0)
    {
        log("message::init(): pthread_create() failed.");
        return -1;
    }

    running = true;

    return 0;
}

int message::deinit()
{
    message::send_messsage(0, MSG_EXIT, NULL, NULL);
    running = false;
    return 0;
}

int message::send_messsage(int handle, message_type type,
                           WPARAM w_param, LPARAM l_param)
{
    msg_blob b;
    b.handle = handle;
    b.type = type;
    b.w_param = w_param;
    b.l_param = l_param;

    if (running == false) {
        return 1;
    }

    pthread_mutex_lock(&queue_mutex);
    msg_queue.push(b);
    pthread_mutex_unlock(&queue_mutex);

    return 0;
}

message_type message::handle_message()
{
    pthread_mutex_lock(&queue_mutex);

    if (msg_queue.empty()) {
        pthread_mutex_unlock(&queue_mutex);
        return MSG_EMPTY;
    }

    msg_blob blob = msg_queue.front();
	msg_queue.pop();

	pthread_mutex_unlock(&queue_mutex);

    switch (blob.type) {
    case MSG_HTTP_COMPLETE:
        http_proxy::get_instance()->on_http_complete(blob.w_param,
                                                     blob.l_param);
        break;

    case MSG_EXIT:
        log("========== GET EXIT MSG ==========");
        break;

    default:
        break;
    }

    return blob.type;
}

void* message::loop(LPVOID lp_param)
{
    lp_param = lp_param;

    pthread_mutex_init(&queue_mutex, NULL);

    for ( ; ; ) {
        if (handle_message() == MSG_EXIT) {
            goto out;
        }

        delay(SLEEP_TIME);
    }

out:
    log("========== EXIT MSG LOOP ==========");
    pthread_mutex_destroy(&queue_mutex);

    pthread_detach(pthread_self());
    return NULL;
}