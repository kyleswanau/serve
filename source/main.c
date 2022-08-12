#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define DIE(s) do { fprintf(stderr, "ERROR - %s\n", s); exit(1); } while (0)

int main(int argc, char **argv) {
    // check user input
    if (argc < 2) {
        DIE("Usage: ./serve <port> [<log-file>]");
    }

    // set up host socket to accept TCP connections
    int port;
    if ((port = atoi(argv[1])) == 0) {
        DIE("Invalid port number given");
    }

    int sock;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        DIE("Creation of socket file descriptor failed");
    }

    struct sockaddr_in addr = { AF_INET, htons(port), INADDR_ANY };
    if ((bind(sock, (struct sockaddr *) &addr, sizeof(addr))) == -1) {
        DIE("Bind on socket failed");
    }

    if (listen(sock, 1) == -1) {
        DIE("Listen on socket failed");
    }

    // HTTP response headers
    char *http200 = "HTTP/1.0 200 OK \r\n\r\n";
    char *http400 = "HTTP/1.0 400 Bad Request\r\n\r\n";
    char *http404 = "HTTP/1.0 404 Not Found\r\n\r\n";
    char *http501 = "HTTP/1.0 501 Not Implemented\r\n\r\n";

    // main loop
    while (1) {
        char req[1024] = { 0 };

        int client, bytes;
        client = accept(sock, 0, 0);
        bytes = read(client, req, 1024);

        char *method = strtok(req, " ");
        char *resource = strtok(NULL, " ");
        char *version = strtok(NULL, "\r");

        if (!method || !resource || !version) {
            // HTTP 400 - Bad Request
            write(client, http400, strlen(http400));

        } else if (strncmp(resource, "/", 1) != 0) {
            // HTTP 404 - Not Found
            write(client, http404, strlen(http404));

        } else if (strncmp(method, "GET", 3) != 0) {
            // HTTP 501 - Not Implemented
            write(client, http501, strlen(http501));

        } else {
            // HTTP 200 - OK (send index.html)
            char res[1024] = { 0 };

            FILE *fp = fopen("index.html", "r");
            int n = fread(res, 1, 1024, fp);
            fclose(fp);

            write(client, http200, strlen(http200));
            write(client, res, n);
        }

        close(client);
    }

    return 0;
}
