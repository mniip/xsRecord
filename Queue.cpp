#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <list>

#include <X11/Xutil.h>

#include "PPM.h"

namespace Queue 
{
	pthread_t Thread;
	pthread_mutex_t Mutex;
	int running;

	std::list<XImage *> Queue;
	std::string Pattern;

	void *Routine(void *unused)
	{
		clock_t lastwrite = 1000;
		int frame = 0;
		while(1)
		{
			pthread_mutex_lock(&Mutex);
			if(Queue.size())
			{
				XImage *image = Queue.front();
				Queue.pop_front();
				pthread_mutex_unlock(&Mutex);
				char filename[512];
				sprintf(filename, Pattern.c_str(), frame++);
				clock_t timer = clock();
				PPM::WriteImage(filename, image);
				lastwrite = clock() - timer;
			}
			else
			{
				if(!running)
					break;
				pthread_mutex_unlock(&Mutex);
			}
			pthread_mutex_lock(&Mutex);
			if(running)
			{
				pthread_mutex_unlock(&Mutex);
				usleep(lastwrite);
			}
			else
				pthread_mutex_unlock(&Mutex);
		}
		pthread_mutex_unlock(&Mutex);
	}

	void Start(std::string pattern)
	{
		char *realname = realpath(pattern.c_str(), NULL);
		Pattern = std::string(realname) + "/screenshot%05d.ppm";
		free(realname);
		pthread_mutex_init(&Mutex, NULL);
		running = 1;
		pthread_create(&Thread, NULL, Routine, NULL);
	}

	void Stop()
	{
		pthread_mutex_lock(&Mutex);
		running = 0;
		pthread_mutex_unlock(&Mutex);
		pthread_join(Thread, NULL);
		pthread_mutex_destroy(&Mutex);
	}

	void Push(XImage *image)
	{
		pthread_mutex_lock(&Mutex);
		Queue.push_back(image);
		pthread_mutex_unlock(&Mutex);
	}
}
