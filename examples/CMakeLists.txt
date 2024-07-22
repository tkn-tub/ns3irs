build_lib_example(
    NAME irs-compare-matlab
    SOURCE_FILES irs-compare-matlab.cc
    LIBRARIES_TO_LINK
        ${libirs}
        ${libwifi}
        ${libinternet}
      # EXECUTABLE_DIRECTORY_PATH ${scratch_directory}/
)
