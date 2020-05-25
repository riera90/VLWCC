#ifndef _VLWCC_H_
#define _VLWCC_H_

#define VLWCC_DEFAULT_PORT 7654
#define VLWCC_BUFF_SIZE 2048
#define VLWCC_RESERVE_MEMORY_EACH 2048

#define VLWCC_OK 0
#define VLWCC_SOCKET_ERROR 1000
#define VLWCC_CONNECT_ERROR 2000
#define VLWCC_SEND_ERROR 3000
#define VLWCC_READ_ERROR 4000

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef DEBUG
    #include <stdio.h>
#endif

int vlwccAppendToBuffer(
        char** buffer,
        unsigned int* buffer_current_size,
        unsigned int* buffer_max_size,
        const char* additional_buffer,
        unsigned int additional_buffer_length
);

/**
 * @warning This is a blocking function
 * Sends a command to the server and waits for the response
 *
 * @param region region sub-domain example: spain, france, italy, canada...
 *          your region might not be active!
 *          please check the project main page to see the active regions
 * @param request payload for the regional master node
 * @param request_len length of the payload
 * @param response pointer to initialized dynamic memory char array
 * @param response_len pointer to future container of response length
 * @return error code xyyy where x is the type or error and yyy is the specific error
 *          1 series error is refereed to socket errors      (socket  unix function)
 *          2 series error is refereed to connection errors  (connect unix function)
 *          3 series error is refereed to sending errors     (send    unix function)
 *          4 series error is refereed to receiving errors   (read    unix function)
 *
 *          yyy errors should be checked on the linux manual of each function
 */
int vlwccSendMessageToServer(
        const char* address,
        const char* request,
        unsigned int request_len,
        char** response,
        unsigned int* response_len
);

#endif
