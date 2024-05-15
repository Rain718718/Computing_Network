# Week 1

This repository contains the starter code for ***CMU 15-441/641 Networking and the Internet Project 1: A Web Server Called Liso***.

## 1. Files
- `DockerFile`: Script to build the docker image for the project's environment.
- `Makefile`: Contains rules for `make`.
- `README.md`: Current document.
- `cp1`: CP1 scripts and examples.
- `cp2`: CP2 scripts and examples.
- `cp3`: CP3 scripts and examples.
- `src/`: Source code for the project.
    - `src/echo_client.c`: Simple echo network client.
    - `src/echo_server.c`: Simple echo network server
    - `src/example.c`: Example driver for parsing.
    - `src/lexer.l`: Lex/Yacc related logic.
    - `src/parser.y`
    - `src/parse.c`
- `include/parse.h`

## 2. Environment Setup
1. Install docker: https://www.docker.com
2. Open a terminal and navigate to the directory containing this `README.md` file.
3. Build the docker image: `docker build -t 15-441/641-project-1:latest -f ./DockerFile .`
4. Run the docker container: ``docker run -it -v `pwd`:/home/project-1/ --name <name for your container> 15-441/641-project-1 /bin/bash``
5. The starter code for the project is available at `/home/project-1/` in the container and `.` on your local machine. To make development easier, a mapping is established between these two folders. Modiying the code in one location will also effect the other one. This means that you can use an IDE to write code on your local machine and then seamlessly test it in the container.
6. To test your server using a web browser, you need to configure port mapping for the docker container. Simply add the argument `-p 8888:15441` to the `docker run` command to establish a mapping from `127.0.0.1:15441` in the container to `127.0.0.1:8888` on your local machine. Then you can test your server by using a web browser (e.g., Chrome) on your local machine to navigate to the URL `127.0.0.1:8888`.

# Week 1

### echo_server.c
Modify the number of input parameters.[45]
'''
if (argc < 3)
    {
        fprintf(stderr, "usage: %s <server-ip> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }
'''
Read the file specified by the fourth parameter and return the request.[80]
'''
if (argc == 3)
{
    fgets(msg, BUF_SIZE, stdin);
}
else
{
    Request *request = readfile(argv[3], msg);
}
'''

### echo_client.c
Classify and process the received messages.[88]
'''
Request *request = parse(buf, BUF_SIZE, client_sock);
if (request == NULL)
{
    strcpy(buf, "HTTP/1.1 400 Bad request\r\n\r\n");
}
else if (strcmp(request->http_method, "GET") && strcmp(request->http_method, "HEAD") && strcmp(request->http_method,"POST"))
{
    strcpy(buf, "HTTP/1.1 501 Not Implemented\r\n\r\n");
}
'''

### parser.y
Allocate memory for the head.Recursively process the request header.[207]
'''
request_header: token ows t_colon ows text ows t_crlf {
	YPRINTF("request_Header:\n%s\n%s\n",$1,$5);
    strcpy(parsing_request->headers[parsing_request->header_count].header_name, $1);
	strcpy(parsing_request->headers[parsing_request->header_count].header_value, $5);
	parsing_request->header_count++;
	parsing_request->headers = (Request_header *) realloc(parsing_request->headers, (1+parsing_request->header_count) * sizeof(Request_header));
};

request_header: request_header request_header ;
'''

### Makefile
Reconnect echo_server and echo_client to other files.
'''
echo_server: $(SVROBJ)
	$(CC) -Werror $^ -o $@

echo_client: $(CLTOBJ)
	$(CC) -Werror $^ -o $@
'''