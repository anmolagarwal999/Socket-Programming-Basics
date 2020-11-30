# Socket Programming Basics

Simple networking model of client and server over TCP connection
Client can requests file from server which are delivered if they exist on the server

### Instructions to compile and run
```
$ gcc -c server_prog.c -o server
$ ./server
$ gcc -c client_prog.c -o client
$ ./client

```

### Implementation details
* the name of the input file is maximum 256 characters long
* max number of files requested in one instance is 50
* I assume file-paths do not have a null character in between (I know this is highly improbable but just to be sure)
* I assume that in case of a 'path to a file', the path points to a 'file' and not to a directory.
* In case the file name contains **spaces**, they will be escaped using either quotes or a backslash.
* The program has enough allotted memory for **1 char array of size 1048576** and other variables used in the program.
* To deal with errors better, I have forked a child for each file to be downloaded in the client process. 
* My intitial implementation was to take inputs via 'get' and 'exit' in client program, but had to be changed  to incorporate CLI based input.

Please refer to [this pdf](./problem_statement.pdf) for more details