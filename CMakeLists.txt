cmake_minimum_required(VERSION 3.16)

set(ARITHMET_VERSION 1.3.0)

if (WIN32)
    project(ARITHMET VERSION ${ARITHMET_VERSION} LANGUAGES C RC)
else()
    project(ARITHMET VERSION ${ARITHMET_VERSION} LANGUAGES C)
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(ARITHMET_SOURCES
        main.c
        app_state.c
        input.c
        protocol.c
        screens.c
        session.c
        ui_helpers.c
        conio.c
        conio_state.c
        conio_terminal.c
        asm.c
        keyboard_mapping.c
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
