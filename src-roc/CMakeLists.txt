
# Add CPP Check
include(CppcheckTargets)
add_cppcheck_sources(test UNUSED_FUNCTIONS STYLE POSSIBLE_ERRORS FORCE)

file(
    GLOB
    headers
    *.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/*.h
)

file(
    GLOB
    sources
    *.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc/*.c
)

add_executable(
    roc2310
    ${sources}
    ${headers}
)
target_link_libraries(roc2310 m)
set_target_properties(roc2310 PROPERTIES LINKER_LANGUAGE C)

install(
  TARGETS roc2310
    DESTINATION lib
)

install(
    FILES ${headers}
    DESTINATION include/${CMAKE_PROJECT_NAME}
)
