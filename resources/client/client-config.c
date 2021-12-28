#include "client-config.h"
#include <stdlib.h>

struct clientConfig ClientConfig = {
        {
                "/h, /help",  "Prints available commands",
                "/e, /exit",             "Exits the app\n",
                NULL
        },
        {
                "-h, --help", "Prints help",
                "-u, --username <name>", "Starts app with given username",
                "-s, --server <id>",
                "Starts app with connection to server with given id (should be hex without '0x')\n",
                NULL
        }
};

struct clientMessages Messages = {
        "Type '/help' to get list of commands available for you.\n",
        "Please enter server id to connect (hex without '0x'): ",
        "Given server doesn't exist. Please enter server id to connect (hex without '0x'): ",
        "The given server is full. Please enter different server id to connect (hex without '0x'): ",
        "The given username is already taken.\n",
        "Please enter your username: ",
        "The given username is already taken. Please enter different username: ",
        "You are not connected to any channel."
};

struct clientCliCommandSignatures CliCommands = {
        {"-u", "--username"},
        {"-s", "--server"},
        {"-h", "--help"}
};
struct appCommandSignatures AppCommands = {
        {"/h", "/help"},
        {"/e", "/exit"}
};