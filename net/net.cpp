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

#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <curl/curl.h>
#include "defs.h"
#include "net/net.h"
#include <log.h>

namespace net
{

#if defined(USE_WGET)
int download(const char* url, const char* local_file)
{
    string cmd;
    cmd.reserve(66);

#ifdef WIN32
    cmd.append("win32\\bin\\");
#endif

    cmd.append("wget ");
    cmd.append(url);
    cmd.append(" -O ");
    cmd.append(local_file);

    return system(cmd.c_str());
}

#elif defined(USE_CURL)

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

int download(const char* url, const char* local_file)
{
    CURL *curl;
    FILE *fp;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(local_file,"wb");
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
        log(string(local_file) + " downloaded.");
    }
    return 0;
}

#else

int download(const char* url, const char* local_file)
{
    const unsigned short port = 80;
    char domain[32];
    char file[128];

    if (url == NULL) {
        return 0;
    }

    memset(domain, 0, sizeof(domain));
    memset(file, 0, sizeof(file));

    size_t url_len = strlen(url);

    size_t i = 0;
    size_t offset = 0;

    if (!strncmp(url, "http://", 7)) {
        offset = 7;
    }

    for (i = 0 ; i < url_len; i++) {
        if (url[i + offset] == '/' || i == sizeof(domain)) {
            break;
        }

        domain[i] = url[i + offset];
    }

    if (i + offset >= url_len || i >= sizeof(domain)) {
        return -4;
    }

    strncpy((char*)file, url + i + offset, sizeof(file));

    cout << "Downloading " << url << endl;

    init_socket();

    socket_t sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sockfd == -1) {
        cout << "socket() failed." << endl;
        return -1;
    }

    hostent* hp;
    uint32_t addr;

    if (inet_addr(domain) == INADDR_NONE) {
        hp = gethostbyname(domain);
    } else {
        addr = inet_addr(domain);
        hp = gethostbyaddr((char*) &addr, sizeof(addr), AF_INET);
    }

    if (hp == NULL) {
        cout << "gethostbyaddr() failed." << endl;
        closesocket(sockfd);
        return -1;
    }

    struct sockaddr_in clisa;

    clisa.sin_addr.s_addr = *((uint32_t*) hp->h_addr);

    clisa.sin_family = AF_INET;

    clisa.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr*) &clisa, sizeof(clisa))) {
        closesocket(sockfd);
        return -1;
    }

    const uint32_t BUFFER_SIZE = 4096;
    char recvbuf[BUFFER_SIZE] = {0};

    sprintf(recvbuf,
            "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/4.0\r\n\r\n",
            file, domain);

    int sent = send(sockfd, recvbuf, (uint32_t) strlen(recvbuf), 0);

    if (sent == -1) {
        return -1;
    }

    int32_t received = recv(sockfd, recvbuf, sizeof(recvbuf), 0);

    if (received > 9 && recvbuf[9] != '2') {
        // if not HTTP/1.x 2xx , quit
        closesocket(sockfd);
#ifdef WIN32
        WSACleanup();
#endif
        return -3;
    }

    ofstream outfile(local_file, ios::out | ios::binary);

    // Remove the header
    string filestart(recvbuf);
    filestart = filestart.substr(filestart.find("\r\n\r\n") + 4);
    outfile.seekp(0);
    outfile.write(filestart.c_str(), filestart.size());

    for (; ;) {
        received = recv(sockfd, recvbuf, sizeof(recvbuf), 0);
        cout << received << " ";

        if (received <= 0) {
            break;
        }

        outfile.write(recvbuf, received);
    }

    cout << endl;

    closesocket(sockfd);

#ifdef WIN32
    WSACleanup();
#endif
    outfile.close();

    cout << local_file << " downloaded." << endl;
    return 0;
};

#endif

}