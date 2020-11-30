#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

//Regular bold text
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define ANSI_RESET "\x1b[0m"

typedef long long LL;
const LL MOD = 1000000007;
#define part fprintf(stderr, "-----------------------------------------\n");
#define part2 fprintf(stderr, "====================================================\n");
#define part3 fprintf(stderr, "############################################################\n");
#define debug(x) printf("\n\"%s\" is: %d\n", #x, x);

#define MAX_CLIENTS 4
#define PORT_ARG 8000

const LL buff_sz = 1048576;

int main(int argc, char *argv[])
{
//   /  part;
    const int initial_msg_len = 256;

    //////////////////////////////////////////////////////////
    //mallocing for message string variables
    char *msg_from_client;
    char *msg_to_client;
    char *file_path;
    msg_from_client = (char *)malloc(sizeof(char) * initial_msg_len);
    msg_to_client = (char *)malloc(sizeof(char) * initial_msg_len);
    file_path = (char *)malloc(sizeof(char) * initial_msg_len);

    //AND buffer
    char *buffer_ptr = (char *)malloc(buff_sz * sizeof(char));

    //get welcoming socket
    //get ip,port

    int wel_socket_fd, client_socket_fd, port_number, clilen;

    int received_num, sent_num;
    struct sockaddr_in serv_addr_obj, client_addr_obj;
    int i, j, k, t, n;
    /////////////////////////////////////////////////////////////////////////
    /* create socket */
    /*
    The server program must have a special door—more precisely,
    a special socket—that welcomes some initial contact 
    from a client process running on an arbitrary host
    */
    wel_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (wel_socket_fd < 0)
    {
        perror("ERROR creating welcoming socket");
        exit(-1);
    }

    ////////////////////////////////////////////////////
    //https://stackoverflow.com/q/15198834/6427607
    //https://stackoverflow.com/q/5106674/6427607
    //https://stackoverflow.com/q/4233598/6427607
    //http://www.softlab.ntua.gr/facilities/documentation/unix/unix-socket-faq/unix-socket-faq-2.html#time_wait

    int option = 1;

    if (setsockopt(wel_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option)))
    {
        perror("setsockopt()");
        exit(-1);
    }
    //////////////////////////////////////////////////////////////////////
    /* IP address can be anything (INADDR_ANY) */
    bzero((char *)&serv_addr_obj, sizeof(serv_addr_obj));
    port_number = PORT_ARG;
    serv_addr_obj.sin_family = AF_INET;
    // On the server side I understand that INADDR_ANY will bind the port to all available interfaces,
    serv_addr_obj.sin_addr.s_addr = INADDR_ANY;
    serv_addr_obj.sin_port = htons(port_number); //process specifies port

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /* bind socket to this port number on this machine */
    /*When a socket is created with socket(2), it exists in a name space
       (address family) but has no address assigned to it.  bind() assigns
       the address specified by addr to the socket referred to by the file
       descriptor wel_sock_fd.  addrlen specifies the size, in bytes, of the
       address structure pointed to by addr.  */

    //CHECK WHY THE CASTING IS REQUIRED
    if (bind(wel_socket_fd, (struct sockaddr *)&serv_addr_obj, sizeof(serv_addr_obj)) < 0)
    {
        perror("Error on bind on welcome socket: ");
        exit(-1);
    }
    //////////////////////////////////////////////////////////////////////////////////////

    /* listen for incoming connection requests */

    listen(wel_socket_fd, MAX_CLIENTS);
    clilen = sizeof(client_addr_obj);

    while (1)
    {
        part3;
        /* accept a new request, create a client_socket_fd */
        /*
        During the three-way handshake, the client process knocks on the welcoming door
of the server process. When the server “hears” the knocking, it creates a new door—
more precisely, a new socket that is dedicated to that particular client. 
        */
        //accept is a blocking call
        printf("Waiting for a new client to request for a connection\n");
        client_socket_fd = accept(wel_socket_fd, (struct sockaddr *)&client_addr_obj, &clilen);
        if (client_socket_fd < 0)
        {
            perror("ERROR while accept() system call occurred in SERVER");
            exit(-1);
        }

        printf(BGRN"New client connected from port number %d and IP %s \n"ANSI_RESET, ntohs(client_addr_obj.sin_port), inet_ntoa(client_addr_obj.sin_addr));

        /* read message from client */
        int ret_val = 1;
        while (ret_val > 0)
        {
            //part2;
            //receive file_names_from_client

            //resetting string which holds file_name whose content needs to be sent
            memset(file_path, 0, initial_msg_len);
            //resetting string which server sends to client regarding the availability of asked file
            memset(msg_to_client, 0, initial_msg_len);

            //part 1 -> getting file which needs to be transferred
            received_num = read(client_socket_fd, file_path, initial_msg_len - 1);
            ret_val = received_num;
           // debug(ret_val);
            if (ret_val < 0)
            {
                perror("Error read()");
                printf("Server could not read msg sent from client\n");
                exit(-1);
            }
            if (ret_val == 0)
            {
                break;
            }
            LL file_path_len = strlen(file_path);

            printf(BYEL "File name to be transferred by server is %s\n" ANSI_RESET, file_path);

            //check if file exists in said location
            //try to open file and check if it has reqd permissions
            int fd1 = open(file_path, O_RDONLY);

            if (fd1 < 0)
            {
                perror("Error details");
                strcpy(msg_to_client, "N\0");

                //send via socket
                printf("Server could not find the said file in its own directory\n");
                int sent_msg = send(client_socket_fd, msg_to_client, strlen(msg_to_client), 0);
                if (sent_msg == -1)
                {
                    printf(BRED "ERROR: Server failed to send intended acknowledgement msg to client\n" ANSI_RESET);
                    exit(-1);
                }
                // exit(-1);

                continue;
            }


            //send success/error msg in that case + length of file for percentage printing

            //start reading drom file and also start transferring it

            //now is the amt of bytes which I want to read from the input file into our buffer
            int tot_sent_already = 0; //stores how many characters have already been sent

            struct stat in_obj;
            if (stat(file_path, &in_obj) < 0)
            {
                perror("Error details");
                printf(BRED "Error encountered by 'stat' involving input_file in server_end" ANSI_RESET);
                exit(-1);
            }
            LL tot_file_size = in_obj.st_size;
            printf(BYEL"Total file size is %lld\n" ANSI_RESET, tot_file_size);
            snprintf(msg_to_client, initial_msg_len, "Y %lld", tot_file_size);
            // printf(BGRN"Msg being sent to client is %s\n" ANSI_RESET, msg_to_client);
            int sent_msg = send(client_socket_fd, msg_to_client, strlen(msg_to_client), 0);
            if (sent_msg == -1)
            {
                printf("ERROR: Server failed to send intended acknowledgement msg to client\n");
                exit(-1);
            }


            //////////////////////////////////////////////////
            received_num = read(client_socket_fd, buffer_ptr, 5);
            ret_val = received_num;
            char ch = buffer_ptr[0];
            if (ch == 'A')
            {
                printf(BGRN "Client was able to open file on it's end\n" ANSI_RESET);
            }
            else
            {
                printf(BMAG "Client was NOT able to open file on it's end, MOVE ON TO NEXT FILE NOW\n" ANSI_RESET);
                ret_val = 0;
                continue;
            }

            //////////////////////////////////////

            //send to client
            while ((n = read(fd1, buffer_ptr, buff_sz)) > 0)
            {
                //  debug(n);
                int sent_to_client = write(client_socket_fd, buffer_ptr, n);
                //debug(sent_to_client);
                if (sent_to_client == -1)
                {
                    perror("Error while writing to client: ");
                    //decide course of action
                    //seems that the socket connection isn't well-established
                    goto close_client_socket_ceremony;
                    // exit(-1);
                }
                tot_sent_already += sent_to_client;
            }

            if (n == -1)
            {
                perror("Error details:");
                printf("Error while reading intended file by server, some bad error occurred\n");
                goto close_client_socket_ceremony;
                //exit(-1);
            }
            //close the file
           // printf(BYEL "File descriptor closed\n" ANSI_RESET);
            close(fd1);
            //part2;
        }
    close_client_socket_ceremony:
        close(client_socket_fd);
        printf(BRED "Disconnected from client"ANSI_RESET"\n");
    }

    close(wel_socket_fd);
    return 0;
}