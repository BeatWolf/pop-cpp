/**
 *
 * Copyright (c) 2005-2012 POP-C++ project - GRID & Cloud Computing group, University of Applied Sciences of western Switzerland.
 * http://gridgroup.hefr.ch/popc
 *
 * @author Tuan Anh Nguyen
 * @date 2005/01/01
 * @brief  POP-C++ thread library
 *
 *
 */

#ifndef _POPC_THREAD
#define _POPC_THREAD
#include <pthread.h>

class paroc_thread
{
public:
	paroc_thread(bool joinable);
	virtual ~paroc_thread();

	virtual void start()=0;
	void join();
	void cancel();
	void kill(int sig);
	bool joinable();

	int create();
public:
	static void *_threadentry(void *param);
	static void _cleanupentry(void *param);

private:
	bool isjoinable;
	bool iscreated;

	pthread_attr_t attr;
	pthread_t th;
	static pthread_mutex_t objlock;
};

#endif
