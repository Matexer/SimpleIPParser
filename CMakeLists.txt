cmake_minimum_required(VERSION 3.5)

project(SimpleIPConverter LANGUAGES C)

add_executable(SimpleIPConverter
    structs.h
    utils.h
    fileManagement.h
    Converting.h
    main.c)

install(TARGETS SimpleIPConverter
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

target_compile_options(SimpleIPConverter PRIVATE -O3)

target_link_libraries(SimpleIPConverter PRIVATE -static)
