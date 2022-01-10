cmake_minimum_required(VERSION 3.21)
project(put_psiw C)

set(CMAKE_C_STANDARD 11)

add_executable( client client.c shared/communication-structs.c shared/communication-structs.h shared/shared.h resources/client/client-lib.c resources/client/client-lib.h resources/client/client-config.c resources/client/client-config.h shared/utils.c shared/utils.h)

add_executable( server server.c shared/communication-structs.c shared/communication-structs.h shared/shared.h shared/utils.c shared/utils.h resources/server/server-lib.c resources/server/server-lib.h resources/server/server-config.c resources/server/server-config.h)