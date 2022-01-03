#include "server-config.h"

struct serverCliCommandSignatures CliCommands = {
        {"-i", "--id"},
        {"-h", "--help"}
};

struct serverConfig ServerConfig = {
        {
                "-h, --help", "Prints help",
                "-i, --id <name>", "Starts server with given queue id",
                NULL
        }
};

struct serverMessages Messages = {
        "Please enter queue id for the server to listen to (hex without '0x'): ",
        "Given queue is already taken.",
        "The given channel doesn't exist.",
        "The given user doesn't exist.",
        "The given name is already taken.",
        "Server is full.",
        "There's no available channels.\n",
        "\033[33mAvailable channels:\033[m\n",
        "\033[33mThere's no users.\033[m\n",
        "\033[33mList of users:\033[m\n",
        "Channel name should have at least 4 characters.",
        "The message cannot be empty.",
        "Couldn't send the message.",
        "You can't send message to yourself."
};
