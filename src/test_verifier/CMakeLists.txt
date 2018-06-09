include(CreateDirectoryGroups)

add_executable(test_verifier
    main.cpp
)
create_target_directory_groups(test_verifier)
target_link_libraries(test_verifier PRIVATE teakra)
target_include_directories(test_verifier PRIVATE .)
target_compile_options(test_verifier PRIVATE ${TEAKRA_CXX_FLAGS})
