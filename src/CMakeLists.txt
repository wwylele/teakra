include(CreateDirectoryGroups)

add_library(teakra
    ../include/teakra/disassembler.h
    ../include/teakra/teakra.h
    apbp.cpp
    apbp.h
    common_types.h
    core.cpp
    core.h
    crash.h
    decoder.h
    disassembler.cpp
    timer.cpp
    timer.h
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
    test.h
    test_generator.cpp
    test_generator.h
)

create_target_directory_groups(teakra)

target_link_libraries(teakra PRIVATE Threads::Threads)
target_include_directories(teakra
                           PUBLIC ../include
                           PRIVATE .)
target_compile_options(teakra PRIVATE ${TEAKRA_CXX_FLAGS})

add_subdirectory(coff_reader)
add_subdirectory(dsp1_reader)
add_subdirectory(test_generator)
add_subdirectory(test_verifier)
add_subdirectory(mod_test_generator)
add_subdirectory(step2_test_generator)
