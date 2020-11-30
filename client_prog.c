#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/wait.h>

typedef long long LL;
const LL MOD = 1000000007;
#define part fprintf(stderr, "-----------------------------------------\n");
#define part2 fprintf(stderr, "====================================================\n");
#define part3 fprintf(stderr, "############################################################\n");
#define debug(x) printf("\n\"%s\" is: %d\n", #x, x);

const LL buff_sz = 1048576;
#define MAX_CLIENTS 4
#define max_files_num 50
#define PORT_ARG 8000

//Regular bold text
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define BWHT "\e[1;37m"
#define ANSI_RESET "\x1b[0m"

///////////////////////////////////////////////////
char *file_paths[max_files_num];
int curr_files_num;

char percent_str[20];
char status_msg[] = "\rTask completion percentage is ";
/////////////////////////////////////////////////
void print_percentage_done(const double val)
{
    // The sprintf() function shall place output followed by the null byte, '\0'
    sprintf(percent_str, "%.2lf", val);
    printf("%s ", status_msg);
    printf("%s %c", percent_str, 37);
    fflush(stdout);
}

void init_file_paths()
{
    for (int i = 0; i < max_files_num; i++)
    {
        file_paths[i] = (char *)malloc(sizeof(char) * 100);
    }
}
// int fill_file_paths(char *cmd_input)
// {
//     // printf("trying to parse cmd\n");
//     // printf("To be parsed is %s\n", cmd_input);
//     curr_files_num = 0;
//     //https://man7.org/linux/man-pages/man3/strtok.3.html
//     char delims[] = " \t";
//     char *token_beg;
//     char *first_word = NULL;

//     token_beg = strtok(cmd_input, delims);

//     int is_get = 0;
//     if (token_beg != NULL)
//     {
//         first_word = token_beg;
//         token_beg = strtok(NULL, delims);
//     }
//     while (token_beg != NULL)
//     {
//         file_paths[curr_files_num++] = token_beg;
//         token_beg = strtok(NULL, delims);
//     }
//     printf("FIRST WORD IS : %s\n", first_word);
//     if (first_word == NULL)
//     {
//         return 0;
//     }
//     if (strcmp(first_word, "get") == 0)
//     {
//         return 1;
//     }
//     else
//     {
//         return 0;
//     }
// }

int get_socket_fd(struct sockaddr_in *ptr)
{
    struct sockaddr_in server_obj = *ptr;

    int port_num = PORT_ARG;
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        perror("Error in socket creation for CLIENT");
        exit(-1);
    }
    /////////////////////////////////////////////////////////////////////////////////////

    /* fill in server address in sockaddr_in datastructure */
    memset(&server_obj, 0, sizeof(server_obj)); // Zero out structure
    server_obj.sin_family = AF_INET;
    server_obj.sin_port = htons(port_num); //convert to big-endian order

    // Converts an IP address in numbers-and-dots notation into either a
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    //https://stackoverflow.com/a/20778887/6427607

    if (inet_pton(AF_INET, "127.0.0.1", &server_obj.sin_addr) <= 0)
    {
        printf(BRED "\nInvalid address/ Address not supported \n" ANSI_RESET);
        exit(-1);
    }
    /////////////////////////////////////////////////////////////////////////////////////////
    /* connect to server */

    if (connect(socket_fd, (struct sockaddr *)&server_obj, sizeof(server_obj)) < 0)
    {
        perror("Problem in connecting to the server");
        exit(-1);
    }

    //part;
    printf(BGRN"Connected to server\n" ANSI_RESET);
    // part;
    return socket_fd;
}

