#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include "string"
#include "vector"
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>
#include <time.h>
#include "ThreadPool.h"
#include <sys/sendfile.h>
#include <unistd.h>		/* for syscall() */
#include <sys/syscall.h>	/* for __NR_* definitions */
#include <linux/aio_abi.h>	/* for AIO types and constants */

using namespace std;

#define MAXEVENTS 64

inline int io_setup(unsigned nr, aio_context_t *ctxp)
{
	return syscall(__NR_io_setup, nr, ctxp);
}

inline int io_destroy(aio_context_t ctx)
{
	return syscall(__NR_io_destroy, ctx);
}

inline int io_submit(aio_context_t ctx, long nr,  struct iocb **iocbpp)
{
	return syscall(__NR_io_submit, ctx, nr, iocbpp);
}

inline int io_getevents(aio_context_t ctx, long min_nr, long max_nr,
		struct io_event *events, struct timespec *timeout)
{
	return syscall(__NR_io_getevents, ctx, min_nr, max_nr, events, timeout);
}

string RESP = "HTTP/1.1 200 OK\r\nServer: Test\r\nDate: Sun, 02 Apr 2017 07:08:36 GMT\r\nConnection: close\r\nContent-Length: 11510\r\nContent-Type: text/html\r\n\r\n";

class sock_state
{
public:
	int fd;
	string *d;
};

class thrdt {
public:
	char *p;
	int q;
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

		int z;  /* Status code */
		struct linger so_linger;

		so_linger.l_onoff = 1;
		so_linger.l_linger = 0;
		z = setsockopt(sfd, SOL_SOCKET, SO_LINGER, &so_linger, sizeof so_linger);
		if ( z )
			perror("setsockopt(2)");

		int o=5;setsockopt(sfd,SOL_TCP,TCP_DEFER_ACCEPT,&o,sizeof(o));

		o=1;setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, &o, sizeof(o));

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

bool writeFile(int fdi, int fdo, int remain_data)
{
	off_t offset = 0;
	int sent_bytes = 0;
	/* Sending file data */
	while (((sent_bytes = sendfile(fdo, fdi, &offset, BUFSIZ)) > 0) && (remain_data > 0))
	{
		remain_data -= sent_bytes;
	}
	return true;
}

class Stask : public Task {
public:
	sock_state* st;
	int q;
	void run() {
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

				size_t ind = string::npos;
				while((ind = st->d->find("\r\n\r\n"))!=string::npos) {
					string r = st->d->substr(0, ind);
					/* open the file to be sent */
					int fd = open("index.html", O_RDONLY);
					if (fd == -1) {
					  exit(1);
					}

					struct stat stat_buf;
					/* get the size of the file to be sent */
					fstat(fd, &stat_buf);

					int s = write (st->fd, &RESP[0], RESP.size()+1);
					if (s == -1)
					{
						done = 1;
						perror ("write");
						break;
					}
					writeFile(fd, st->fd, stat_buf.st_size);
					close(fd);
					*(st->d) = st->d->substr(ind+4);
				}
			}
		}

		if (done)
		{
			//printf ("Closed connection on descriptor %d\n", st->fd);

			/* Closing the descriptor will make epoll remove it
			 from the set of descriptors which are monitored. */
			close (st->fd);
			if(st->d!=NULL) {
				delete st->d;
			}
			delete st;
		}
		else
		{
			struct epoll_event ev={0};
			ev.data.ptr=(void*)st;ev.events=EPOLLIN|EPOLLRDHUP|EPOLLERR|EPOLLET;
			epoll_ctl(q,EPOLL_CTL_ADD,st->fd,&ev);
		}
	}
};

class Atask : public Task {
public:
	sock_state* st;
	int q;
	void run() {
		while (1)
		{
			struct sockaddr in_addr;
			socklen_t in_len;
			int infd;
			char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

			in_len = sizeof in_addr;
			infd = accept (st->fd, &in_addr, &in_len);
			if (infd == -1)
			{
				if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
				{
					/* We have processed all incoming
						 connections. */
					break;
				}
			}

			int s = getnameinfo (&in_addr, in_len,
					hbuf, sizeof hbuf,
					sbuf, sizeof sbuf,
					NI_NUMERICHOST | NI_NUMERICSERV);
			if (s == 0)
			{
				printf("Accepted connection on descriptor %d (host=%s, port=%s)\n", infd, hbuf, sbuf);
			}

			/* Make the incoming socket non-blocking and add it to the
			 list of fds to monitor. */
			s = make_socket_non_blocking (infd);
			if (s == -1)
				abort ();

			struct timeval tv={0};
			tv.tv_sec=5;
			setsockopt(infd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(tv));
			fcntl(infd,F_SETFL,O_NONBLOCK);

			int flag = 1;
			setsockopt(infd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));

			sock_state *nst = new sock_state;
			nst->fd = infd;
			nst->d = NULL;

			struct epoll_event ev={0};
			ev.data.ptr=(void*)nst;ev.events=EPOLLIN|EPOLLRDHUP|EPOLLERR|EPOLLET;
			epoll_ctl(q,EPOLL_CTL_ADD,infd,&ev);
		}

		struct epoll_event ev={0};
		ev.data.ptr=(void*)st;ev.events=EPOLLIN|EPOLLRDHUP|EPOLLERR|EPOLLET;
		epoll_ctl(q,EPOLL_CTL_ADD,st->fd,&ev);
	}
};

void poll(int sfd) {
	aio_context_t ctx;
	int ret = io_setup(128, &ctx);
	if (ret < 0) {
		perror("io_setup ");
		abort ();
	}

	struct epoll_event event;
	int efd = epoll_create1 (0);
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

	ThreadPool pool(4);
	Atask at;

	struct epoll_event e[1000];
	while(1)
	{
		int n = epoll_wait (efd, e, 1000, -1);
		if(n==-1) {
			perror("epoll_wait");
		}
		for(int i=0;i<n;++i) {
			sock_state *st = (sock_state*)e[i].data.ptr;
			if((e[i].events & EPOLLERR) || (e[i].events & EPOLLHUP) || (!(e[i].events & EPOLLIN))) {
				printf("Closed connectio\n");
				close(st->fd);
				if(st->d!=NULL) {
					delete st->d;
				}
				delete st;
			} else {
				printf("Service connection\n");
				struct epoll_event e;
				epoll_ctl (efd, EPOLL_CTL_DEL, st->fd, &e);

				Task* t = NULL;
				if(sfd == st->fd) {
					at.q = efd;
					at.st = st;
					t = &at;
				} else {
					Stask* tt = new Stask;
					tt->q = efd;
					tt->st = st;
					tt->setCleanUp(true);
					t = tt;
				}
				pool.submit(t);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	int sfd, s, n = 2048;

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

	struct rlimit r;
	getrlimit(RLIMIT_NOFILE, &r);
	if(r.rlim_cur){r.rlim_cur=n;if(setrlimit(RLIMIT_NOFILE,&r)==-1)perror("setrlimit");}

	pid_t pids[n];
	for (int i = 0; i < 4; ++i) {
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
	}
}
