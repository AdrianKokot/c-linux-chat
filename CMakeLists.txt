cmake_minimum_required(VERSION 3.21)
project(put_psiw C)

set(CMAKE_C_STANDARD 11)

#file( GLOB SOURCES *.c)
#foreach ( source ${SOURCES} )
#    string( REPLACE ".c" "" name ${source} )
#    string( REPLACE "/mnt/c/git/put-psiw/" "" name ${name} )
#    add_executable( ${name} ${source} shared/cli-helper.c shared/cli-helper.h shared/communication-structs.c shared/communication-structs.h shared/shared.c shared/shared.h )
#
#endforeach ( source ${SOURCES} )

add_executable( client client.c shared/cli-helper.c shared/cli-helper.h shared/communication-structs.c shared/communication-structs.h shared/shared.c shared/shared.h resources/client/client-lib.c resources/client/client-lib.h resources/client/client-config.c resources/client/client-config.h shared/utils.c shared/utils.h)

add_executable( server server.c shared/cli-helper.c shared/cli-helper.h shared/communication-structs.c shared/communication-structs.h shared/shared.c shared/shared.h shared/utils.c shared/utils.h resources/server/server-lib.c resources/server/server-lib.h resources/server/server-config.c resources/server/server-config.h)