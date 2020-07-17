#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

#include "skynet_thread.h"
#include "skynet_malloc.h"

struct skynet_thread {
	pthread_t tid;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	thread_cb cb;
	void *arg;
	int idle;
	int destroy;
};

struct skynet_thread_pool {
	struct skynet_thread *pool;
	int size;
};

static struct skynet_thread_pool TP;

int
skynet_thread_pool_create(int size) {
	TP.pool = skynet_malloc(size * sizeof(struct skynet_thread));
	TP.size = size;
	return 0;
}

int
skynet_thread_pool_size() {
	return TP.size;
}

int
skynet_thread_start(int id, thread_cb cb, void *arg) {
	struct skynet_thread *t = TP.pool + id;
	t->cb = cb;
	t->arg = arg;
	t->idle = 0;
	t->destroy = 0;
	if (pthread_mutex_init(&t->mutex, NULL)) {
		fprintf(stderr, "init mutex error.\n");
		exit(1);
	}
	if (pthread_cond_init(&t->cond, NULL)) {
		fprintf(stderr, "init cond error.\n");
		exit(1);
	}
	if (pthread_create(&t->tid, NULL, t->cb, t->arg)) {
		fprintf(stderr, "create pthread error.\n");
		exit(1);
	}
	return 0;
}

int
skynet_thread_wait(int id) {
	int ret;
	struct skynet_thread *t = TP.pool + id;

	assert(t->destroy == 0);

	if (pthread_mutex_lock(&t->mutex)) {
		fprintf(stderr, "lock mutex error.\n");
		return -1;
	}
	t->idle = 1;
	ret = pthread_cond_wait(&t->cond, &t->mutex);
	if (ret != 0 && ret != ETIMEDOUT) {
		fprintf(stderr, "condition wait error.\n");
		return -1;
	}
	t->idle = 0;
	if (pthread_mutex_unlock(&t->mutex)) {
		fprintf(stderr, "unlock mutex error.\n");
		return -1;
	}
	return 0;
}

int
skynet_thread_signal(int id) {
	struct skynet_thread *t = TP.pool + id;

	assert(t->destroy == 0);

	pthread_cond_signal(&t->cond);
	return 0;
}

int
skynet_thread_join(int id) {
	struct skynet_thread *t = TP.pool + id;

	assert(t->destroy == 0);

	pthread_join(t->tid, NULL);
	return 0;
}

static int
skynet_thread_destroy(int id) {
	struct skynet_thread *t = TP.pool + id;

	assert(t->destroy == 0);

	pthread_mutex_destroy(&t->mutex);
	pthread_cond_destroy(&t->cond);
	t->destroy = 1;
	return 0;
}

int
skynet_thread_pool_destroy() {
	int i;
	for (i = 0; i < TP.size; ++i) {
		skynet_thread_destroy(i);
	}
	return 0;
}

int
skynet_thread_idle(int id) {
	struct skynet_thread *t = TP.pool + id;

	return t->idle == 1;
}
