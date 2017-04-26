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
#include "Thread.h"

using namespace std;

#define MAXEVENTS 64

string RESP = "HTTP/1.1 200 OK\r\nServer: Test\r\nDate: Sun, 02 Apr 2017 07:08:36 GMT\r\nConnection: close\r\nContent-Length: 28\r\nContent-Type: application/json\r\n\r\n{\"message\": \"Hello World!\"}";

class sock_state
{
public:
	int fd;
	string *d;
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

class worker {
public:
	int q;
};

pthread_cond_t tc;
pthread_mutex_t tm;

void set_affinity(int id) {
	cpu_set_t c;CPU_ZERO(&c);CPU_SET(id,&c);pthread_setaffinity_np(pthread_self(),sizeof(c),&c);
}

void handle(struct sock_state *st) {
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
				int s = write (st->fd, &RESP[0], RESP.size()+1);
				if (s == -1)
				{
					perror ("write");
					abort ();
				}
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
}

vector<worker> workers;
static int t = 2;

void* run(void* it) {
	int* id = (int*)it;

	printf ("Started thread %d\n", *id);

	set_affinity(*id);
	pthread_mutex_lock(&tm);

	int efd;

	efd = epoll_create1 (0);
	if (efd == -1)
	{
		perror ("epoll_create");
		abort ();
	}

	workers[*id].q = efd;

	pthread_mutex_unlock(&tm);
	pthread_cond_signal(&tc);

	long long wt = 0, pt = 0;

	timespec crti;
	clock_gettime(CLOCK_MONOTONIC, &crti);

	struct epoll_event e[1000];
	while(1)
	{
		timespec crte;
		clock_gettime(CLOCK_MONOTONIC, &crte);
		long df = (crte.tv_sec - crti.tv_sec);

		if(df>10) {
			if(wt>0) {
				printf("Epoll wait time = %lld\n", wt);
			}
			if(pt>0) {
				printf("Process time = %lld\n", pt);
			}
		}

		timespec sti;
		clock_gettime(CLOCK_MONOTONIC, &sti);
		int n = epoll_wait (efd,e,1000,10);
		timespec end;
		clock_gettime(CLOCK_MONOTONIC, &end);

		wt += (((end.tv_sec - sti.tv_sec) * 1E9) + (end.tv_nsec - sti.tv_nsec))/1E6;

		timespec sti1;
		clock_gettime(CLOCK_MONOTONIC, &sti1);
		if(n==-1) {
			perror("epoll_wait");
		}
		for(int i=0;i<n;++i) {
			struct sock_state *st = (sock_state*)e[i].data.ptr;
			if((e[i].events & EPOLLERR) || (e[i].events & EPOLLHUP) || (!(e[i].events & EPOLLIN))) {
				close(st->fd);
				if(st->d!=NULL) {
					delete st->d;
				}
				delete st;
			} else {
				handle(st);
			}
		}
		timespec end1;
		clock_gettime(CLOCK_MONOTONIC, &end1);
		pt += (((end1.tv_sec - sti1.tv_sec) * 1E9) + (end1.tv_nsec - sti1.tv_nsec))/1E6;
	}
}

int pick(void) {
	static int c;
	++c;
	return workers[c%t].q;
}

void poll() {
	pthread_t id;
	pthread_attr_t a;
	pthread_attr_init(&a);
	//pthread_attr_setscope(&a,PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setdetachstate(&a,PTHREAD_CREATE_DETACHED);
	//t = sysconf(_SC_NPROCESSORS_ONLN);
	for(int i=0;i<t;++i){pthread_create(&id, &a, (void*)run, (void*)new int(i));workers.push_back(worker());}
	int tt = t;
	while(tt>0){pthread_mutex_lock(&tm);pthread_cond_wait(&tc,&tm);pthread_mutex_unlock(&tm);--tt;}
}

int mainold (int argc, char *argv[])
{
	int sfd, s, n = 2048;

	if (argc != 2)
	{
		fprintf (stderr, "Usage: %s [port]\n", argv[0]);
		exit (EXIT_FAILURE);
	}

	struct rlimit r;
	getrlimit(RLIMIT_NOFILE, &r);
	if(r.rlim_cur){r.rlim_cur=n;if(setrlimit(RLIMIT_NOFILE,&r)==-1)perror("setrlimit");}

	sfd = create_and_bind (argv[1]);
	if (sfd == -1)
		abort ();

	s = listen (sfd, SOMAXCONN);
	if (s == -1)
	{
		perror ("listen");
		abort ();
	}

	pthread_mutex_init(&tm, NULL);
	pthread_cond_init(&tc, NULL);

	poll();

	for(;;) {
		struct sockaddr in_addr;
		socklen_t in_len;
		int infd;
		char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

		in_len = sizeof in_addr;
		infd = accept (sfd, &in_addr, &in_len);
		if (infd == -1)
			perror ("accept");
		int s = getnameinfo (&in_addr, in_len,
				hbuf, sizeof hbuf,
				sbuf, sizeof sbuf,
				NI_NUMERICHOST | NI_NUMERICSERV);
		if (s == 0)
		{
			//printf("Accepted connection on descriptor %d (host=%s, port=%s)\n", infd, hbuf, sbuf);
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

		sock_state *st = new sock_state;
		st->fd = infd;
		st->d = NULL;

		int q = pick();
		struct epoll_event ev={0};
		ev.data.ptr=(void*)st;ev.events=EPOLLIN|EPOLLRDHUP|EPOLLERR|EPOLLET;
		epoll_ctl(q,EPOLL_CTL_ADD,infd,&ev);
	}

	return EXIT_SUCCESS;
}

ThreadPool pool(4);

void* pT(void* it) {
	int* id = (int*)it;

	printf ("Started thread %d\n", *id);

	set_affinity(*id);
	pthread_mutex_lock(&tm);

	int efd;

	efd = epoll_create1 (0);
	if (efd == -1)
	{
		perror ("epoll_create");
		abort ();
	}

	workers[*id].q = efd;

	pthread_mutex_unlock(&tm);
	pthread_cond_signal(&tc);

	long long wt = 0, pt = 0;

	timespec crti;
	clock_gettime(CLOCK_MONOTONIC, &crti);

	struct epoll_event e[1000];
	while(1)
	{
		timespec crte;
		clock_gettime(CLOCK_MONOTONIC, &crte);
		long df = (crte.tv_sec - crti.tv_sec);

		if(df>10) {
			if(wt>0) {
				printf("Epoll wait time = %lld\n", wt);
			}
			if(pt>0) {
				printf("Process time = %lld\n", pt);
			}
		}

		timespec sti;
		clock_gettime(CLOCK_MONOTONIC, &sti);
		int n = epoll_wait (efd,e,1000,10);
		timespec end;
		clock_gettime(CLOCK_MONOTONIC, &end);

		wt += (((end.tv_sec - sti.tv_sec) * 1E9) + (end.tv_nsec - sti.tv_nsec))/1E6;

		timespec sti1;
		clock_gettime(CLOCK_MONOTONIC, &sti1);
		if(n==-1) {
			perror("epoll_wait");
		}
		for(int i=0;i<n;++i) {
			struct sock_state *st = (sock_state*)e[i].data.ptr;
			if((e[i].events & EPOLLERR) || (e[i].events & EPOLLHUP) || (!(e[i].events & EPOLLIN))) {
				close(st->fd);
				if(st->d!=NULL) {
					delete st->d;
				}
				delete st;
			} else {
				handle(st);
			}
		}
		timespec end1;
		clock_gettime(CLOCK_MONOTONIC, &end1);
		pt += (((end1.tv_sec - sti1.tv_sec) * 1E9) + (end1.tv_nsec - sti1.tv_nsec))/1E6;
	}
}

void* aT(void* prtc) {
	char *port = (char*)prtc;
	int sfd, s, n = 2048;

	sfd = create_and_bind (argv[1]);
	if (sfd == -1)
		abort ();

	s = listen (sfd, SOMAXCONN);
	if (s == -1)
	{
		perror ("listen");
		abort ();
	}

	ThreadPool pool(4);

	for(;;) {
		struct sockaddr in_addr;
		socklen_t in_len;
		int infd;
		char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

		in_len = sizeof in_addr;
		infd = accept (sfd, &in_addr, &in_len);
		if (infd == -1)
			perror ("accept");
		int s = getnameinfo (&in_addr, in_len,
				hbuf, sizeof hbuf,
				sbuf, sizeof sbuf,
				NI_NUMERICHOST | NI_NUMERICSERV);
		if (s == 0)
		{
			//printf("Accepted connection on descriptor %d (host=%s, port=%s)\n", infd, hbuf, sbuf);
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

		sock_state *st = new sock_state;
		st->fd = infd;
		st->d = NULL;

		int q = pick();
		struct epoll_event ev={0};
		ev.data.ptr=(void*)st;ev.events=EPOLLIN|EPOLLRDHUP|EPOLLERR|EPOLLET;
		epoll_ctl(q,EPOLL_CTL_ADD,infd,&ev);
	}

	return NULL;
}

void poll1() {
	pthread_t id;
	pthread_attr_t a;
	pthread_attr_init(&a);
	//pthread_attr_setscope(&a,PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setdetachstate(&a,PTHREAD_CREATE_DETACHED);
	//t = sysconf(_SC_NPROCESSORS_ONLN);
	for(int i=0;i<t;++i){pthread_create(&id, &a, (void*)pT, (void*)new int(i));workers.push_back(worker());}
	int tt = t;
	while(tt>0){pthread_mutex_lock(&tm);pthread_cond_wait(&tc,&tm);pthread_mutex_unlock(&tm);--tt;}
}

int main(int argc, char *argv[])
{
	int sfd, s, n = 2048;

	if (argc != 2)
	{
		fprintf (stderr, "Usage: %s [port]\n", argv[0]);
		exit (EXIT_FAILURE);
	}

	struct rlimit r;
	getrlimit(RLIMIT_NOFILE, &r);
	if(r.rlim_cur){r.rlim_cur=n;if(setrlimit(RLIMIT_NOFILE,&r)==-1)perror("setrlimit");}

	Thread at1(&aT, argv[1]);
	at1.execute();

	Thread at2(&aT, argv[1]);
	at2.execute();

	poll1();
}
