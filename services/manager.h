#ifndef __MUNIX_SERVICE_MANAGER
#define __MUNIX_SERVICE_MANAGER


typedef void (*service_initializer_t)(void);
typedef void (*service_reciever_t)(char *, char *);

typedef struct __munix_service_t {
    const char *name;
    service_initializer_t initializer;
    service_reciever_t reciever;
} service_t;


void service_init(void);

void service_send_message(const char *name, const char *data);
void service_recieve_message(const char *name, const char *output);

#endif
