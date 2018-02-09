include(CreateDirectoryGroups)

add_library(teakra
    ../include/teakra/teakra.h
    common_types.h
    core.cpp
    core.h
    decoder.h
    disassembler.h
    icu.h
    interpreter.h
    matcher.h
    memory_interface.cpp
    memory_interface.h
    mmio.cpp
    mmio.h
    oprand.h
    register.h
    shared_memory.h
    teakra.cpp
)

create_target_directory_groups(teakra)

target_link_libraries(teakra PRIVATE Threads::Threads)
target_include_directories(teakra
                           PUBLIC ../include
                           PRIVATE .)
target_compile_options(teakra PRIVATE ${TEAKRA_CXX_FLAGS})

#####
add_executable(teakra_tests
    main.cpp
)
create_target_directory_groups(teakra_tests)
target_link_libraries(teakra_tests PRIVATE teakra)
target_include_directories(teakra_tests PRIVATE .)
target_compile_options(teakra_tests PRIVATE ${TEAKRA_CXX_FLAGS})
