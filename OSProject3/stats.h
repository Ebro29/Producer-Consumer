#ifndef STATS_H
#define STATS_H

#include <iostream>
using namespace std;

class Stats
{
   private:
       //vars
       int simulationTime; //holds simulation time
       int maxThreadSleepTime; //holds max sleep time for threads
       int numProducerThreads; //holds # of producer threads
       int numConsumerThreads; //holds # of consumer threads
       int sizeOfBuffer; //holds size of the buffer
       int totalNumConsumed; //gets # of items consumed
       int totalNumProduced; //gets # of items produced

   public:
       //vars
       int *totalThreadNumProduced; //holds # of items produced in array for each thread
       int *totalThreadNumConsumed; //holds # of items consumed in array for each thread
       int numTimesBufferFull; //total # of times the buffer was full
       int numTimesBufferEmpty; //total # of times the buffer was empty
       int numItemsRemaining; //total # of items that were left in the buffer

       //functions
       void print(); //does the output at the end
       Stats(int, int, int, int, int); //Constructor to set all the stuff we get from cli
       ~Stats(); //destructor that gets rid of dynamic elements
};

Stats::Stats(int _simTime, int _maxSleep, int _numProducerThreads, int _numConsumerThreads, int _bufferSize)
{
   //Vars
   simulationTime = _simTime;
   maxThreadSleepTime = _maxSleep;
   numProducerThreads = _numProducerThreads;
   numConsumerThreads = _numConsumerThreads;
   sizeOfBuffer = _bufferSize;

   //Allocate memory
   totalThreadNumProduced = new int[_numProducerThreads];
   totalThreadNumConsumed = new int[_numConsumerThreads];

}

Stats::~Stats()
{
   delete [] totalThreadNumProduced;
   delete [] totalThreadNumConsumed;
}

void Stats::print()
{

   cout << "PRODUCER / CONSUMER SIMULATION COMPLETE\n";
   cout << "=======================================\n";
   cout << "Simulation Time:" << setw (24) << simulationTime << endl;
   cout << "Maximum Thread Sleep Time:" << setw(16) << maxThreadSleepTime << endl;
   cout << "Number of Producer Threads:" << setw(16) << numProducerThreads << endl;
   cout << "Number of Consumer Threads:" << setw(16) << numConsumerThreads << endl;
   cout << "Size of Buffer:" << setw(24) << sizeOfBuffer << "\n\n";

   //Producer threads
   int totalProd = 0;

   //-----sum up the amount of items produced by all threads
   for(int i=0; i < numProducerThreads; i++)
       totalProd += totalThreadNumProduced[i];

   //-----display total found above
   cout << "Total Number of Items Produced:" << setw(11) << totalProd << endl;

   //-----for each producer thread show # of items produced
   for(int i=0; i < numProducerThreads; i++)
   {
       cout << "   Thread " << setw(2);
       cout << (i+1) << ":" << setw(30);
       cout << totalThreadNumProduced[i] << endl;
   }

   cout << endl;

   //Consumer threads
   int totalCons = 0;

   //------sum up the amount of items consumed by all threads
   for(int i=0; i < numConsumerThreads; i++)
       totalCons += totalThreadNumConsumed[i];

   //------display total # of items consumed
   cout << "Total Number of Items Consumed:" << setw(11) << totalCons << endl;

   //-----for each consumer thread show # of items produced
   for(int i=0; i < numConsumerThreads; i++)
   {
              cout << "   Thread " << setw(2);
       cout << (i+numProducerThreads+1) << ":" << setw(30);
              cout << totalThreadNumConsumed[i] << endl;
   }

   cout << endl;

   cout << "Number Of Items Remaining in Buffer:" << setw(6) << numItemsRemaining << endl;
   cout << "Number Of Times Buffer Was Full:" << setw(10) << numTimesBufferFull << endl;
   cout << "Number Of Times Buffer Was Empty:" << setw(9) << numTimesBufferEmpty << endl;

}
#endif
