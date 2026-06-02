cmake_minimum_required(VERSION 3.16)

project(ARITHMET C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(ARITHMET_SOURCES
        arith.c
        conio.c
        asm.c
        mouse.c
        dos_compat.c
)

if (WIN32)
    list(APPEND ARITHMET_SOURCES
            terminal_input_win.c
            terminal_screen_win.c
    )
else()
    list(APPEND ARITHMET_SOURCES
            terminal_input_unix.c
            terminal_screen_unix.c
    )
endif()

set_source_files_properties(${ARITHMET_SOURCES} PROPERTIES LANGUAGE C)

add_executable(arithmet ${ARITHMET_SOURCES})

target_include_directories(arithmet PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
