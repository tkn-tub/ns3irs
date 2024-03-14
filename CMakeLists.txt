check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/irs-examples-test-suite.cc
        )
endif()

build_lib(
    LIBNAME irs
    SOURCE_FILES model/irs.cc
                 helper/irs-helper.cc
    HEADER_FILES model/irs.h
                 helper/irs-helper.h
    LIBRARIES_TO_LINK ${libcore}
    TEST_SOURCES test/irs-test-suite.cc
                 ${examples_as_tests_sources}
)
