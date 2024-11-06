check_include_file_cxx(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)
if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        # test/irs-examples-test-suite.cc
        )
endif()

build_lib(
    LIBNAME irs
    SOURCE_FILES model/irs-model.cc
                 model/irs-lookup-model.cc
                 model/irs-spectrum-model.cc
                 helper/irs-lookup-helper.cc
                 helper/irs-lookup-table.cc
                 model/irs-propagation-loss-model.cc
    HEADER_FILES model/irs-model.h
                 model/irs-lookup-model.h
                 model/irs-spectrum-model.h
                 helper/irs-lookup-helper.h
                 helper/irs-lookup-table.h
                 model/irs-propagation-loss-model.h
    LIBRARIES_TO_LINK
        ${libpropagation}
    TEST_SOURCES test/irs-propagation-loss-model-test-suite.cc
                 test/irs-spectrum-model-test-suite.cc
                 ${examples_as_tests_sources}
)
