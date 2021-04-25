#ifndef _BUFFER_H_DEFINED_
#define _BUFFER_H_DEFINED_

#include <iostream>
#include <iomanip>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

typedef int buffer_item;

#define BUFFER_SIZE 5
buffer_item global_buffer[BUFFER_SIZE];
unsigned int bufferCount=0; //keeps track of number of items in buffer
unsigned int buffer_In_index=0; //index position for putting stuff in buffer
unsigned int buffer_Out_index=0; //index position for pulling stuff out of buffer

//function prototypes
bool buffer_insert_item( buffer_item item );
bool buffer_remove_item( buffer_item *item );
void buffer_print();
void buffer_initialize();

//Semphores and mutexs
sem_t bufferEmpty;
sem_t bufferFull;
pthread_mutex_t mutex;

void buffer_initialize()
{
   sem_init(&bufferEmpty, 0, BUFFER_SIZE );
   sem_init( &bufferFull, 0, 0 );
   pthread_mutex_init( &mutex, NULL );
   for(int i=0 ; i < BUFFER_SIZE; i++)
   {
       global_buffer[i] = -1; //sets all buffer positions to -1
   }
}

bool buffer_insert_item( buffer_item item )
{
   cout << "Item: " << (int) item << endl; //debug print statement

   //put item into buffer
   global_buffer[buffer_In_index] = item;

   //move buffer in index plus one in circular fashion
   buffer_In_index = (buffer_In_index + 1) % BUFFER_SIZE;
   //cout << bufferCount << endl; //debug output buffercount

   //increase buffer count since item was inserted
   bufferCount++;

   return true;
}

bool buffer_remove_item( buffer_item *item )
{
   //Grab item from buffer
   *item = global_buffer[buffer_Out_index];

   //Move out to next index position in circular fashion
   buffer_Out_index = ( buffer_Out_index + 1) % BUFFER_SIZE;

   //Decrease number of items in buffer
   bufferCount--;

   return true;
}

void buffer_print()
{
   //line 1
   cout << "(buffers occupied: " << (uint) bufferCount << ")\n";

   //line 2
   cout << "buffers: ";


   for(int i=0; i < BUFFER_SIZE; i++)
       cout << " " << (uint) global_buffer[i] << " "; //print #'s in buffer
   cout << endl; //start a new line

   // Third line of output. puts dashes under numbers
   cout << "        ";
   for(int i=0; i < BUFFER_SIZE; i++)
       cout << " ----";
   cout << endl;

   // Fourth line of output. Shows position of in & out indexes
   cout << "         ";
   for(int i=0; i < BUFFER_SIZE; i++)
   {
       cout << " ";

       if(buffer_In_index == i || buffer_Out_index == i)
       {
           if( bufferCount == BUFFER_SIZE )
               cout << "WR";
           else if( bufferCount == 0 )
               cout << "RW";
           else if( buffer_In_index == i )
               cout << "W ";
           else
               cout << "R ";

           cout << " ";
       }
       else
           cout << "    ";
   }
   cout << endl << endl;


}

#endif // _BUFFER_H_DEFINED_
