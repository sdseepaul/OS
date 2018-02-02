/**********************************************************
 context_switch.c : measures the time between a context switch
**********************************************************/
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <sched.h>
#include <math.h>

#define SAMPLE_SIZE 500

int main()
{
  pid_t pid;
  int pp[2], cp[2], i, message;
  long time_stmp, delta, sum;
  struct timespec start, end;

  //Bind process to designated CPU
  cpu_set_t mask;                                   //define bit mask
  CPU_ZERO(&mask);                                  //Initialze to 0
  CPU_SET(1, &mask);                                //set to desired core
  sched_setaffinity(0,sizeof(cpu_set_t), &mask);    //set affinity of process to defined mask

  //Create pipes
  if(pipe(pp) < 0 || pipe(cp) < 0)                  //check for error when creating pipes
    return 1;

  //fork process to create child
  pid = fork();

  if(pid < 0) //check if fork failed
    return 1;

  else if (pid > 0) //parent process
  {
    for(i = 0; i < SAMPLE_SIZE; i++)
    {
      close(pp[0]);                                       //close read end of parent pipe before writing from it
      write(pp[1], " ", 1);                               //send arbitrary data to child via parent pipe
      clock_gettime(CLOCK_MONOTONIC, &start);             //get time closest to start of the context switch

      close(cp[1]);                                       //close write end of child pipe
      read(cp[0], &time_stmp, sizeof(time_stmp));         //read timestamp from child pipe

      delta = time_stmp - start.tv_nsec;                  //(time child read pipe message) - (time parent sent message)
  //    printf("pdelta:%ld\n", delta);                    //use to check difference values
      sum += delta;
    }

    printf("AVG: %ld nanoseconds\n", sum/SAMPLE_SIZE);    //display average of sample size
  }

  else  //child process
  {
    for(i = 0; i < SAMPLE_SIZE; i++)
    {
      close(pp[1]);                                       //close write end of parent pipe before reading from it
      read(pp[0], &message, sizeof(message));             //read in the value sent from parent
      clock_gettime(CLOCK_MONOTONIC, &end);               //get time closest to end of read from parent pipe

      close(cp[0]);                                       //read end of child pipe
      write(cp[1], &end.tv_nsec, sizeof(end.tv_nsec));    //send the time at which read finished to the parent via child pipe
    }
  }

  return 0;
}