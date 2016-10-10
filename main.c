#include "common.h"
#include <event2/util.h>

int
init(){
    kp_server_t *s = kp_server_new();
    kp_server_init(s, NULL, 8888);
    event_base_dispatch(s->ev_base);
    kp_server_free(s);
}

int
main() {
    init();
    return 0;
}
