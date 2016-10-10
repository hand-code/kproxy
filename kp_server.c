/*
* ----------------------------------------------------------------------------
* Copyright (c) 2015-2016, nbboy <lazymather at gmail dot com>
* All rights reserved.
* Distributed under GPL license.
* ----------------------------------------------------------------------------
*/

#include "common.h"
#include <event2/util.h>
#include <event2/http.h>

#define KP_SOCKET_BACKLOG 500

static void listener_accept_cb(struct evconnlistener *, evutil_socket_t,
                               struct sockaddr *, int socklen, void *);

kp_server_t *
kp_server_new(){
    kp_server_t *s = NULL;
    s = (kp_server_t*)kp_malloc(sizeof(struct kp_server_s));
    s->ev_base = event_base_new();
    if(-1 == event_base_priority_init(s->ev_base, 1))
        kp_log_exit("priority_init error\n");
    return s;
}

void
kp_server_free(kp_server_t *s){
    if(s){
        if(s->ev_base){
            event_base_free(s->ev_base);
            kp_free(s);
        }

        if(s->ev_listener){
            evconnlistener_free(s->ev_listener);
        }
    }
}

int
kp_server_init(kp_server_t *s, const char *ip, ushort port){
    evutil_socket_t ss;
    struct evconnlistener *ev_conn;
    struct sockaddr_in addr;

    ss = socket(PF_INET, SOCK_STREAM, 0);
    if(ss == -1){
        kp_log_exit("socket call error\n");
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    bind(ss, (struct sockaddr*)&addr, sizeof(addr));

    evutil_make_socket_nonblocking(ss);
    ev_conn = evconnlistener_new(s->ev_base,listener_accept_cb, s, LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE,
                       KP_SOCKET_BACKLOG, ss);
    if(!ev_conn){
        kp_log_exit("evconnlistener_new call error\n");
    }
    s->ev_listener = ev_conn;
    return 0;
}

static void
revent_cb(evutil_socket_t ev_socket, short event, void *arg){
    kp_server_t *s = (kp_server_t*)arg;
    char buffer[32];
    char output_buffer[1024];
    ssize_t nbytes;
    ssize_t sbytes = 0;

    if(EV_READ & event){
        do{
            nbytes = read(ev_socket, buffer, 32);
            if(nbytes == 0){/*EOF*/
                break;
            }
            else if(nbytes == -1){/*ERROR*/
                kp_log_exit("read error\n");break;
            }else{/*Continue read*/
                memcpy(&output_buffer[sbytes], buffer, nbytes);
                sbytes += nbytes;
                if(nbytes < 32)break;
            }
        }while (1);
        output_buffer[sbytes] = (char) '\0';
        kp_log_debug("buffer %s\n", output_buffer);
    }else if(EV_WRITE & event){
        write(ev_socket, "OK", strlen("OK"));
    }
}

//static void
//wevent_cb(evutil_socket_t ev_socket, short event, void *arg) {
//}

void read_cb(struct bufferevent *bev, void *ctx){
    struct evbuffer *output, *input = NULL;
    char buffer[512];

    input = bufferevent_get_input(bev);
    size_t input_length = evbuffer_get_length(input);
    output = bufferevent_get_output(bev);

    bufferevent_read(bev, buffer, input_length);
    buffer[input_length] = (char)'\0';
    evbuffer_add_printf(output, "%s%s\r\n", buffer, "OK");
}

void event_cb(struct bufferevent *bev, short what, void *ctx){
    //struct bufferevent *s = (struct bufferevent*)ctx;

    if(what & BEV_EVENT_EOF || what & BEV_EVENT_ERROR ||
            what & BEV_EVENT_TIMEOUT){
        bufferevent_free(bev);
        kp_log_debug("free bufferevent s\n");
    }
}

static void
listener_accept_cb(struct evconnlistener * listener, evutil_socket_t ev_socket,
                               struct sockaddr * s_addr, int s_addr_len, void * arg){
    char buffer[20];
    struct event *read_event, *write_event;
  
    evutil_inet_ntop(AF_INET, s_addr->sa_data, buffer, s_addr->sa_len);
    kp_log_debug("new a socket, (addr: %s)", buffer);
     
    struct bufferevent  *bev = NULL;
//    read_event = event_new(evconnlistener_get_base(listener), ev_socket,
//              EV_READ|EV_WRITE, revent_cb, arg);
//    evutil_make_socket_nonblocking(ev_socket);
//    evutil_make_listen_socket_reuseable(ev_socket);
//    event_add(read_event, NULL);

//    write_event = event_new(evconnlistener_get_base(listener), ev_socket,
//            EV_WRITE|EV_PERSIST, wevent_cb, arg);
//    event_add(write_event, NULL);

    bev = bufferevent_socket_new(evconnlistener_get_base(listener), ev_socket, BEV_OPT_CLOSE_ON_FREE|
    BEV_OPT_DEFER_CALLBACKS);
    bufferevent_setcb(bev, read_cb, NULL, event_cb, bev);
    bufferevent_enable(bev, EV_WRITE|EV_READ);
}

