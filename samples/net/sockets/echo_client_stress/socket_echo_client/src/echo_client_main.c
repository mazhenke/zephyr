/*
 * Copyright (c) 2017 Linaro Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#ifndef __ZEPHYR__

#include <stdint.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define CONFIG_NET_APP_PEER_IPV6_ADDR   "fe80::ad59:4e45:bd12:7d43"
#define CONFIG_NET_APP_PEER_IPV4_ADDR   "192.168.1.100"
static uint32_t k_cycle_get_32(void)
{
    return 0;
}
#else

#include <kernel.h>
#include <net/net_app.h>
#include <net/socket.h>

#endif
#include "common.h"

#undef TEST_IPV6

#define PRINT(fmt, ...) printf("[0x%08x ]" fmt, k_cycle_get_32(), ##__VA_ARGS__)

/* Socket server to connect to */
#ifndef SOCKET_HOST
#ifdef TEST_IPV6
#warning "SOCKET Server not define, use default: fe80::ad59:4e45:bd12:7d43"
#define SOCKET_HOST CONFIG_NET_APP_PEER_IPV6_ADDR
#else
#warning "SOCKET Server not define, use default: 192.168.1.100"
#define SOCKET_HOST CONFIG_NET_APP_PEER_IPV4_ADDR
#endif
#endif

#define SOCKET_PORT 5555

#define MIN_SIZE (4)
#define MAX_SIZE (1024)
#define BUF_SIZE (MAX_SIZE + MIN_SIZE)
static char request[BUF_SIZE];
static char response[BUF_SIZE];

uint32_t init_data(void)
{
    uint32_t i   = 0;
    uint32_t len = 0;
    uint8_t c    = 0;
    do {
        len = rand32();
        len &= (MAX_SIZE - 1);
    } while (len <= MIN_SIZE || len >= (MAX_SIZE - 1));

    PRINT("Length: 0x%lx(%ld)\n", len, len);

    memset(request, 0, BUF_SIZE);
    memset(response, 0, BUF_SIZE);

    c = rand_char();
    //    rand_string_no_efs(request, len);
    for (i = 0; i < len; i++) {
        request[i] = c;
    }
    request[len]     = '\r';
    request[len + 1] = '\n';
    request[len + 2] = '\0';
    return len + 2; /* exclude \0 */
}

void dump_addrinfo(const struct addrinfo *ai)
{
    PRINT("addrinfo @%p: ai_family=%d, ai_socktype=%d, ai_protocol=%d, "
          "sa_family=%d, sin_port=%x\n",
          ai,
          ai->ai_family,
          ai->ai_socktype,
          ai->ai_protocol,
          ai->ai_addr->sa_family,
          ((struct sockaddr_in *)ai->ai_addr)->sin_port);
}

extern void dhcpv4_get_ip(void);
extern int is_check_ipv4_ready(void);

ssize_t sendall(int sock, const void *buf, size_t len)
{
    while (len) {
        ssize_t out_len = send(sock, buf, len, 0);
        if (out_len < 0) {
            PRINT("errno: 0x%x\n", errno);
            return out_len;
        }
        buf = (const char *)buf + out_len;
        len -= out_len;
    }

    return 0;
}

void socket_thread_loop(void)
{
    int ret = 0;
#if 0
	static struct addrinfo hints;
#endif
#ifdef TEST_IPV6
    struct sockaddr_in6 dest;
#else
    struct sockaddr_in dest;
#endif
#if 0
	struct addrinfo *res;
#endif
    int sock;

    //    while (!is_check_ipv4_ready());
    PRINT("IP is ready!\n");
#if 0
	PRINT("Preparing HTTP GET request for http://" SOCKET_HOST
	       ":" SOCKET_PORT SOCKET_PATH "\n");


	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	st = getaddrinfo(SOCKET_HOST, SOCKET_PORT, &hints, &res);
	PRINT("getaddrinfo status: %d errno: %d\n", st, errno);
	if (st != 0) {
		PRINT("Unable to resolve address, quitting\n");
		return;
	}
#endif

#if 0
	for (; res; res = res->ai_next) {
		dump_addrinfo(res);
	}
#endif

    memset(&dest, 0, sizeof(dest));
#ifdef TEST_IPV6
    dest.sin6_family = AF_INET6;
    dest.sin6_port   = htons(SOCKET_PORT);
    if (inet_pton(AF_INET6, SOCKET_HOST, &dest.sin6_addr) < 0) {
        PRINT("inet_pton failed!\n, errno: %d\n", errno);
        return;
    }
#else
    dest.sin_family = AF_INET;
    dest.sin_port   = htons(SOCKET_PORT);
    if (inet_pton(AF_INET, SOCKET_HOST, &dest.sin_addr) < 0) {
        PRINT("inet_pton failed!\n, errno: %d\n", errno);
        return;
    }
#endif

// PRINT("before socket!\n");
#ifdef TEST_IPV6
    sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
#else
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif
    // PRINT("after socket!\n");
    if (-1 == sock) {
        PRINT("socket failed! errno: %d\n", errno);
        return;
    }

    // PRINT("sock = %d\n", sock);
    // PRINT("before connect!\n");
    // k_busy_wait(1000000);
    ret = connect(sock, (struct sockaddr *)&dest, sizeof(dest));
    if (-1 == ret) {
        PRINT("connect failed! errno: %d\n", errno);
        return;
    }
    // PRINT("after connect!\n");

    while (1) {
        uint32_t send_len = 0;
        send_len          = init_data();

        // PRINT("before send!\n");
        ret = sendall(sock, request, send_len);
        if (0 != ret) {
            PRINT("sendall failed! errno: %d\n", errno);
            return;
        }

        // PRINT("after send!\n");
        // PRINT("Send: %s\n", request);

        // PRINT("Response:\n\n");

        uint32_t received_len = 0;
        while (send_len != received_len) {
            // PRINT("before recv!\n");
            uint32_t len =
                recv(sock, response + received_len, send_len - received_len, 0);
            // PRINT("after recv!\n");
            received_len += len;
            PRINT("recv len: 0x%lx total len: 0x%lx\n", len, received_len);

            if (len < 0) {
                PRINT("Error reading response\n");
                return;
            }

            if (len == 0) {
                PRINT("Close!!!\n");
                break;
            }
        }

        /* check data */
        if ((0 != memcmp(response, request, send_len))
            || (received_len != send_len)) {
            PRINT("Not Match!!!\n");
            PRINT("Send Len: 0x%lx, Receive Len: 0x%lx\n",
                  send_len,
                  received_len);
            PRINT("Send: %s\n", request);
            PRINT("Receive: %s\n", response);

            close(sock);
            return;
        } else {
            PRINT("Match!\n");
        }
    }

    // PRINT("before close!\n");
    close(sock);
    // PRINT("after close!\n");

    return;
}

int main(void)
{
    PRINT("DHCP v4 GetIP Start!\n");
    // dhcpv4_get_ip();
    PRINT("DHCP v4 GetIP End!\n");
#ifndef __ZEPHYR__
    socket_thread_loop();
#endif
    return 0;
}

#ifdef __ZEPHYR__
#define STACKSIZE 2048
#define PRIORITY 7
K_THREAD_DEFINE(socket_connect_thread_id,
                STACKSIZE,
                socket_thread_loop,
                NULL,
                NULL,
                NULL,
                PRIORITY,
                0,
                K_NO_WAIT);
#endif
