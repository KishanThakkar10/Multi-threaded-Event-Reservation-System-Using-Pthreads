# Multi-threaded Event-Reservation System Using Pthreads

## Overview
This project implements a multi-threaded event-reservation system designed to handle concurrent seat reservations, inquiries, and cancellations for an auditorium. The system utilizes pthreads, semaphores, and mutexes to ensure efficient handling of multiple queries while maintaining database consistency and avoiding overloading the system.

## Features
- **Concurrent Queries**: Supports simultaneous seat inquiries, bookings, and cancellations.
- **Synchronization**: Ensures mutual exclusion for read/write operations on the same event to maintain data integrity.
- **Scalability**: Can manage a large number of events and threads, with a configurable maximum limit on active queries.
- **Randomized Query Generation**: Simulates real-world scenarios with randomly generated queries and delays.

## Project Structure
- `main.cpp`: Contains the implementation of the event-reservation system using pthreads, semaphores, and mutexes.
- `README.md`: Provides an overview of the project, its features, usage, and how to run the code.

## How It Works
1. **Initialization**: The master thread initializes the number of events and the auditorium capacity. It then creates worker threads to handle queries.
2. **Query Processing**: Worker threads perform randomized queries, including seat inquiries, bookings, and cancellations. The system enforces a maximum number of active queries using semaphores.
3. **Synchronization**: Mutexes are used to ensure that read and write operations on the same event do not conflict, preserving database integrity.
4. **Execution**: The system runs for a specified duration (`T`) before shutting down and printing the final reservation status for all events.

## Future Enhancements
- Improved Query Scheduling: Implement a priority-based scheduling system to handle urgent bookings or cancellations.
- Advanced Error Handling: Enhance the system to recover from specific errors or conflicts without failing the query.
- GUI Integration: Develop a graphical user interface to make the system more user-friendly and accessible.
