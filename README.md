# Thread-Safe Sliding Window Rate Limiter (C++)

A simple **multithreaded rate limiter** implemented using the **sliding window algorithm**.  
This design demonstrates a typical backend rate-limiting system suitable for **single-node services** and commonly discussed in **system design interviews**.

---

## Problem

Design a service that determines whether a client request should be allowed based on a request rate limit.

Example rule:

    limit = 5 requests
    window = 10 seconds

Meaning:

    A client may send at most 5 requests within any 10-second window.

If the limit is exceeded, the service rejects the request:

    HTTP 429 Too Many Requests

---

## Functional Requirements

### 1. Check if a request is allowed

    bool allow(clientId, timestamp)

Returns:

    true  -> request allowed
    false -> request rejected

### 2. Configure limits for a specific client

    reconfigure(clientId, limit, window)

Example:

    reconfigure("user123", 10, 60)

Meaning:

    user123 may send 10 requests every 60 seconds

### 3. Update global default configuration

    reconfigure(limit, window)

Applies to clients that do not yet have explicit configuration.

---

## Non-Functional Requirements

### Performance

    O(1) request processing

Each request should be processed with minimal overhead.

### Concurrency

The system must support multiple threads calling `allow()` simultaneously.

### Availability

The rate limiter should not become a bottleneck for API services.

### Memory Efficiency

Only timestamps inside the active rate window should be stored.

---

## High-Level Design

Each client is associated with a **bucket** that tracks request timestamps.

    Client -> Bucket

Example:

    userA -> bucket
    userB -> bucket
    userC -> bucket

Each bucket stores:

    limit
    window
    deque of timestamps

---

## Data Model

```cpp
struct Bucket {
    int limit;
    int window;
    std::deque<long long> timestamps;
};
