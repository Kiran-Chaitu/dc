#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>

#define SERVER_PORT 12345
#define TRUE 1
#define FALSE 0

int main(int argc, char *argv[])
{
    int listen_sd = -1, new_sd = -1;
    int len, rc, on = 1;
    int desc_ready, end_server = FALSE, compress_array = FALSE;
    int close_conn;
    char buffer[80];
    struct sockaddr_in6 addr;
    int timeout;
    struct pollfd fds[200];
    int nfds = 1, current_size = 0, i, j;

    // Create a socket for IPv6
    listen_sd = socket(AF_INET6, SOCK_STREAM, 0);
    if (listen_sd < 0)
    {
        perror("socket() failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to allow reusing the address
    rc = setsockopt(listen_sd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
    if (rc < 0)
    {
        perror("setsockopt() failed");
        close(listen_sd);
        exit(EXIT_FAILURE);
    }

    // Set socket to non-blocking mode
    rc = ioctl(listen_sd, FIONBIO, (char *)&on);
    if (rc < 0)
    {
        perror("ioctl() failed");
        close(listen_sd);
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    memcpy(&addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
    addr.sin6_port = htons(SERVER_PORT);

    // Bind the socket
    rc = bind(listen_sd, (struct sockaddr *)&addr, sizeof(addr));
    if (rc < 0)
    {
        perror("bind() failed");
        close(listen_sd);
        exit(EXIT_FAILURE);
    }

    // Start listening
    rc = listen(listen_sd, 32);
    if (rc < 0)
    {
        perror("listen() failed");
        close(listen_sd);
        exit(EXIT_FAILURE);
    }

    // Initialize the poll structure
    memset(fds, 0, sizeof(fds));
    fds[0].fd = listen_sd;
    fds[0].events = POLLIN;
    timeout = 3 * 60 * 1000; // 3 minutes

    do
    {
        printf("Waiting on poll()...\n");
        rc = poll(fds, nfds, timeout);

        if (rc < 0)
        {
            perror("poll() failed");
            break;
        }
        if (rc == 0)
        {
            printf("poll() timed out. Ending program.\n");
            break;
        }

        current_size = nfds;
        for (i = 0; i < current_size; i++)
        {
            if (fds[i].revents == 0)
                continue;
            if (fds[i].revents != POLLIN)
            {
                printf("Error! revents = %d\n", fds[i].revents);
                end_server = TRUE;
                break;
            }

            if (fds[i].fd == listen_sd)
            {
                printf("Listening socket is readable\n");
                do
                {
                    new_sd = accept(listen_sd, NULL, NULL);
                    if (new_sd < 0)
                    {
                        if (errno != EWOULDBLOCK)
                        {
                            perror("accept() failed");
                            end_server = TRUE;
                        }
                        break;
                    }
                    printf("New incoming connection - %d\n", new_sd);
                    fds[nfds].fd = new_sd;
                    fds[nfds].events = POLLIN;
                    nfds++;
                } while (new_sd != -1);
            }
            else
            {
                printf("Descriptor %d is readable\n", fds[i].fd);
                close_conn = FALSE;
                do
                {
                    rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                    if (rc < 0)
                    {
                        if (errno != EWOULDBLOCK)
                        {
                            perror("recv() failed");
                            close_conn = TRUE;
                        }
                        break;
                    }
                    if (rc == 0)
                    {
                        printf("Connection closed\n");
                        close_conn = TRUE;
                        break;
                    }
                    len = rc;
                    printf("%d bytes received\n", len);
                    rc = send(fds[i].fd, buffer, len, 0);
                    if (rc < 0)
                    {
                        perror("send() failed");
                        close_conn = TRUE;
                        break;
                    }
                } while (TRUE);

                if (close_conn)
                {
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    compress_array = TRUE;
                }
            }
        }

        if (compress_array)
        {
            compress_array = FALSE;
            for (i = 0; i < nfds; i++)
            {
                if (fds[i].fd == -1)
                {
                    for (j = i; j < nfds - 1; j++)
                    {
                        fds[j].fd = fds[j + 1].fd;
                    }
                    nfds--;
                    i--;
                }
            }
        }
    } while (end_server == FALSE);

    for (i = 0; i < nfds; i++)
    {
        if (fds[i].fd >= 0)
            close(fds[i].fd);
    }
    return 0;
}
