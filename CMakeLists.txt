cmake_minimum_required(VERSION 3.16)

project(ARITHMET VERSION 1.2.4 LANGUAGES C RC)

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
            arith.rc
    )
else()
    list(APPEND ARITHMET_SOURCES
            terminal_input_unix.c
            terminal_screen_unix.c
    )
endif()

add_executable(arithmet ${ARITHMET_SOURCES})

target_include_directories(arithmet PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(arithmet PRIVATE
    ARITHMET_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    ARITHMET_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    ARITHMET_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)
