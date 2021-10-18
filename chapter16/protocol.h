#include <sys/types.h>

struct message {
    u_int32_t message_length;
    u_int32_t message_type;
    char buf[128];
};