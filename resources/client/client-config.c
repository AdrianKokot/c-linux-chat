#include "client-config.h"
#include <stdlib.h>

struct clientConfig ClientConfig = {
        {
                "/h, /help",  "Prints available commands",
                "/ch, /channels", "Get list of available channels",
                "/j, /join <name>", "Join a channel with the given name",
                "/c, /create <name>", "Create a channel with the given name",
                "/l, /leave", "Leave the currently connected channel",
                "/e, /exit",             "Exits the app\n",
                NULL
        },
        {
                "-h, --help", "Prints help",
                "-u, --username <name>", "Starts app with the given username",
                "-s, --server <id>",
                "Starts app with connection to a server with the given id (should be hex without '0x')\n",
                NULL
        }
};

struct clientMessages Messages = {
        "Type '/help' to get list of commands available for you.\n",
        "Please enter server id to connect (hex without '0x'): ",
        "Given server doesn't exist. Please enter server id to connect (hex without '0x'): ",
        "The given server is full. Please enter different server id to connect (hex without '0x'): ",
        "Please enter your username: ",
        "The given username is already taken. Please enter different username: ",
        "You are not connected to any channel.",
        "The given channel name is already taken.",
        "The server has maximum number of channels. Cannot create a new one.",
        "Successfully created channel",
        "Channel with the given name doesn't exist.",
        "Successfully joined the channel. ",
        "List of users:\n",
        "List of users on this channel:\n"
};

struct clientCliCommandSignatures CliCommands = {
        {"-u", "--username"},
        {"-s", "--server"},
        {"-h", "--help"}
};
struct appCommandSignatures AppCommands = {
        {"/h", "/help"},
        {"/e", "/exit"},
        {"/ch", "/channels"},
        {"/j", "/join"},
        {"/c", "/create"},
        {"/l", "/leave"}
};