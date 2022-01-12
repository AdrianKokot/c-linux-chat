# Linux Chat App

Linux chat app that uses only System V inter-process communication facilities (IPCS) to communicate written in C.

Project created for "System and Concurrent Programming" course at Poznan University of Technology.

## General description

The app has two executable files: server and client. It's the client-server architecture, so there's one server and many clients.

Server uses one IPC message queue to communicate with all clients. Users are differentiate by [`connectionId`](#protocol) ([msgrcv's msgtyp parameter](https://linux.die.net/man/2/msgrcv)).

The user defines the IPC queue id during the start of the file.

## [](#protocol) Protocol

Structs used during communication are defined as follows:

```c
typedef struct {
    long connectionId;
    char body[REQUEST_BODY_MAX_SIZE];
    unsigned long bodyLength;
    RType type;
    long responseConnectionId;
    int channelId;
} Request;

typedef struct {
    long connectionId;
    char body[REQUEST_BODY_MAX_SIZE];
    unsigned long bodyLength;
    RType type;
    StatusCode status;
} Response;
```

Purpose of struct's properties:

* **`long connectionId`** - IPC message type ([`long msgtyp`](https://linux.die.net/man/2/msgrcv)), treated as userId and the main type for performing *[requests](#request)*
* **`char body[]`** - message body (text)
* **`unsigned long bodyLength`** - length of body (string length)
* **`RType type`** - *[response/request type](#rtype)* defining what server is asked for (during the request) or what contains the response
* **`long responseConnectionId`** - IPC message type that will be used to send *[response](#response)* to *[request](#request)*
* **`int channelId`** - the channel id that user is connected to
* **`StatusCode status`** - response *[status](#statuscode)*

### [](#rtype) RType

RType is an enum defined as follows:

```c
enum eRType {
    R_Init = 1,
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
    R_Heartbeat,
    R_EndConnection
};

typedef enum eRType RType;
```

`RType` property is used in the requests and responses to define what the request/response contains. For example: on the `R_ListChannel` request, the server will respond with a `R_ListChannel` response. The response will contain a list of channels in the `body`.

`R_Init` and `R_RegisterUser` are "unregistered requests". It's because during those requests user is not known to the server. During the `R_Init` request client establishes a connection with the server, and during the `R_RegisterUser` request client sets its username.

`R_EndConnection` is used only to send [`StatusNotVerified`](#statuscode) to all connected users before shutting down the server.

The rest types are used in general communication to get the data from the server.

### [](#statuscode) StatusCode

StatusCode is an enum defined as follows:

```c
typedef enum {
    StatusOK,
    StatusServerFull,
    StatusValidationError,
    StatusNotVerified
} StatusCode;
```

StatusCode is used to define the status of the response from the server. The client can react to the response status without parsing the body.

Usage of statuses:

* `StatusOK` - sent when the request is parsed correctly, and the body has contents that match the [request rtype](#rtype)
* `StatusServerFull` - sent when the server has reached the limit of users. Sent only in answer to [unregistered requests](#rtype)
* `StatusValidationError` - sent when the request body doesn't match requirements. Example: username is already taken, the username is too long/short.
* `StatusNotVerified` - sent when the user that performs request is no longer verified (didn't respond to [`R_Hearbeat`](#heartbeat))

### [](#request) Request

During request, every property is filled, but not always all of the properties are used. For example: `channelId` is used when sending a message to the channel.

Request has a property named `responseConnectionId`. The server listens to all IPC messages with the type below `queueTypeGap` (calculated at the start of the server). The `responseConnectionId` is always above `queueTypeGap`. That prevents the server from detecting its responses as requests.

During communication with the user, there is one queue with 6 different types (msgtype):

* 2 types are used for general requests. The client asks for the first `connectionId` during `R_Init` request. By using `connectionId` client calculates `responseConnectionId`.
* 2 types are used for messages (private and channel). For sending a message the client uses `connectionId + 1`. The server sends the message to other clients using their's `responseConnectionId + 1`.
* 2 types are used for the heartbeat mechanism. After every request server checks the user's connection. During check, the server sends `R_Heartbeat` request on user's `connectionId + 2` and listens for answer on user's `responseConnectionId + 2`.

### [](#response) Response

If the response is successful, its `body` contains information related to the request's `RType`. Otherwise, the server responds with the error message in `body`.

### [](#heartbeat) Heartbeat mechanism

Heartbeat is a mechanism that allows the server to verify if the user is still connected to the server. The server listens for the requests in the loop. At the end of the loop, there's a user check.

User check is defined as follows:

1. If the last check was performed later than 5 seconds ago, continue
2. Send `R_Heartbeat` request to the client
3. Wait 10ms for response
4. If the client responded, mark as verified. Otherwise, delete the user from the user's list. The next requests are going to be rejected with `StatusNotVerified`.

![Heartbeat messages communication diagram](https://mermaid.ink/img/eyJjb2RlIjoic2VxdWVuY2VEaWFncmFtXG5TZXJ2ZXItLT4-Q2xpZW50OiBSZXF1ZXN0IFJfSGVhcnRiZWF0IFtjbGllbnQuY29ubmVjdGlvbklkKzJdXG5DbGllbnQtLT4-U2VydmVyOiBSZXNwb25zZSBSX0hlYXJ0YmVhdCBbY2xpZW50LnJlc3BvbnNlQ29ubmVjdGlvbklkKzJdXG5Ob3RlIGxlZnQgb2YgU2VydmVyOiBHb3QgaGVhcmJlYXQgcmVzcG9uc2U8YnI-TWFya2luZyB1c2VyIGFzIHZlcmlmaWVkXG5Ob3RlIGxlZnQgb2YgU2VydmVyOiBPdGhlcndpc2UgZGVsZXRpbmcgdXNlciIsIm1lcm1haWQiOiJ7XG4gIFwidGhlbWVcIjogXCJkZWZhdWx0XCJcbn0iLCJ1cGRhdGVFZGl0b3IiOmZhbHNlLCJhdXRvU3luYyI6dHJ1ZSwidXBkYXRlRGlhZ3JhbSI6ZmFsc2V9)

### Communication diagram

Communication on diagram is described as follows:
`Request/Response RType [connectionId]`

* RType describes RType used during communication
* connectionId describes name of variable and context of used [connectionId](#protocol) (msgtyp)

#### General requests

The server answers with the response of the same `RType` as the client's request. Only when the user is not verified, the server returns `R_NotVerified` and refuse to answer the client.

![General requests communication diagram](https://mermaid.ink/img/eyJjb2RlIjoic2VxdWVuY2VEaWFncmFtXG5DbGllbnQtLT4-U2VydmVyOiBSZXF1ZXN0IFJUeXBlIFtjbGllbnQuY29ubmVjdGlvbklkXVxuU2VydmVyLS0-PkNsaWVudDogUmVzcG9uc2UgUlR5cGUgW3JlcXVlc3QucmVzcG9uc2VDb25uZWN0aW9uSWRdXG5Ob3RlIGxlZnQgb2YgU2VydmVyOiBXaGVuIHVzZXIgaXMgdmVyaWZpZWRcblNlcnZlci0tPj5DbGllbnQ6IFJlc3BvbnNlIFJfTm90VmVyaWZpZWQgW3JlcXVlc3QucmVzcG9uc2VDb25uZWN0aW9uSWRdXG5Ob3RlIGxlZnQgb2YgU2VydmVyOiBXaGVuIHVzZXIgaXMgbm90IHZlcmlmaWVkXG4iLCJtZXJtYWlkIjoie1xuICBcInRoZW1lXCI6IFwiZGVmYXVsdFwiXG59IiwidXBkYXRlRWRpdG9yIjpmYWxzZSwiYXV0b1N5bmMiOnRydWUsInVwZGF0ZURpYWdyYW0iOmZhbHNlfQ)

#### Channel & private messages

The client listens for channel and private messages on a different process. That makes whole message communication asynchronous.

The server parses the message and adds the additional information: the time of the message, sender, receiver (only when private message), and channel name (only when channel message).

##### Channel messages

The channel message request contains the message in the `body`. The channel is specified in the request struct (`channelId`).

![Channel messages communication diagram](https://mermaid.ink/img/eyJjb2RlIjoic2VxdWVuY2VEaWFncmFtXG5TZW5kZXItLT4-U2VydmVyOiBSZXF1ZXN0IFJfQ2hhbm5lbE1lc3NhZ2UgW3NlbmRlci5jb25uZWN0aW9uSWRdXG5TZXJ2ZXItLT4-Q2hhbm5lbCBjbGllbnRzOiBSZXNwb25zZSBSX0NoYW5uZWxNZXNzYWdlIFtjbGllbnQucmVzcG9uc2VDb25uZWN0aW9uSWQrMV1cbiIsIm1lcm1haWQiOiJ7XG4gIFwidGhlbWVcIjogXCJkZWZhdWx0XCJcbn0iLCJ1cGRhdGVFZGl0b3IiOmZhbHNlLCJhdXRvU3luYyI6dHJ1ZSwidXBkYXRlRGlhZ3JhbSI6ZmFsc2V9)

##### Private message

`body` of private message request contains receiver's username and the message, separated by a space.

![Private messages communication diagram](https://mermaid.ink/img/eyJjb2RlIjoic2VxdWVuY2VEaWFncmFtXG5TZW5kZXItLT4-U2VydmVyOiBSZXF1ZXN0IFJfUHJpdmF0ZU1lc3NhZ2UgW3NlbmRlci5jb25uZWN0aW9uSWRdXG5TZXJ2ZXItLT4-Q2hhbm5lbCBjbGllbnRzOiBSZXNwb25zZSBSX1ByaXZhdGVNZXNzYWdlIFtyZWNlaXZlci5yZXNwb25zZUNvbm5lY3Rpb25JZCsxXVxuU2VydmVyLS0-PlNlbmRlcjogUmVzcG9uc2UgUl9Qcml2YXRlTWVzc2FnZSBbc2VuZGVyLnJlc3BvbnNlQ29ubmVjdGlvbklkKzFdXG4iLCJtZXJtYWlkIjoie1xuICBcInRoZW1lXCI6IFwiZGVmYXVsdFwiXG59IiwidXBkYXRlRWRpdG9yIjpmYWxzZSwiYXV0b1N5bmMiOnRydWUsInVwZGF0ZURpYWdyYW0iOmZhbHNlfQ)

## Compilation

```bash
gcc -Wall shared/* resources/client/* client.c -o client.out
gcc -Wall shared/* resources/server/* server.c -o server.out
```

## How to run

Executable files have help, which can be run by `./<executable-file> --help`.

### Server

When you run the server file, you'll be asked about the IPC id to create a queue.
You can also provide the queue id by `-i <id>` or `--id <id>` parameter.

### Client

When you execute the client file, there'll be a question about the IPC id to connect to (it has to exist and be used by the server).
You can also provide that by `-s <id>` or `--server <id>` parameter.

After getting the correct server id, there'll be a question about the username. It has to be unique.
You can also provide that by `-u <username>` or `--username <username>`.
