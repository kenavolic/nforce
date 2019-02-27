# gtest
if (${NFORCE_BUILD_TESTS})
    set(gtest_SOURCE_DIR ${PROJECT_SOURCE_DIR}/third_party/gtest/googletest)
    add_subdirectory(gtest)
    set_target_properties(gtest PROPERTIES FOLDER "third_party")
    set_target_properties(gtest_main PROPERTIES FOLDER "third_party")

    foreach (tgt gtest gtest_main)
        target_compile_options(${tgt} PUBLIC "${NFORCE_COMPILE_OPTIONS}")
        target_compile_options(${tgt} PUBLIC "$<$<CONFIG:DEBUG>:${NFORCE_TEST_COMPILE_OPTIONS_DEBUG}>")
        target_compile_options(${tgt} PUBLIC "$<$<CONFIG:RELEASE>:${NFORCE_TEST_COMPILE_OPTIONS_RELEASE}>")
    endforeach()
endif()