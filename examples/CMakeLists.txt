build_lib_example(
    NAME irs-compare-matlab
    SOURCE_FILES irs-compare-matlab.cc
    LIBRARIES_TO_LINK
        ${libirs}
        ${libwifi}
        ${libinternet}
      # EXECUTABLE_DIRECTORY_PATH ${scratch_directory}/
)

build_lib_example(
    NAME irs-validate-changing-sizes
    SOURCE_FILES irs-validate-changing-sizes.cc
    LIBRARIES_TO_LINK
        ${libirs}
        ${libwifi}
        ${libinternet}
      # EXECUTABLE_DIRECTORY_PATH ${scratch_directory}/
)

build_lib_example(
    NAME irs-validation
    SOURCE_FILES irs-validation.cc
    LIBRARIES_TO_LINK
        ${libirs}
        ${libwifi}
        ${libinternet}
        ${libapplications}
        ${libflow-monitor}
      # EXECUTABLE_DIRECTORY_PATH ${scratch_directory}/
)
