#ifndef SKYNET_IMP_H
#define SKYNET_IMP_H

struct skynet_config {
	int thread;
	int harbor;
	int profile;
	const char * daemon;
	const char * module_path;
	const char * bootstrap;
	const char * logger;
	const char * logservice;
};

#define THREAD_WORKER 0
#define THREAD_MAIN 1
#define THREAD_SOCKET 2
#define THREAD_TIMER 3
#define THREAD_MONITOR 4

#define MAIN_THREAD_ID 0
#define MONITOR_THREAD_ID 0
#define TIMER_THREAD_ID 1
#define SOCKET_THREAD_ID 2
#define WORKER_THREAD_ID_OFFSET 3

void skynet_start(struct skynet_config * config);

#endif
