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
        "Available channels:\n"
};
