#ifndef SKYNET_THEAD_H
#define SKYNET_THEAD_H


typedef void* (*thread_cb)(void *);

int skynet_thread_pool_create(int size);
int skynet_thread_pool_destroy();
int skynet_thread_pool_size();
int skynet_thread_start(int id, thread_cb cb, void *arg);
int skynet_thread_wait(int id);
int skynet_thread_signal(int id);
int skynet_thread_join(int id);
int skynet_thread_idle(int id);

#endif
