# muStore - A Nested Key-Value Store with TCP Interface

A high-performance key-value store that supports:
- Nested field storage per key
- Multiple transport methods (zero-copy, one-copy, two-copy)
- TCP network interface
- Concurrent access
- Debug logging

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Running the Server

```bash
# Start server on port 8080
./src/mustore
```

## Usage Examples

Connect using netcat:

```bash
nc localhost 8080
```

### Create a new key with fields
```
SET_FIELDS user123 name="John Doe" email="john@example.com" age="30"
```
Response: 0 user123 0

### Get all fields for a key
```
GETALL user123
```
Response:
```
1 user123 3 name="John Doe" email="john@example.com"  age="30"
```

### Get specific fields
```
GET_FIELDS user123 name email
```
Response:
```
1 user123 2 name="John Doe" email="john@example.com"
```

### Update fields
```
SET_FIELDS user123 age="31" city="New York"
```
Response: 0 user123 0

## Performance Testing

Different transport methods can be tested by modifying the transport type in Server.cpp processCommand() function.:

```cpp

    //Available send methods are TWOCOPY, ONECOPY, ZEROCOPY, MuSer(not implemented)
    response.sendMethod = Response::SendMethod::TWOCOPY; // Modify this line to use different send methods 

    
```

