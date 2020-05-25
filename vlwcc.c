#include <vlwcc.h>


int vlwccAppendToBuffer(char** buffer, unsigned int* buffer_current_size, unsigned int* buffer_max_size, const char* additional_buffer, unsigned int additional_buffer_length) {
    if ((*buffer_current_size) + additional_buffer_length > (*buffer_max_size)) {
        (*buffer_max_size) += VLWCC_RESERVE_MEMORY_EACH;
        (*buffer) = (char*)realloc((*buffer), sizeof(char) * (*buffer_max_size));
    }
    memcpy((char*)(*buffer) + (*buffer_current_size), additional_buffer, additional_buffer_length);
    (*buffer_current_size) += additional_buffer_length;
    return 0;
}

int vlwccSendMessageToServer(
        const char* address,
        const char* request,
        unsigned int request_len,
        char** response,
        unsigned int* response_len
) {
    // variables initialization
    int socket_fd, retval, nbytes;
    char buffer[VLWCC_BUFF_SIZE];
    char* formatted_request;
    struct sockaddr_in server_dir;

    // initializes the socket and check for errors
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
        return VLWCC_SOCKET_ERROR + errno;

    // builds the sockaddr_in struct containing the necessary information to identify the server
    bzero((char*)&server_dir, sizeof(server_dir));
    server_dir.sin_family = AF_INET;
    server_dir.sin_addr.s_addr = inet_addr(address);
    server_dir.sin_port = htons(VLWCC_DEFAULT_PORT);

    // connects tho the server
    retval = connect(
            socket_fd,
            (const struct sockaddr*) &server_dir,
            sizeof(server_dir)
    );
    if (retval < 0)
        return VLWCC_CONNECT_ERROR + errno;

    // prepare the formatted request, this should follow the next syntax
    // {+,-}[control message]<[payload]>
    // the + indicates all is ok, normally the control message is not needed here
    // the - indicates the presence of errors, they should be indicated on the control message field
    // as we are sending the first message, an error is not expected here, there should be no control message.
    formatted_request = (char*)malloc(sizeof(char) * (request_len + 3));
    strcpy(formatted_request, "+<");
    strcat(formatted_request, request);
    strcat(formatted_request, ">");

    #ifdef DEBUG
        printf("sending request to \"%s\"\n", address);
        printf("request \"%s\"%i\n", formatted_request, request_len);
    #endif
    // sends the formatted payload to the server
    nbytes = send(
            socket_fd,
            formatted_request,
            request_len + 3,
            0
    );
    // free the formatted payload memory
    free(formatted_request);
    // check for errors in the send
    if (nbytes < 0)
        return VLWCC_SEND_ERROR + errno;

    *response_len = 0;
    bzero(*response, strlen(*response));
    do {
        // read from the socket file desriptor
        bzero(buffer, VLWCC_BUFF_SIZE);
        nbytes = read(
                socket_fd,
                buffer,
                VLWCC_BUFF_SIZE
        );
        // checks for errors
        if (nbytes < 0)
            return VLWCC_READ_ERROR + errno;
        // add the read bytes to the response length
        (*response_len) += nbytes;
        #ifdef DEBUG
            printf("resizing response to %i\n", *response_len);
        #endif
        // reallocate the memory with the new size
        (*response) = (char*)realloc((*response), sizeof(char) * (*response_len));
        // copy the newly read buffer into the final response
        // can't use strcat Using it is running off the end of the array and corrupting the data structures
        // used by realloc
        // start to copy at the start of the newly reallocated memory
        memcpy((char*)*response + (*response_len) - nbytes, buffer, nbytes);
        #ifdef DEBUG
            printf("currently read \"%s\"%lu\n", *response, strlen(*response));
        #endif
    } while (buffer[strlen(buffer)-1]!='>'); // this indicates the end of payload
    #ifdef DEBUG
        printf("full response \"%s\"%lu\n", *response, strlen(*response));
    #endif

    // return the ok status (0)
    return VLWCC_OK;
}