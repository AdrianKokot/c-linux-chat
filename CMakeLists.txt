cmake_minimum_required(VERSION 3.21)
project(put_psiw C)

set(CMAKE_C_STANDARD 11)

file( GLOB SOURCES *.c)
foreach ( source ${SOURCES} )
    string( REPLACE ".c" "" name ${source} )
    string( REPLACE "/mnt/c/git/put-psiw/" "" name ${name} )
    add_executable( ${name} ${source} shared/cli-helper.c shared/cli-helper.h shared/communication-structs.c shared/communication-structs.h )

endforeach ( source ${SOURCES} )