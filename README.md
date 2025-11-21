# C++ Thread Pool (2025.11.14)

A lightweight C++ thread pool implemented from scratch for concurrency practice.  
Supports multiple worker threads, task queueing, and safe shutdown.

## Update (2025.11.21)
- Added **return-value support** for tasks.
- `enqueue()` now returns `std::future<T>`, allowing tasks to return results.
- Internally implemented using `std::packaged_task` + `std::future`.

## Features
- Simple and clean thread pool implementation
- MPMC task queue (multiple producers / multiple consumers)
- `std::future` return values
- Graceful shutdown using condition variable
