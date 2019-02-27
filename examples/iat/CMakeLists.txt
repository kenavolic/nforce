set (TARGET_NAME iat)

add_executable(${TARGET_NAME} main.cpp)
set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "examples")
target_link_libraries(${TARGET_NAME} ${NFORCE_LIB})