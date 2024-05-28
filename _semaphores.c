#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>

int available_resources;
sem_t resource_semaphore;
FILE *log_file;

void log_message(long thread_id, int iteration, const char *action)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);

  int minutes = tv.tv_sec / 60 % 60;
  int seconds = tv.tv_sec % 60;
  int microseconds = tv.tv_usec;

  char buffer[50];
  sprintf(buffer, "%02d:%02d:%06d", minutes, seconds, microseconds);

  fprintf(log_file, "[%s] [Thread %ld] (%d) %s\n", buffer, thread_id, iteration + 1, action);
}

/* decrease available resources by count resources */
/* return 0 if sufficient resources available, */
/* otherwise return -1 */
int decrease_count(int count)
{
  available_resources -= count;
  return 0;
}

/* increase available resources by count */
int increase_count(int count)
{
  available_resources += count;
  return 0;
}

void *thread_function(void *arg)
{
  long thread_id = *((int *)arg);         // Get thread_id from the argument
  int num_iterations = *((int *)arg + 1); // Get NUM_ITERATIONS from the argument
  for (int i = 0; i < num_iterations; i++)
  {
    log_message(thread_id, i, "STARTED: iteration");

    sem_wait(&resource_semaphore);

    decrease_count(1);
    log_message(thread_id, i, "CONSUMED: a resource");

    // Simulate work
    int work_time = rand() % 1000000;
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "WORKING: during %dμs", work_time);
    log_message(thread_id, i, buffer);
    usleep(work_time);

    // Return resource
    increase_count(1);
    log_message(thread_id, i, "RETURNED: a resource");

    sem_post(&resource_semaphore);
  }
  return NULL;
}

int main(int argc, char *argv[])
{
  if (argc != 5)
  {
    fprintf(stderr, "Usage: %s <NUM_THREADS> <NUM_ITERATIONS> <AVAILABLE_RESOURCES> <OUTPUT_FILE>\n", argv[0]);
    return 1;
  }

  int num_threads = atoi(argv[1]);
  int num_iterations = atoi(argv[2]);
  available_resources = atoi(argv[3]);
  char *output_file = argv[4];

  pthread_t threads[num_threads];
  sem_init(&resource_semaphore, 0, available_resources);

  log_file = fopen(output_file, "w");
  if (log_file == NULL)
  {
    perror("Error opening log file");
    return 1;
  }

  // Write the configuration to the log file
  fprintf(log_file, "CONFIGURATION\n");
  fprintf(log_file, "NUM_THREADS: %d\n", num_threads);
  fprintf(log_file, "NUM_ITERATIONS: %d\n", num_iterations);
  fprintf(log_file, "AVAILABLE_RESOURCES: %d\n", available_resources);
  fprintf(log_file, "OUTPUT_FILE: %s\n", output_file);
  fprintf(log_file, "\n");
  fprintf(log_file, "FORMAT\n");
  fprintf(log_file, "[HH:MM:SS:μμμμμμ] [Thread <Thread Id>] (<Iteration>) <Action>: <Description>\n");
  fprintf(log_file, "\n");
  fprintf(log_file, "LOG\n");

  for (long i = 0; i < num_threads; i++)
  {
    int args[2] = {i, num_iterations}; // Pack thread_id and num_iterations into args
    pthread_create(&threads[i], NULL, thread_function, args);
  }

  for (int i = 0; i < num_threads; i++)
  {
    pthread_join(threads[i], NULL);
  }

  fclose(log_file);
  sem_destroy(&resource_semaphore);
  return 0;
}
