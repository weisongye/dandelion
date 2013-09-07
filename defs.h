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

#ifndef DEFS_H_
#define DEFS_H_

//#define DEBUG_MODE

#ifdef WIN32
#include <windows.h>
#include <winsock.h>
#include <process.h>
#pragma comment (lib,"ws2_32.lib")
#pragma warning(disable:4996)

typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
typedef int             int32_t;

#define sleep           delay
#define socklen_t       int
#define socket_t        SOCKET
#define s_addr          S_un.S_addr

#define delay(x)    Sleep(x)

inline int init_socket()
{
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;

    int err = WSAStartup(wVersionRequested, &wsaData);

    if (err != 0) {
        return -1;
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        WSACleanup();
        return -1;
    }

    return 0;
}

#else /* WIN32 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <errno.h>
#include <cctype>
#include <cstdio>
#include <cstring>

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#define __stdcall

#define far
#define near

#undef  FAR
#undef  NEAR
#define FAR                 far
#define NEAR                near
#ifndef CONST
#define CONST               const
#endif

typedef unsigned long        DWORD;
typedef int                  BOOL;
typedef unsigned char        BYTE;
typedef unsigned short       WORD;
typedef float                FLOAT;
typedef FLOAT*               PFLOAT;
typedef BOOL near*           PBOOL;
typedef BOOL far*            LPBOOL;
typedef BYTE near*           PBYTE;
typedef BYTE far*            LPBYTE;
typedef int near*            PINT;
typedef int far*             LPINT;
typedef WORD near*           PWORD;
typedef WORD far*            LPWORD;
typedef long far*            LPLONG;
typedef DWORD near*          PDWORD;
typedef DWORD far*           LPDWORD;
typedef void far*            LPVOID;
typedef CONST void far*      LPCVOID;

typedef int                  INT;
typedef unsigned int         UINT;
typedef unsigned int*        PUINT;

#if defined(__x86_64)
typedef int64_t  INT_PTR, *PINT_PTR;
typedef uint64_t UINT_PTR, *PUINT_PTR;

typedef int64_t  LONG_PTR, *PLONG_PTR;
typedef uint64_t ULONG_PTR, *PULONG_PTR;
#else
typedef int32_t  INT_PTR, *PINT_PTR;
typedef uint32_t UINT_PTR, *PUINT_PTR;

typedef int32_t  LONG_PTR, *PLONG_PTR;
typedef uint32_t ULONG_PTR, *PULONG_PTR;
#endif

/* Types use for passing & returning polymorphic values */
typedef UINT_PTR            WPARAM;
typedef LONG_PTR            LPARAM;
typedef LONG_PTR            LRESULT;

typedef wchar_t WCHAR;    // wc,   16-bit UNICODE character
typedef WCHAR TCHAR, *PTCHAR;
typedef WCHAR TBYTE , *PTBYTE ;

#define closesocket close
#define socket_t    int

#define delay(x)    usleep((x) * 1000)

inline char* itoa(int value, char* str, int radix)
{
    sprintf(str, "%d", value);
    return str;
}

inline char* strlwr(char* str)
{
    char* orig = str;

    for (; *str != '\0'; str++) {
        *str = tolower(*str);
    }

    return orig;
}

inline int init_socket()
{
    return 0;
}

inline int strncpy_s(char* dest,
                     size_t number_of_elements,
                     const char* src,
                     size_t count)
{
    int n = (count < number_of_elements ? count : number_of_elements);
    char* ret = strncpy(dest, src, n);
    ret[n] = 0;

    return 0;
}

#endif /* WIN32 */

#endif /* DEFS_H_ */
