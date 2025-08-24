include(CreateDirectoryGroups)

add_executable(test_verifier
    main.cpp
)
create_target_directory_groups(test_verifier)
target_link_libraries(test_verifier PRIVATE teakra)
target_include_directories(test_verifier PRIVATE .)
target_include_directories(test_verifier PRIVATE ../../include/teakra/impl/)
target_compile_options(test_verifier PRIVATE ${TEAKRA_CXX_FLAGS})


# Test related stuff
set(ASSET_SHA256SUM "baffcd4f805a7480d969401792443a34aa39f813b4f0ae49c6365f1d1f3ce120")
if(TEAKRA_RUN_TESTS)
  message(STATUS "Will run Teakra accuracy tests")
  # check if provided fixtures are good
  file(SHA256 "${TEAKRA_TEST_ASSETS_DIR}/teaklite2_tests_result.bin" ASSET_CHECKSUM)
  if(ASSET_SHA256SUM STREQUAL ASSET_CHECKSUM)
    message(STATUS "Unit test sample looks good.")
  else()
    message(FATAL_ERROR "Unit test sample broken. Please download the file using: git lfs pull --exclude='' --include='*'")
  endif()

  add_test(NAME tests COMMAND test_verifier "${TEAKRA_TEST_ASSETS_DIR}/teaklite2_tests_result.bin")
endif(TEAKRA_RUN_TESTS)
