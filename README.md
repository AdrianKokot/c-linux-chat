# Linux Chat App

Linux chat app that uses only System V inter-process communication facilities (IPCS) to communicate written in C.

Project created for "System and Concurrent Programming" course at Poznan University of Technology.

## Protocol

C structs used during communication:

```c
typedef  struct {
 long type;
 char body[REQUEST_BODY_MAX_SIZE];
 unsigned long bodyLength;
 StatusCode status;
 RType rtype;
 long responseType;
 int channelId;
} Request;

typedef Request Response;
```

Purpose of struct's properties:

* **type** - communication type also treated as userId
* **body** - communicate body
* **bodyLength** - length of communicate body (string length)
* **status** - response status
* **rtype** - response/request type defining what server is asked for (request) or what contains the response
* **responseType** - communication type used to send response
* **channelId** - id of the channel that user is connected to

StatusCode and RType are enums defined as follows:

```c
typedef enum {
 StatusG,
 StatusOK,
 StatusServerFull,
 StatusValidationError,
 StatusInternalError,
 StatusNotVerified
} StatusCode;

typedef enum {
 R_Generic,
 R_Init,
 R_RegisterUser,
 R_UnregisterUser,
 R_ListChannel,
 R_JoinChannel,
 R_LeaveChannel,
 R_CreateChannel,
 R_ListUsersOnChannel,
 R_ListUsers,
 R_ChannelMessage,
 R_PrivateMessage,
 R_Heartbeat
} RType;
```

### Communication diagram

In square brackets on diagrams contains the communication type id.

#### General requests

On the client request, the server answers with a response of the same `R_Type`. Only when the user is not verified, the server returns `R_NotVerified` and refuse to answer the client.

<img src="https://mermaid.ink/img/eyJjb2RlIjoic2VxdWVuY2VEaWFncmFtXG5DbGllbnQgLS0-PiBTZXJ2ZXI6IFJlcXVlc3QgUl9UeXBlIFt0eXBlXSBcbk5vdGUgcmlnaHQgb2YgU2VydmVyOiBJZiB1c2VyIGlzIHZlcmlmaWVkXG5TZXJ2ZXIgLS0-PiBDbGllbnQ6IFJlc3BvbnNlIFJfVHlwZSBbcmVxdWVzdC5yZXNwb25zZVR5cGVdXG5Ob3RlIHJpZ2h0IG9mIFNlcnZlcjogSWYgdXNlciBpcyBub3QgdmVyaWZpZWRcblNlcnZlciAtLT4-IENsaWVudDogUmVzcG9uc2UgUl9Ob3RWZXJpZmllZCBbcmVxdWVzdC5yZXNwb25zZVR5cGVdIiwibWVybWFpZCI6IntcbiAgXCJ0aGVtZVwiOiBcImRlZmF1bHRcIlxufSIsInVwZGF0ZUVkaXRvciI6ZmFsc2UsImF1dG9TeW5jIjp0cnVlLCJ1cGRhdGVEaWFncmFtIjpmYWxzZX0" />

#### Channel & private messages

The client listens for channel and private messages on a different process. That makes whole message communication asynchronous.

Communication type is incremented by 1 to avoid collision with the general and heartbeat requests.

##### Channel messages