int main(int argc, char *argv[])
{
    //  part;
    LL i, j, k, t, temp;
    int socket_fd, port_num, n;

    struct sockaddr_in server_obj;
    const int normal_len = 256;
    char cmd_input[normal_len];
    int cmd_len = 255;
    char *buffer_ptr = (char *)malloc(buff_sz * sizeof(char));
    char *msg_to_server = (char *)malloc(normal_len * sizeof(char));
    char *msg_from_server = (char *)malloc(normal_len * sizeof(char)); /////////////////////////////////////////////////////////////////////////////////////

    /* ask user for input */
    ////////////////////////////////////////////////////////////////////////////////////////

    // part2;
    curr_files_num = 0;
    //initialize command string
    // for (i = 0; i < 255; i++)
    // {
    //     cmd_input[i] = '\0';
    // }

    // printf("Please enter command:\n> ");
    // scanf("%[^\n]s", cmd_input);
    // char ch;
    // scanf("%c", &ch);
    // printf(BYEL "Cmd inputted was : %s with len %ld\n" ANSI_RESET, cmd_input, strlen(cmd_input));

    // //check if command was exit
    // if (strcmp(cmd_input, "exit") == 0)
    // {
    //     //command is exit
    //     exit(-1);
    // }

    //if not, extract filenames
    // int valid_cmd = fill_file_paths(cmd_input);
    // if (valid_cmd == 0)
    // {
    //     printf("Invalid input: Cmd inputted was neither GET nor EXIT\n");
    //     exit(-1);
    // }

    //  loop for filenames

    init_file_paths();

    curr_files_num = argc - 1;
    if (curr_files_num == 0)
    {
        printf(BRED "NO FILES SUPPLIED FOR DOWNLOAD\n" ANSI_RESET);
        exit(0);
    }
    //part;
    for (i = 0; i < curr_files_num; i++)
    {
        strcpy(file_paths[i], argv[i + 1]);
        // printf("%lld filepath is %s\n", i, file_paths[i]);
    }
    //part;
    printf(BMAG "Total number of files requested : %d\n"ANSI_RESET, curr_files_num);
    for (i = 0; i < curr_files_num; i++)
    {
        //send filename
        int k = i;
        //debug(k);
        part;

        int child_pid = fork();
        if (child_pid == 0)
        {

            char *ptr_to_file_name = file_paths[i];
            socket_fd = get_socket_fd(&server_obj);
            strcpy(msg_to_server, ptr_to_file_name);
            // printf("FIle name is %s\n", ptr_to_file_name);
            printf("%s\n", ptr_to_file_name);
            //part 1
            int intended_len = strlen(msg_to_server);
            int ret_val = write(socket_fd, msg_to_server, intended_len);
            if (ret_val == -1)
            {
                perror("Error in write()");
                printf("Error occurred during part 1 WRITE while sending file_name_to_server to server, seems like connection isn't well-established\n");
                exit(-1);
            }

            //receive notification from client if job is possible or not and also receive TOTAL FILE SIZE FOR PROGRESS BAR
            ret_val = read(socket_fd, msg_from_server, normal_len);
            if (ret_val == -1)
            {
                perror("Error in read()");
                printf("Error while reading msg sent from server: Seems like connection isn't well-established\n");
                exit(-1);
            }

            int msg_rcv_len = strlen(msg_from_server);
            msg_from_server[msg_rcv_len] = '\0';

            //if not possible, print reason
            if (msg_from_server[0] == 'N')
            {
                printf(BRED "DENIED : Server responded with NO when asked to let client download the intended file\n" ANSI_RESET);
                exit(-1);
            }

            //   printf(BYEL "Msg received from server is %s\n" ANSI_RESET, msg_from_server);
            // https://linux.die.net/man/3/strtoll
            LL file_sz = strtoll(msg_from_server + 2, NULL, 10);
            printf(BYEL "file size is %lld\n" ANSI_RESET, file_sz);
            //Creating output file
            int fd2 = open(file_paths[i], O_WRONLY | O_TRUNC | O_CREAT, 0644);
            if (fd2 < 0)
            {
                perror("Error generated ");
                printf(BRED "Error occurred while creation/opening of output file in client\n" ANSI_RESET);
                int ret_val = write(socket_fd, "B\0", 2);
                if (ret_val == -1)
                {
                    perror("ERROR-> write() in part 3:");
                    printf("Error occurred during part 3 WRITE to server, surely socket is corrupted\n");
                    exit(-1);
                }
                exit(-1);
            }
            ret_val = write(socket_fd, "A\0", 2);
            //   ret_val=-1;
            if (ret_val == -1)
            {
                perror("ERROR-> write() in part 3:");
                printf("Error occurred during part 3 WRITE to server, surely socket is corrupted\n");
                exit(-1);
            }

            //send success/error msg in that case + length of file for percentage printing

            //start reading drom file and also start transferring it

            double percentage_done = 0.0;

            LL tot_written_already = 0; //done stores how many characters have already been sent

            //send to client
            //printf(BRED "Goinging to enter loop\n" ANSI_RESET);
            while ((n = read(socket_fd, buffer_ptr, buff_sz)) > 0)
            {
                // debug(n);
                int written_in_this_stint = write(fd2, buffer_ptr, n);
                // debug(written_in_this_stint);
                if (written_in_this_stint == -1)
                {
                    perror("Error while writing to file in client directory\n");
                    exit(-1);
                    //decide course of action
                }
                tot_written_already += written_in_this_stint;
                percentage_done = 100 * ((double)(tot_written_already)) / file_sz;
                print_percentage_done(percentage_done);
                if (tot_written_already == file_sz)
                {
                    break;
                }
            }

            if (n == -1)
            {
                perror("Error details:");
                printf("^ Above Error while reading from socket\n");
                exit(-1);
            }
            printf("\n");
            close(fd2);
        close_connection:
            close(socket_fd);
            //   part3;
            exit(0);
        }
        else
        {
            //wait for child to exit
            //print error message
            //  printf(BMAG "pid of forked child is %d\n" ANSI_RESET, child_pid);

            //    pid_t waitpid(pid_t pid, int *stat_loc, int options);{}
            int status;
            {
                int wpid = waitpid(child_pid, &status, 0);
                if (wpid == (-1))
                {
                    perror("waitpid");
                    //exit(EXIT_FAILURE);
                }

                if (WIFEXITED(status))
                {
                    if (WEXITSTATUS(status) != 0)
                    {
                        printf("child exited, status=%d\n", WEXITSTATUS(status));
                    }
                }
            }
        }
        part;
    }
    //part2;

    printf(BRED "JOB DONE\n" ANSI_RESET);
    /////////////////////////////////////////////////////////////////////////////////////////////////////

    return 0;
}