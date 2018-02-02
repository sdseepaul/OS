/**********************************************************
 system_call.c : measures the time it takes to do system call
**********************************************************/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#define SAMPLE_SIZE 10000

int main()
{
  int i;
  double sum = 0, delta = 0, average = 0;
  struct timespec start, end;

  char data[0];

  int file_dscr = open("test.txt", O_CREAT | O_RDONLY, S_IRUSR);  //open/create file

  if(file_dscr < 0) //error
    return 1;

  for(i = 0; i < SAMPLE_SIZE; i++)
  {
    clock_gettime(CLOCK_MONOTONIC, &start); //get time right before making the sys call
    read(file_dscr,data,0);                 //make the system call read for 0 bytes
    clock_gettime(CLOCK_MONOTONIC, &end);   //get time right after making the sys call

    delta = end.tv_nsec - start.tv_nsec;    //calculate difference in time1 and time2
    //printf("%ld - %ld\n", end.tv_nsec, start.tv_nsec );
    sum += delta;                           //accumulate differences from each loop
  }

  //close file
  if(close(file_dscr) < 0)
    return 1;

  average = sum/SAMPLE_SIZE;                //calculate average
  printf("%f nanoseconds\n", average);      //display average

  return 0;
}