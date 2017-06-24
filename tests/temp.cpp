#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include "string"
#include <pthread.h>
#include "ThreadPool.h"

using namespace std;

#define MAXEVENTS 64

string RESP = "HTTP/1.1 200 OK\r\nServer: Test\r\nDate: Sun, 02 Apr 2017 07:08:36 GMT\r\nConnection: close\r\nContent-Length: 28\r\nContent-Type: application/json\r\n\r\n{\"message\": \"Hello World!\"}";

class sock_data;

class sock_data {
public:
	char *d;
	int l;
	int t;
	int c;
	sock_data* e;
	sock_data* n;
};

class sock_state
{
public:
	int fd;
	//string *d;
	sock_data *d;
};

class Stask : public Task {
public:
	int fd;
	//string d;
	sock_data *d;
	void run() {
		int s = write (fd, &RESP[0], RESP.size()+1);
		if (s == -1)
		{
			perror ("write");
			abort ();
		}
	}
};

static int make_socket_non_blocking (int sfd)
{
	int flags, s;

	flags = fcntl (sfd, F_GETFL, 0);
	if (flags == -1)
	{
		perror ("fcntl");
		return -1;
	}

	flags |= O_NONBLOCK;
	s = fcntl (sfd, F_SETFL, flags);
	if (s == -1)
	{
		perror ("fcntl");
		return -1;
	}

	return 0;
}

static int create_and_bind (char *port)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s, sfd;

	memset (&hints, 0, sizeof (struct addrinfo));
	hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
	hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
	hints.ai_flags = AI_PASSIVE;     /* All interfaces */

	s = getaddrinfo (NULL, port, &hints, &result);
	if (s != 0)
	{
		fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
		return -1;
	}

	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue;

		s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
		if (s == 0)
		{
			/* We managed to bind successfully! */
			break;
		}

		close (sfd);
	}

	if (rp == NULL)
	{
		fprintf (stderr, "Could not bind\n");
		return -1;
	}

	freeaddrinfo (result);

	return sfd;
}

void* ioserv(void* sti) {
	return NULL;
}

/* this function is run by the second thread */
void* service(void *sti)
{
	sock_state* st = (sock_state*)sti;
	int s = write (st->fd, &RESP[0], RESP.size()+1);
	if (s == -1)
	{
		perror ("write");
		abort ();
	}
	return NULL;
}

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

