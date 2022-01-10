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
        "\033[33mList of users:\033[m\n",
        "The channel name should be between 4 and " STR(MAX_CHANNEL_NAME) " characters long.",
        "The message should be between 2 and " STR(MESSAGE_MAX_SIZE) " characters long.",
        "Couldn't send the message.",
        "You can't send message to yourself.",
        "The username should be between 4 and " STR(MAX_USERNAME) " characters."
};
