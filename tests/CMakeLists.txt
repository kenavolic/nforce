set (TARGET_NAME ${NFORCE_LIB}_test)

set (NFORCE_TST
    expr_test.cpp
    lexer_test.cpp
    parser_test.cpp
)

create_test_sourcelist( 
    ${TARGET_NAME}
    nforce_test_driver.cpp
    ${NFORCE_TST}
)

add_executable(${TARGET_NAME} nforce_test_driver.cpp ${NFORCE_TST})
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "tests")
target_link_libraries(${TARGET_NAME} ${NFORCE_LIB} gtest gtest_main)
target_compile_options(${TARGET_NAME} PUBLIC "${NFORCE_COMPILE_OPTIONS}")

foreach(TST ${NFORCE_TST})
    get_filename_component(TNAME ${TST} NAME_WE)
    add_test(NAME ${TNAME} COMMAND ${TARGET_NAME} ${TNAME})
endforeach()