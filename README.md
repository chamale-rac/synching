# synching 🧵🔧
Experimenting with software synchronization concepts using C and C++ 

| 🧪 Laboratorio 5 - SISTEMAS OPERATIVOS - SECCIÓN - 20 - 2024 - 1

## Program 1: Using Semaphores 🚦

### Description
This program manages the consumption of resources by multiple threads using semaphores. It logs detailed information about the operations performed by each thread.

### Compilation
To compile the program, use the following command:
```sh
gcc -o _semaphores _semaphores.c -lpthread
```

### Execution
To run the compiled program, use the following command:
```sh
./_semaphores <NUM_THREADS> <NUM_ITERATIONS> <AVAILABLE_RESOURCES> <OUTPUT_FILE>
```

### Example
```sh
./_semaphores 5 10 5 semaphore_log.txt
```

### Log File
The program will create a log file named `semaphore_log.txt` in the same directory, recording the detailed operations performed by each thread.

---

## Program 2: Using Monitors 📏

### Description
This program allows threads to consume a given number of resources and uses a monitor for synchronization. It logs detailed information about the operations performed by each thread.

### Compilation
To compile the program, use the following command:
```sh
gcc -o _monitors _monitors.c -lpthread
```

### Execution
To run the compiled program, use the following command:
```sh
./_monitors <NUM_THREADS> <NUM_ITERATIONS> <AVAILABLE_RESOURCES> <MIN_RESOURCES> <MAX_RESOURCES> <OUTPUT_FILE>
```

### Example
```sh
./_monitors 5 10 5 1 3 monitor_log.txt
```

### Log File
The program will create a log file named `monitor_log.txt` in the same directory, recording the detailed operations performed by each thread.

---

## Team 🤓

Samuel Chamalé