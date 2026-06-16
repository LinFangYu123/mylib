# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Development

This is a pure C library for Linux (no build system). Compile manually with gcc:

```bash
# Compile the full library (headers in root + subdirs, pthread/epoll linking as needed)
gcc -I. -Ibase -ITcp -IThreadPool -Iepoll_reactor -Ilog \
    base/*.c Tcp/Socket.c ThreadPool/ThreadPool.c epoll_reactor/epoll_reactor.c log/logc.c \
    -lpthread -o mylib

# Compile the TCP server test
gcc -I. -ITcp Tcp/Socket.c Tcp/testserv.c -o Tcp/server

# Compile the TCP client test
gcc -I. -ITcp Tcp/Socket.c Tcp/testclie.c -o Tcp/client

# Compile the thread pool test
gcc -I. -IThreadPool ThreadPool/ThreadPool.c ThreadPool/test.c -lpthread -o ThreadPool/test

# Compile the alternative thread pool
gcc -I. -IThreadPool ThreadPool/otherpool.c -lpthread -o ThreadPool/otherpool
```

**Key flags:** `-lpthread` for thread pool, no extra flags for base data structures. All code targets Linux (epoll, POSIX sockets, pthreads).

## Architecture

Five independent modules, all in C, no shared internal state between modules:

### `base/` — Core Data Structures (4650 lines)
Header `base.h` includes `list.h` and `avl.h`. Provides bit manipulation macros (SET_BIT, CLEAR_BIT, TEST_BIT, bitfield operations, IS_POWER_OF_TWO).

- **`list.h/c`** — Doubly-linked list (`dlist_s`) and singly-linked list (`slist_s`). Independent APIs, no shared code. Features: push front/back, insert at position, remove, find with comparator, foreach, reverse (slist), cycle detection (slist), merge sorted (slist).
- **`hash.h/c`** — Hash table with 3 collision strategies: chain, linear probe, quadratic probe. Built-in hash functions (DJB2, FNV-1a, MurmurHash3, SDBM, integer, combine) and comparators (string, int, pointer, memory). Supports auto-resize via prime table.
- **`avl.h/c`** — AVL self-balancing BST. Insert/delete/search with 4 traversal orders (inorder, preorder, postorder, levelorder). Predecessor/successor queries, validate function for debugging.
- **`rbtree.h/c`** — Red-black tree with sentinel nil node. Same API surface as AVL plus range query, reverse inorder, black height calculation.

### `Tcp/` — POSIX Socket Wrappers (404 lines)
`Socket.h/c` wraps socket(), bind(), listen(), accept(), connect(), read/write/recv/send with EINTR retry loops. `initTcpSocket()` creates a listening server socket (SO_REUSEADDR, backlog 128). `initTcpConn()` connects to a remote host (defaults to 127.0.0.1). `testclie.c` / `testserv.c` are simple echo client/server examples. No dependencies on other modules.

### `ThreadPool/` — Thread Pool (928 lines)
Two implementations:
- **`ThreadPool.h/c`** — Dynamic thread pool using a doubly-linked worker list and singly-linked task queue. Workers are detached. Idle workers are reaped when exceeding `max_free_thread_num`. New workers are spawned on demand when no free workers exist and capacity allows. Task queue uses mutex + condition variable.
- **`otherpool.c`** — Alternative pool with a management thread (`adjust_thread`) that periodically adjusts thread count based on queue depth. Uses a ring buffer task queue with min/max thread bounds.

### `epoll_reactor/` — Epoll Event Reactor (110 lines)
`epoll_reactor.h/c` wraps epoll_ctl with an `my_event_s` structure. Uses `base.h` bit macros for epoll flag tracking. `eventset()` / `eventadd()` / `eventmod()` / `eventdel()` map to EPOLL_CTL_ADD/MOD/DEL. Depends on base.h for bit operations and Socket.h.

### `log/` — Simple Logger (82 lines)
`logc.h/c` writes timestamped log entries to a file. Opens/closes the file on every write. Uses `FILE*` global. Thread-safe only at the `fprintf` level — `va_list` formatted output.

## Code Style

### 1. Brace Placement — Allman Style
All opening braces `{` must be on their own line, never on the same line as the control statement:
```c
// ✅ Correct
if (condition)
{
    ...
}
for (int i = 0; i < n; i++)
{
    ...
}
while (running)
{
    ...
}
int func(void)
{
    ...
}

// ❌ Wrong
if (condition) {
    ...
}
```

### 2. Yoda Conditions — Constants on the Left
Put constants on the left side of `==` / `!=` to prevent accidental assignment (`=` instead of `==`):
```c
// ✅ Correct
if (NULL == ptr)
if (0 == ret)
while (NULL != ptr)
if (0 != func())

// ❌ Wrong
if (ptr == NULL)
if (ret == 0)
```

### 3. Parenthesize Compound Boolean Expressions
Every sub-condition in compound boolean expressions must be wrapped in parentheses:
```c
// ✅ Correct
if ((0 != ret) && (errno == EINTR))
if (((a > b) && (c < d)) || (e == f))

// ❌ Wrong
if (0 != ret && errno == EINTR)
if (a > b && c < d || e == f)
```