![Channel messages communication diagram](https://mermaid.ink/img/eyJjb2RlIjoic2VxdWVuY2VEaWFncmFtXG5TZW5kZXIgLS0-PiBTZXJ2ZXI6IFJlcXVlc3QgUl9DaGFubmVsTWVzc2FnZSBbdXNlci50eXBlXVxuU2VydmVyIC0tPj4gQ2hhbm5lbCBjbGllbnRzIChhbHNvIHNlbmRlcik6IFJlc3BvbnNlIFJfQ2hhbm5lbE1lc3NhZ2UgW2NsaWVudC5yZXNwb25zZVR5cGUrMV0iLCJtZXJtYWlkIjoie1xuICBcInRoZW1lXCI6IFwiZGVmYXVsdFwiXG59IiwidXBkYXRlRWRpdG9yIjpmYWxzZSwiYXV0b1N5bmMiOnRydWUsInVwZGF0ZURpYWdyYW0iOmZhbHNlfQ "Channel messages communication diagram")

##### Private message

![Private messages communication diagram](https://mermaid.ink/img/eyJjb2RlIjoic2VxdWVuY2VEaWFncmFtXG5TZW5kZXIgLS0-PiBTZXJ2ZXI6IFJlcXVlc3QgUl9Qcml2YXRlTWVzc2FnZSBbdXNlci50eXBlXVxuU2VydmVyIC0tPj4gUmVjZWl2ZXI6IFJlc3BvbnNlIFJfUHJpdmF0ZU1lc3NhZ2UgW2NsaWVudC5yZXNwb25zZVR5cGUrMV1cblNlcnZlciAtLT4-IFNlbmRlcjogUmVzcG9uc2UgUl9Qcml2YXRlTWVzc2FnZSBbY2xpZW50LnJlc3BvbnNlVHlwZSsxXSIsIm1lcm1haWQiOiJ7XG4gIFwidGhlbWVcIjogXCJkZWZhdWx0XCJcbn0iLCJ1cGRhdGVFZGl0b3IiOmZhbHNlLCJhdXRvU3luYyI6dHJ1ZSwidXBkYXRlRGlhZ3JhbSI6ZmFsc2V9 "Private messages communication diagram")

#### Heartbeat mechanism

Every 5 seconds (+- current response time) server sends `R_Heartbeat` request to the client. If the client sends a response in 10ms, the user is marked as verified. Otherwise is marked as not verified and the next request will be rejected with `R_NotVerified` response.

Communication type is incremented by 2 to distinct the communication from general requests and messages.

![Heartbeat messages communication diagram](https://mermaid.ink/img/eyJjb2RlIjoic2VxdWVuY2VEaWFncmFtXG5TZXJ2ZXIgLS0-PiBDbGllbnQ6IFJlcXVlc3QgUl9IZWFydEJlYXQgW3VzZXIudHlwZSsyXVxuQ2xpZW50IC0tPj4gU2VydmVyOiBSZXNwb25zZSBSX0hlYXJ0QmVhdCBbdXNlci5yZXNwb25zZVR5cGUrMl1cbk5vdGUgbGVmdCBvZiBTZXJ2ZXI6IEdvdCBoZWFydGJlYXQgcmVzcG9uc2UgPGJyPiBNYXJraW5nIHVzZXIgYXMgdmVyaWZpZWRcbk5vdGUgbGVmdCBvZiBTZXJ2ZXI6IE90aGVyd2lzZSBtYXJraW5nIGFzIG5vdCB2ZXJpZmllZCIsIm1lcm1haWQiOiJ7XG4gIFwidGhlbWVcIjogXCJkZWZhdWx0XCJcbn0iLCJ1cGRhdGVFZGl0b3IiOmZhbHNlLCJhdXRvU3luYyI6dHJ1ZSwidXBkYXRlRGlhZ3JhbSI6ZmFsc2V9 "Heartbeat messages communication diagram")

## Compilation

```bash
gcc -Wall shared/* resources/client/* client.c -o client.out
gcc -Wall shared/* resources/server/* server.c -o server.out
```

## How to run

Executable files have help, which can be run by `./<executable-file> --help`.

### Server

When you run the server file, you'll be asked about ipc id to create a queue.
You can also provide the queue id by `-i <id>` or `--id <id>` parameter.

### Client

When you run the client file, you'll be asked about ipc id to connect to (it has to exist and has to be used by the server).
You can also provide that by `-s <id>` or `--server <id>` parameter.

After getting a correct server id, you'll be asked about the username. It has to be unique.
You can also provide that by `-u <username>` or `--username <username>`.
