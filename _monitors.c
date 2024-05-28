#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>

int available_resources;
int min_resources;
int max_resources;
pthread_mutex_t resource_mutex;
pthread_cond_t resource_cond;
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

  fprintf(log_file, "[%s] [Thread %ld] (%d) %s\n", buffer, thread_id, iteration, action);
}

/* decrease available resources by count resources */
/* return 0 if sufficient resources available, */
/* otherwise return -1 */
int decrease_count(int count)
{
  pthread_mutex_lock(&resource_mutex);
  while (available_resources < count)
  {
    pthread_cond_wait(&resource_cond, &resource_mutex);
  }
  available_resources -= count;
  pthread_mutex_unlock(&resource_mutex);
  return 0;
}

/* increase available resources by count */
int increase_count(int count)
{
  pthread_mutex_lock(&resource_mutex);
  available_resources += count;
  pthread_cond_broadcast(&resource_cond);
  pthread_mutex_unlock(&resource_mutex);
  return 0;
}

void *thread_function(void *arg)
{
  long thread_id = (long)arg;
  int num_iterations = *((int *)arg + 1); // Get NUM_ITERATIONS from the argument
  for (int i = 0; i < num_iterations; i++)
  {
    log_message(thread_id, i, "STARTED: iteration");

    int resources_needed = (rand() % (max_resources - min_resources + 1)) + min_resources; // Threads will need between min_resources to max_resources
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "REQUESTING: %d resources", resources_needed);
    log_message(thread_id, i, buffer);

    decrease_count(resources_needed);
    snprintf(buffer, sizeof(buffer), "CONSUMED: %d resources", resources_needed);
    log_message(thread_id, i, buffer);

    // Simulate work
    int work_time = rand() % 1000000;
    snprintf(buffer, sizeof(buffer), "WORKING: during %dμs", work_time);
    log_message(thread_id, i, buffer);
    usleep(work_time);

    // Return resources
    increase_count(resources_needed);
    snprintf(buffer, sizeof(buffer), "RETURNED: %d resources", resources_needed);
    log_message(thread_id, i, buffer);
  }
  return NULL;
}

int main(int argc, char *argv[])
{
  if (argc != 7)
  {
    fprintf(stderr, "Usage: %s <NUM_THREADS> <NUM_ITERATIONS> <AVAILABLE_RESOURCES> <MIN_RESOURCES> <MAX_RESOURCES> <OUTPUT_FILE>\n", argv[0]);
    return 1;
  }

  int num_threads = atoi(argv[1]);
  int num_iterations = atoi(argv[2]);
  available_resources = atoi(argv[3]);
  min_resources = atoi(argv[4]);
  max_resources = atoi(argv[5]);
  char *output_file = argv[6];

  if (min_resources > max_resources)
  {
    fprintf(stderr, "Error: MIN_RESOURCES must be less than or equal to MAX_RESOURCES\n");
    return 1;
  }

  pthread_t threads[num_threads];
  pthread_mutex_init(&resource_mutex, NULL);
  pthread_cond_init(&resource_cond, NULL);

  log_file = fopen(output_file, "w");
  if (log_file == NULL)
  {
    perror("Error opening log file");
    return 1;
  }

  // Write the configuration to the log file
  fprintf(log_file, "CONFIGURATION (MONITORS)\n");
  fprintf(log_file, "NUM_THREADS: %d\n", num_threads);
  fprintf(log_file, "NUM_ITERATIONS: %d\n", num_iterations);
  fprintf(log_file, "AVAILABLE_RESOURCES: %d\n", available_resources);
  fprintf(log_file, "MIN_RESOURCES: %d\n", min_resources);
  fprintf(log_file, "MAX_RESOURCES: %d\n", max_resources);
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
  pthread_mutex_destroy(&resource_mutex);
  pthread_cond_destroy(&resource_cond);
  return 0;
}
