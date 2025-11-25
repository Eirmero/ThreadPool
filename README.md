# C++ Pool Design (2025.11.14)
A lightweight C++ thread pool implemented from scratch for concurrency practice.  
Supports multiple worker threads, task queueing, and safe shutdown.

## Update (2025.11.21)
- Added **return-value support** for tasks.
- `enqueue()` now returns `std::future<T>`, allowing tasks to return results.
- Internally implemented using `std::packaged_task` + `std::future`.

## Update (2025.11.22)
- Added a **Memory_Pool** .
- Auto allocate & deallocate blocks for tasks from pre-allocated memory page.
- Calculate page size & auto align with system `size_t`.

## Features
- Not very simple and clean thread pool implementation
- MPMC task queue 
- `std::future` return values

## Update (2025.11.25)
- Added a **LRU** .
