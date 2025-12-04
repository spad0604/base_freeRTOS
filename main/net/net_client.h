#ifndef NET_CLIENT_H
#define NET_CLIENT_H

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

#ifdef __cplusplus
extern "C"
{
#endif
    typedef enum
    {
        NET_OK = 0,
        NET_ERR = -1,
        NET_NOT_IMPLEMENTED = -2
    } net_status_t;

    typedef void (*net_data_cb_t)(const char *topic_or_path, const uint8_t *data, size_t len, void *ctx);
    typedef void (*net_event_cb_t)(int event_code, void *ctx);

    net_status_t net_client_init(void);
    net_status_t net_client_deinit(void);
#ifdef __cplusplus
}
#endif

#endif // NET_CLIENT_H