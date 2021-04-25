//Ebrahim Shahid
//Operating Systems
//Project 3 - Producer Consumer problem

#include <iostream>
#include <cstring>
#include <string>
#include <cstdlib>
#include "buffer.h"
#include "stats.h"
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
using namespace std;

//Structs
struct ARGS //struct to hold args from command line
{
   int simulationLength; //simulation length
   int T_maxSleepTime; //maximum sleep time for thread
   int num_of_Producers; //number of producers
   int num_of_Consumers; //number of consumers
   bool outputBufferSnapshots; //true/false output buffer snapeshots
};

//Global args
ARGS global_args; //create an instance of arguments struct
bool runSimulation = true; //flag to indicate that the simluation should stop when false

//function prototype
void arguments(int, char **);
bool checkPrime(buffer_item);
void *producer(void * param);
void *consumer(void * param);

int main( int argc, char *argv[] )
{
    cout << "Please enter 4 numbers followed by yes or no as compiler arguments\n";
    cout << "The format of your input will be like (10 5 5 5 yes)\n";
   //Get command line arguments
   arguments(argc, argv);

   //Store simulation statistics
   Stats simulationStats(global_args.simulationLength, global_args.T_maxSleepTime,
           global_args.num_of_Producers, global_args.num_of_Consumers,
           BUFFER_SIZE);

   //Initialize the buffer
   buffer_initialize();

   //========= Threads =========
          pthread_t tidP[global_args.num_of_Producers]; //producer thread identifier array
          pthread_t tidC[global_args.num_of_Consumers]; //consumer thread identifier array
          pthread_attr_t attr; //set of thread attributes
   pthread_attr_init( &attr ); //get the default attributes

   cout << "Starting Threads...\n";

   //Create producer threads
   for(int i=0; i < global_args.num_of_Producers; i++)
   {
       pthread_create( &tidP[i], &attr, producer, NULL);
   }

   //Create consumer threads
   for(int i=0; i < global_args.num_of_Consumers; i++)
   {
       pthread_create( &tidC[i], &attr, consumer, NULL);
   }

   //Sleep
   sleep(global_args.simulationLength);

   //Signal Threads to quit
   runSimulation = false;

   //Used to accept array from threads
   int *tempStats;

   //Join Threads
   for(int i=0; i < global_args.num_of_Producers; i++)
   {
       //Store stats for each thread
       tempStats = (int*) pthread_join( tidP[i], NULL );

       simulationStats.totalThreadNumProduced[i] = tempStats[0];
       simulationStats.numTimesBufferFull += tempStats[1];

   }

   for(int i=0; i < global_args.num_of_Consumers; i++)
   {
       //Store stats for each thread
       tempStats = (int*) pthread_join( tidC[i], NULL );

       simulationStats.totalThreadNumConsumed[i] = tempStats[0];
       simulationStats.numTimesBufferEmpty += tempStats[1];
   }

   //Grab # of items remaining in buffer
   simulationStats.numItemsRemaining = bufferCount;

   //Display Statistics
   simulationStats.print();

   //Exit
   return 0;
}

void arguments(int argc, char *argv[])
{
   if(argc < 6)
   {
       cerr << "5 arguments are needed. The arguments should be ";
       cerr << "in the following order:\n simulation length\n ";
       cerr << "max thread sleep\n number of producers\n number ";
       cerr << "of consumers\n yes or no for outputting buffer snapshots\n";
       exit(-1);
   }

   global_args.simulationLength = atoi(argv[1]);
   global_args.T_maxSleepTime = atoi(argv[2]);
   global_args.num_of_Producers = atoi(argv[3]);
   global_args.num_of_Consumers = atoi(argv[4]);

   if(strcmp(argv[5],"yes") == 0)
       global_args.outputBufferSnapshots = 1;
   else if(strcmp(argv[5],"no") == 0)
       global_args.outputBufferSnapshots = 0;
   else
   {
       cerr << "The last argument must be 'yes' or 'no'\n";
       exit(-1);
   }


}

bool checkPrime(buffer_item in)
{
   int lim = in/2;
   int result;

   for(int i=2; i<=lim; i++)
   {
       result = in % i;
       if(result == 0)
           return false;
   }

   return true;

}

void *producer(void * param)
{
   //Variables
   int * stats = new int[2]; //used to return stats
   int time; //holds random number for time to sleep
   stats[0] = 0; //holds # of items produced
   stats[1] = 0; //holds # of times buffer is full
   buffer_item bItem; // this is my item

   while(runSimulation)
   {
       //Sleep for a random amount of time.
       time = rand();
       time = time % global_args.T_maxSleepTime;

       sleep(time);

       //Generate a random number
       bItem = random();
       bItem %= 9999;
       
       //Check if buffer is full
       if(bufferCount == BUFFER_SIZE)
       {
           cout << "All buffers are full. Producer " << pthread_self();
           cout << " waits.\n\n";
           stats[1]=(stats[1] + 1);
       }

       //Wait
       sem_wait( &bufferEmpty );

       //Lock
       pthread_mutex_lock( &mutex );

       //Produce - CRITICAL SECTION
       if(runSimulation && buffer_insert_item(bItem))
       {
           cout << "Producer " << pthread_self() << " writes ";
           cout << bItem << endl;

           stats[0]= (stats[0] + 1); //add # of items produced

           if(global_args.outputBufferSnapshots) //if snapshots enabled
               buffer_print(); //print snapshots
       }

       //unlock
       pthread_mutex_unlock( &mutex );

       //signal
       sem_post( &bufferEmpty );

   }

   pthread_exit( (void*)stats ); //return stats array
}

void *consumer(void * param)
{
   //Variable definitions
   buffer_item bItem; //bItem to store received item
   int time; //int to hold random time to sleep
   int * stats = new int[2];
   stats[0] = 0; //array to hold # of items consumed
   stats[1] =0; //array to hold # of times buffer was empty

   while(runSimulation)
   {
       //Sleep for a random amount of time
       time = rand();
       time = time % global_args.T_maxSleepTime;
       sleep(time);

       //Check if buffers are empty
       if(bufferCount == 0)
       {
           cout << "All buffers are empty. Consumer ";
           cout << pthread_self();
           cout << " waits.\n\n";

           stats[1]= (stats[1] + 1); //time + 1 if empty buffer
       }

       //Check semaphore
       sem_wait( &bufferFull );

       //Lock
       pthread_mutex_lock( &mutex );

       //Consume
       if(runSimulation && buffer_remove_item( &bItem)) //consumes in if statement
       {
           //line 1
           cout << "Consumer " << pthread_self() << " reads ";
           cout << bItem;

           stats[0] = (stats[0]+1); //increase number of times consumed

           if(checkPrime(bItem)) //Checks if the number is prime
               cout << "   * * * PRIME * * *";
           cout << endl;

           //if snapshots are enabled
           if(global_args.outputBufferSnapshots)
               buffer_print();
       }

       //Unlock
       pthread_mutex_unlock( &mutex );

       //Signal
       sem_post( &bufferFull );

   }

   pthread_exit( (void*)stats ); //return stats for this thread

}

//Sample input
//30 3 2 2 yes
