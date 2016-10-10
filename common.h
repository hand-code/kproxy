//
// Created by 陈晓峰 on 16/9/11.
//

#ifndef _KPROXY_COMMON_H_
#define _KPROXY_COMMON_H_
#include <event.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <event2/listener.h>
#include <event2/util.h>

struct kp_server_s{
    struct event_base *ev_base;

    struct evconnlistener *ev_listener;


};

typedef struct kp_server_s kp_server_t;

/*************************kp_server**********************/

kp_server_t *kp_server_new();
void kp_server_free(kp_server_t *s);
int kp_server_init(kp_server_t *s, const char *ip, ushort port);

/*************************kp_utils***********************/
#define kp_malloc malloc

#define kp_free free

#define kp_calloc calloc

#define kp_realloc realloc

#define __QUOTE(x)              # x
#define  _QUOTE(x)              __QUOTE(x)

#define kp_log_debug(fmt, ...) do {                                                               \
        time_t      t  = time(NULL);                                                               \
        struct tm * dm = localtime(&t);                                                            \
                                                                                                   \
        fprintf(stdout, "[%02d:%02d:%02d] kp_server.c:[" _QUOTE(__LINE__) "]\t                %-26s: "  \
                fmt "\n", dm->tm_hour, dm->tm_min, dm->tm_sec, __func__, ## __VA_ARGS__);          \
        fflush(stdout);                                                                            \
} while (0)

#define kp_log_exit(fmt, ...) do{kp_log_debug(fmt, ## __VA_ARGS__);exit(-1);}while(0)

#endif /*_KPROXY_COMMON_H_*/

