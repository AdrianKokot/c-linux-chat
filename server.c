#include <sys/msg.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>

int messageQueueId;

void terminateServer() {
    msgctl(messageQueueId,IPC_RMID,NULL);
}

int main(int argc, char *argv[]) {

    signal(SIGTERM, terminateServer);

    int messageQueueId = msgget(0x1234, IPC_CREAT | 0644);

    printf("%d\n", messageQueueId);

    while(true) {

    }

    return 0;
}