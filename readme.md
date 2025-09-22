# mzlib

**mzlib** is a modern C++20 header-only library providing foundational utilities for scientific, numerical, and geometric computing. It offers robust, type-safe abstractions for containers, bit manipulation, error handling, and more, with a focus on performance and composability.

## Features

- **Generic Containers**: Dynamic arrays (`Vector`), non-owning views (`Span`, `Slice`), and pointer stacks (`Stack`).
- **Elementwise Operations**: CRTP interfaces for arithmetic, logical, and formatting operations on containers.
- **Bit Manipulation**: Efficient bitset and dual-bitset types for algebraic and geometric applications.
- **Error Handling**: Macros and utilities for domain, logic, and argument errors with formatted messages.
- **Type Traits & Concepts**: C++20 concepts for arithmetic, sequence, and floating-point types.
- **Serialization**: Unified stream API for file and string I/O, supporting custom and standard containers.
- **Utilities**: Timer, formatting, enum helpers, and size/index types.

---

## File Overview

### Containers

- **Vector.h**  
  Dynamic, resizable array with elementwise operations, serialization, and interoperability with Span/Slice.

- **Span.h**  
  Lightweight, non-owning view over contiguous sequences. Supports elementwise operations, queue-like access, assignment, and casting.

- **Slice.h**  
  Non-owning view over strided sequences. Useful for submatrix or subvector views, with assignment and casting utilities.

- **Stack.h**  
  Non-owning, dynamically resizable stack of pointers. Provides push/pop, sorting, and uniqueness.

### Elementwise Operations

- **ElementwiseOperationsInterface.h**  
  CRTP base classes for elementwise arithmetic, logical, and formatting operations on containers.

### Bit Manipulation

- **zbitset.h**  
  Efficient bitset (`BitsT<T>`) and dual-bitset (`BitLinesT<T>`) types. Supports bitwise operations, counting, scanning, and geometric logic.

### Algorithms & Utilities

- **algorithm.h**  
  Generic search and partition algorithms for pointer ranges, including binary search and sign-based partitioning.

- **RandomAccessIteratorInterface.h**  
  Generic random access iterator classes for containers, supporting both const and mutable access.

### Error Handling & Type Traits

- **error_utils.h**  
  Macros and functions for error reporting and exception throwing, with formatted output.

- **concept_utils.h**  
  C++20 concepts and type traits for arithmetic, floating-point, sequence, and memory layout.

- **enum_utils.h**  
  Comparison operators and utilities for enum types.

- **size_types.h**  
  Common type aliases for sizes, indices, and IDs, with null value and comparison utilities.

### Formatting & String Utilities

- **string_utils.h**  
  Formatting helpers for sequences and generators, plus basic statistics functions.

### Serialization

- **zstream.h**  
  Unified stream API for file and string I/O. Supports serialization of trivially copyable types and standard containers.

### Miscellaneous

- **timer_utils.h**  
  High-resolution timer class for measuring durations and generating time-based seeds.

- **XA.h**  
  Specialized integer vector with custom move assignment logic for efficient memory management.

---