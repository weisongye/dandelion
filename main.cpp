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
#include <pthread.h>
#include <curl/curl.h>
#include "defs.h"
#include "auth.h"
#include "robot.h"
#include <log.h>
#include <net/message.h>

using namespace std;

extern pthread_mutex_t log_mutex;

void show_banner()
{
    cout << "-------------------------------------------------------" << endl;
    cout << "Dandelion\t(Build " << __DATE__ << " " << __TIME__ << ")" << endl;
    cout << endl;
    cout << "Copyright (C) 2013 Phoebus Veiz <phoebusveiz@gmail.com>" << endl;
    cout << "Licensed under the Apache License, Version 2.0." << endl;
    cout << "See source distribution for full notice." << endl;

    cout << "-------------------------------------------------------" << endl;
    cout << endl;
}

int main()
{
    show_banner();

    pthread_mutex_init(&log_mutex, NULL);
    curl_global_init(CURL_GLOBAL_ALL);

    message::init();

    log("Create a new robot.");
    robot* r = new robot();
    r->send_news();
    delete r;

    message::deinit();
    delay(6000);
    curl_global_cleanup();
    pthread_mutex_destroy(&log_mutex);

    return 0;
}
