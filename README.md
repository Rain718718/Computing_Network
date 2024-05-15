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