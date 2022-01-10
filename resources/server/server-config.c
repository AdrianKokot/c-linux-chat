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
        "\033[31mGiven queue is already taken.\033[m",
        "\033[31mThe given channel doesn't exist.\033[m",
        "\033[31mThe given user doesn't exist.\033[m",
        "\033[31mThe given name is already taken.\033[m",
        "\033[31mServer is full.\033[m",
        "\033[31mThere's no available channels.\033[m",
        "\033[33mAvailable channels:\033[m\n",
        "\033[33mList of users:\033[m\n",
        "\033[31mThe channel name should be between 4 and " STR(MAX_CHANNEL_NAME) " characters long.\033[m",
        "\033[31mThe message should be between 2 and " STR(MAX_MESSAGE_SIZE) " characters long.\033[m",
        "\033[31mCouldn't send the message.\033[m",
        "\033[31mYou can't send message to yourself.\033[m",
        "\033[31mThe username should be between 4 and " STR(MAX_USERNAME) " characters.\033[m",
        "\033[31mType '/help' to get list of commands available for you.\n\033[m",
        "\033[31mPlease enter server id to connect (hex without '0x'): \033[m",
        "\033[31mGiven server doesn't exist. Please enter server id to connect (hex without '0x'): \033[m",
        "\033[31mThe given server is full. Please enter different server id to connect (hex without '0x'): \033[m",
        "\033[31mPlease enter your username: \033[m",
        "\033[31mThe given username is already taken. Please enter different username: \033[m",
        "\033[31mYou are not connected to any channel.\033[m",
        "\033[31mThe server has maximum number of channels. Cannot create a new one.\033[m",
        "Successfully created channel",
        "\033[33mSuccessfully left channel.\033[m"
};
