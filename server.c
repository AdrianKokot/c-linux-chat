#include "shared/shared.h"

#define MAX_USERS 5

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
int messageQueueId;

void terminateServer() {
    msgctl(messageQueueId, IPC_RMID, NULL);
    kill(getpid(), 9);
}

int currentUsers = 0;
typedef struct {
    int id;
    char username[255];
    int channelId;
} User;


int main(int argc, char *argv[]) {
    User users[MAX_USERS];

    currentUsers++;
    strcpy(users[0].username, "asdf");

    signal(SIGINT, terminateServer);

    messageQueueId = msgget(0x1234, IPC_CREAT | 0644);

    printfDebug("%d\n", messageQueueId);

    if (messageQueueId == -1) {
        printf("Error\n");
        return 0;
    }

    Request request;

    while (true) {

        int size = (int) msgrcv(messageQueueId, &request, REQUEST_SIZE, 0, 0);

        printfDebug("Received request of type %lu: %.*s\n", request.type, size, request.body);

        if (request.type == REQUEST_USERNAME) {
            printf("Username check %d, %d\n", currentUsers + 1, MAX_USERS);
            if (currentUsers + 1 < MAX_USERS) {
                bool isTaken = false;
                char *requestBody = malloc(sizeof(char) * size);
                snprintf(requestBody, size + 1, "%s", request.body);

                printf("Requested username: %s\n", requestBody);

                for (int i = 0; i < currentUsers; i++) {
                    printf("Checking user %s\n", users[i].username);


                    if (strcmp(users[i].username, requestBody) == 0) {
                        isTaken = true;
                        break;
                    }
                }

                Response response = {REQUEST_USERNAME};

                if (isTaken) {
                    sprintf(response.body, "taken");
                } else {
                    currentUsers++;
                    users[currentUsers].id = currentUsers;
                    sprintf(users[currentUsers].username, "%.*s", size, request.body);
                    sprintf(response.body, "%d", users[currentUsers].id);
                }

                unsigned long bodyLength = strlen(response.body);
                msgsnd(messageQueueId, &response, bodyLength, 0);
                printfDebug("Send message: %s with length %lu\n", response.body, bodyLength);
            } else {
                Response response = {REQUEST_USERNAME};
                sprintf(response.body, "%ld", LONG_MAX);
                msgsnd(messageQueueId, &response, strlen(response.body), 0);
            }

        }

        sleep(2);

    }

    return 0;
}

#pragma clang diagnostic pop