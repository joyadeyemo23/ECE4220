#ifndef MODULE
#define MODULE
#endif
#ifndef __KERNEL__
#define __KERNEL__
#endif
   
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <stdint.h>
#include <sys/timerfd.h>
#include <time.h>           
#include <semaphore.h>
#include <wiringPi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <softTone.h> 


void *song_play(void* tid);
void *stopSong(void* tid);
void *next_song(void* tid);

// Semaphore -- This variable should be a global variable so that each thread
// can access it.
sem_t mySem;    
                
//General Purpose Event Detection Register
unsigned long *GPEDS;

struct sched_param param;

int main(void)
{
    pthread_t threads[3];     
    sem_init(&mySem,0,1); 
    
    
    wiringPiSetup(); //number scheme that provides mapping
    wiringPiSetupGpio(); //can use Broadcom GPIO pin numbers directly with no re-mapping
    
    softToneCreate(6); 
	softToneWrite(6,0);
    //set up pins 
    //pinMode(6, OUTPUT);     //speaker connected to 6 on raspberryPi GPIO6
    pinMode(16, INPUT);     //1st button play
    pinMode(17, INPUT);     //2nd button stop 
    pinMode(18, INPUT);     //3rd button next

	//paramter to ground
    pullUpDnControl(16, PUD_DOWN); 
    pullUpDnControl(17, PUD_DOWN);  
    pullUpDnControl(18, PUD_DOWN);  

    
     // Get access to GPEDS register which is going to tell the status whether
    // button is pressed or not. If the button is pressed then it's value will
    // be 1.
	int fd = open("/dev/mem", O_RDWR | O_SYNC);
	unsigned long *ptr = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x3F200000);
	GPEDS = ptr + 0x10;

    //create pthreads
    pthread_create(&threads[0], NULL, song_play, NULL);
    pthread_create(&threads[1], NULL, stopSong, NULL);
    pthread_create(&threads[2], NULL, next_song, NULL);

    //join pthreads 
    pthread_join(threads[0], NULL); 
    pthread_join(threads[1], NULL);
    pthread_join(threads[2], NULL);

    close(fd);
    pthread_exit(NULL);
    
    return 0;
}

void *song_play(void* threadid)
{
    long tid;
    tid = (long)threadid;
    param.sched_priority = 52;   

	// Set the parameters for the scheduler
    if(sched_setscheduler(0, SCHED_FIFO, &param) != 0)
    {
        printf("There has been a potnetial schedule error\n");
        exit(0);
    }

    while(1)
    {
		if(*GPEDS == 0x10000)
		{ 
			sem_wait(&mySem);
			softToneWrite(6, 300);
			delay(1000); 
			
			*GPEDS = 0x10000;
			sem_post(&mySem);    
			usleep(10000);
        }
    
    }
    pthread_exit(NULL);
}

void *stopSong(void* threadid)
{
    long tid;
    tid = (long)threadid;
    param.sched_priority = 52;   

    // Set the parameters for the scheduler
    if(sched_setscheduler(0, SCHED_FIFO, &param) != 0){
        printf("Schedule error\n");
        exit(0);
    }
    while(1)
    {
		if(*GPEDS == 0x20000)
		{ 
			sem_wait(&mySem);
			softToneWrite(6, 0);
			delay(1000); 
			*GPEDS = 0x20000;
			sem_post(&mySem);      
			usleep(10000);
        }
    
    }
    
    pthread_exit(NULL);

}

void *next_song(void *threadid)
{
    long tid;
    tid = (long)threadid;
    param.sched_priority = 52;   

    // Set the parameters for the scheduler
    if(sched_setscheduler(0, SCHED_FIFO, &param) != 0){
        printf("Schedule error\n");
        exit(0);
    }
	while(1)
	{
    if(*GPEDS == 0x40000){ 
			int f = f + 100; 
			sem_wait(&mySem);
			softToneWrite(6, f);
			
			delay(1000); 
			*GPEDS = 0x40000;
			
			sem_post(&mySem);      
			usleep(10000);
        }
    
    }
    
    pthread_exit(NULL);

}