void poll(int sfd) {
	int efd;
	struct epoll_event event;
	struct epoll_event *events;

	efd = epoll_create1 (0);
	if (efd == -1)
	{
		perror ("epoll_create");
		abort ();
	}

	struct sock_state st;
	st.fd = sfd;
	event.data.ptr = (void*)&st;

	event.events = EPOLLIN | EPOLLET;
	int s = epoll_ctl (efd, EPOLL_CTL_ADD, sfd, &event);
	if (s == -1)
	{
		perror ("epoll_ctl");
		abort ();
	}

	/* Buffer where events are returned */
	events = (epoll_event*)calloc (MAXEVENTS, sizeof event);

	ThreadPool pool(4);

	/* The event loop */
	while (1)
	{
		int n, i;

		n = epoll_wait (efd, events, MAXEVENTS, -1);
		for (i = 0; i < n; i++)
		{
			struct sock_state *st = (sock_state*)events[i].data.ptr;

			if ((events[i].events & EPOLLERR) ||
					(events[i].events & EPOLLHUP) ||
					(!(events[i].events & EPOLLIN)))
			{
				/* An error has occured on this fd, or the socket is not
				 ready for reading (why were we notified then?) */
				fprintf (stderr, "epoll error\n");
				close (st->fd);
				continue;
			}

			else if (sfd == st->fd)
			{
				/* We have a notification on the listening socket, which
				 means one or more incoming connections. */
				while (1)
				{
					struct sockaddr in_addr;
					socklen_t in_len;
					int infd;
					char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

					in_len = sizeof in_addr;
					infd = accept (sfd, &in_addr, &in_len);
					if (infd == -1)
					{
						if ((errno == EAGAIN) ||
								(errno == EWOULDBLOCK))
						{
							/* We have processed all incoming
							 connections. */
							 break;
						}
						else
						{
							perror ("accept");
							break;
						}
					}

					s = getnameinfo (&in_addr, in_len,
							hbuf, sizeof hbuf,
							sbuf, sizeof sbuf,
							NI_NUMERICHOST | NI_NUMERICSERV);
					if (s == 0)
					{
						printf("Accepted connection on descriptor %d "
								"(host=%s, port=%s)\n", infd, hbuf, sbuf);
					}

					/* Make the incoming socket non-blocking and add it to the
					 list of fds to monitor. */
					s = make_socket_non_blocking (infd);
					if (s == -1)
						abort ();

					sock_state *st = new sock_state;
					st->fd = infd;
					st->d = NULL;
					event.data.ptr = (void*)st;

					event.events = EPOLLIN | EPOLLET;
					s = epoll_ctl (efd, EPOLL_CTL_ADD, infd, &event);
					if (s == -1)
					{
						perror ("epoll_ctl");
						abort ();
					}
				}
				continue;
			}
			else
			{
				/* We have data on the fd waiting to be read. Read and
				 display it. We must read whatever data is available
				 completely, as we are running in edge-triggered mode
				 and won't get a notification again for the same
				 data. */
				int done = 0;

				while (1)
				{
					ssize_t count;
					char *buf = (char*)malloc(1024 * sizeof(char));

					count = read (st->fd, buf, 1024);
					if (count == -1)
					{
						/* If errno == EAGAIN, that means we have read all
						 data. So go back to the main loop. */
						if (errno != EAGAIN)
						{
							perror ("read");
							done = 1;
						}
						break;
					}
					else if (count == 0)
					{
						/* End of file. The remote has closed the
						 connection. */
						done = 1;
						break;
					}
					else
					{
						//String based examples
						if(st->d == NULL) {
							st->d = new string;
						}
						st->d->append(buf, count);
						free(buf);

						size_t ind = st->d->find("\r\n\r\n");
						if(ind!=string::npos) {
							string r = st->d->substr(0, ind);

							//In line example
							/*s = write (st->fd, &RESP[0], RESP.size()+1);
							if (s == -1)
							{
								perror ("write");
								abort ();
							}*/

							//threaded example
							/*pthread_t t;
							sock_state* pst = new sock_state;
							pst->fd = st->fd;
							pst->d = new string;
							*(pst->d) = r;
							if( pthread_create( &t, NULL, service, (void*) pst) < 0)
							{
								perror("could not create thread");
							}*/

							Stask* t = new Stask;
							t->fd = st->fd;
							t->d = r;
							t->setCleanUp(true);
							pool.submit(t);




							*(st->d) = st->d->substr(ind+4);
						}
					}
				}

				if (done)
				{
					printf ("Closed connection on descriptor %d\n", st->fd);

					/* Closing the descriptor will make epoll remove it
					 from the set of descriptors which are monitored. */
					close (st->fd);
					if(st->d!=NULL) {
						delete st->d;
					}
					delete st;
				}
			}
		}
	}

	free (events);

	close (sfd);
}

void poll1(int sfd) {
	int efd;
	struct epoll_event event;
	struct epoll_event *events;
}

int main (int argc, char *argv[])
{
	int sfd, s, n = 5;

	if (argc != 2)
	{
		fprintf (stderr, "Usage: %s [port]\n", argv[0]);
		exit (EXIT_FAILURE);
	}

	sfd = create_and_bind (argv[1]);
	if (sfd == -1)
		abort ();

	s = make_socket_non_blocking (sfd);
	if (s == -1)
		abort ();

	s = listen (sfd, SOMAXCONN);
	if (s == -1)
	{
		perror ("listen");
		abort ();
	}

	//Forking example
	/*pid_t pids[n];
	for (i = 0; i < n; ++i) {
		if ((pids[i] = fork()) < 0) {
			perror("fork");
			abort();
		} else if (pids[i] == 0) {
			poll(sfd);
			exit(0);
		}
	}
	int status;
	pid_t pid;
	while (n > 0) {
	  pid = wait(&status);
	  printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);
	  --n;  // TODO(pts): Remove pid from the pids array.
	}*/

	poll(sfd);

	return EXIT_SUCCESS;
}
