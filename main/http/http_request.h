#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "net/net_client.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* Simple async HTTP request:
       - method: "GET" or "POST"
       - url: full URL
       - body: nullable (for POST)
       - timeout_ms: overall timeout (blocking used internally if needed)
       - on_response: called when response data available (may be called multiple times for chunks)
       - ctx: user context passed to callback
    */
    net_status_t http_request_async(
        const char *method,
        const char *url,
        const uint8_t *body,
        uint32_t timeout_ms,
        net_data_cb_t on_response,
        void *ctx);
    /* Simple blocking HTTP request:
       - method: "GET" or "POST"
       - url: full URL
       - body: nullable (for POST)
       - timeout_ms: overall timeout
       - out_response: pointer to allocated response buffer (must be freed by caller)
       - out_response_len: length of response data
    */
    net_status_t http_request_blocking(
        const char *method,
        const char *url,
        const uint8_t *body,
        uint32_t timeout_ms,
        uint8_t **out_response,
        size_t *out_response_len);

    /**
     * @brief Convenience wrapper for HTTP GET requests.
     * @param url The full URL to send the GET request to.
     * @param timeout_ms Overall timeout for the request in milliseconds.
     * @param on_response Callback function to handle response data.
     * @param ctx User-defined context passed to the callback.
     * @return net_status_t Status of the HTTP GET request.
     */
    net_status_t http_get_async(
        const char *url,
        uint32_t timeout_ms,
        net_data_cb_t on_response,
        void *ctx);

    /**
     * @brief Convenience wrapper for HTTP POST requests.
     * @param url The full URL to send the POST request to.
     * @param body Pointer to the request body data.
     * @param timeout_ms Overall timeout for the request in milliseconds.
     * @param on_response Callback function to handle response data.
     * @param ctx User-defined context passed to the callback.
     * @return net_status_t Status of the HTTP POST request.
     */
    net_status_t http_post_async(
        const char *url,
        const uint8_t *body,
        uint32_t timeout_ms,
        net_data_cb_t on_response,
        void *ctx);

    /**
     * @brief Convenience wrapper for blocking HTTP GET requests.
     * @param url The full URL to send the GET request to.
     * @param timeout_ms Overall timeout for the request in milliseconds.
     * @param out_response Pointer to allocated response buffer (must be freed by caller).
     * @param out_response_len Length of response data.
     * @return net_status_t Status of the HTTP GET request.
     */
    net_status_t http_get_blocking(
        const char *url,
        uint32_t timeout_ms,
        uint8_t **out_response,
        size_t *out_response_len);

    /**
     * @brief Convenience wrapper for blocking HTTP POST requests.
     * @param url The full URL to send the POST request to.
     * @param body Pointer to the request body data.
     * @param timeout_ms Overall timeout for the request in milliseconds.
     * @param out_response Pointer to allocated response buffer (must be freed by caller).
     * @param out_response_len Length of response data.
     * @return net_status_t Status of the HTTP POST request.
     */
    net_status_t http_post_blocking(
        const char *url,
        const uint8_t *body,
        uint32_t timeout_ms,
        uint8_t **out_response,
        size_t *out_response_len);

#ifdef __cplusplus
}

#endif
#endif // HTTP_REQUEST_H