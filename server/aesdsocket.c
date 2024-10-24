#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>

#define PORT "9000"
#define BUFFER_SIZE 1024
#define DATA_FILE "/var/tmp/aesdsocketdata"

volatile sig_atomic_t is_running = 1;


void transactions(int client_fd);

void signal_handler(int signal)
{
    // Handle termination signals (SIGINT, SIGTERM)
    printf("Received signal %i. Stopping aesdsocket.\n", signal);
    syslog(LOG_USER, "Received signal %i. Stopping aesdsocket.\n", signal);
    is_running = 0;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "-d") == 0)
    {
        // Fork a new process if "-d" is passed
        pid_t child_pid = fork();
        if (child_pid < 0)
        {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
        if (child_pid > 0)
        {
            exit(EXIT_SUCCESS);
        }
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler; // set the signal handler function
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1 || sigaction(SIGTERM, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    int sockfd;//, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	int yes=1;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, 10) == -1) {
		perror("listen");
		exit(1);
	}
    
    syslog(LOG_USER, "Server is waiting for connections...\n");

    while (is_running)
    {
        struct sockaddr_storage client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

        if (client_fd < 0)
        {
            syslog(LOG_ERR, "Accepting connection failed.\n");
            continue;
        }

        char address_buffer[INET6_ADDRSTRLEN];
        inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr), address_buffer, sizeof(address_buffer));
        syslog(LOG_USER, "Accepted connection from %s\n", address_buffer);

        transactions(client_fd);
        syslog(LOG_USER, "Closed connection from %s\n", address_buffer);
    }

    remove(DATA_FILE);
    close(sockfd);
    return 0;
}


void transactions(int client_fd)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    FILE *fd = fopen(DATA_FILE, "a+b");

    if (fd == NULL)
    {
        perror("File open failed");
        return;
    }

    while ((bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0)) > 0)
    {
        fwrite(buffer, 1, bytes_received, fd);
        fseek(fd, 0, SEEK_SET);

        for (ssize_t i = 0; i < bytes_received; i++)
        {
            if (buffer[i] == '\n')
            {
                // Read the fd and send its content
                char character;
                while ((character = fgetc(fd)) != EOF)
                {
                    send(client_fd, &character, 1, 0);
                }
            }
        }
    }

    fclose(fd);
    close(client_fd);
}