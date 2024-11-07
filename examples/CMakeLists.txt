build_lib_example(
    NAME irs-compare-matlab
    SOURCE_FILES irs-compare-matlab.cc
    LIBRARIES_TO_LINK
        ${libirs}
        ${libwifi}
        ${libinternet}
)

build_lib_example(
    NAME irs-validate-changing-sizes
    SOURCE_FILES irs-validate-changing-sizes.cc
    LIBRARIES_TO_LINK
        ${libirs}
        ${libwifi}
        ${libinternet}
)

build_lib_example(
    NAME irs-optimal-placement
    SOURCE_FILES irs-optimal-placement.cc
    LIBRARIES_TO_LINK
        ${libirs}
        ${libwifi}
        ${libinternet}
)

build_lib_example(
    NAME irs-validation
    SOURCE_FILES irs-validation.cc
    LIBRARIES_TO_LINK
        ${libirs}
        ${libwifi}
        ${libinternet}
        ${libapplications}
)

build_lib_example(
    NAME irs-energy
    SOURCE_FILES irs-energy.cc
    LIBRARIES_TO_LINK
        ${libirs}
        ${libwifi}
        ${libinternet}
        ${libapplications}
        ${libenergy}
)

build_lib_example(
    NAME irs-mobility
    SOURCE_FILES irs-mobility.cc
    LIBRARIES_TO_LINK
        ${libirs}
        ${libwifi}
        ${libinternet}
        ${libapplications}
)

build_lib_example(
    NAME hidden-node-problem
    SOURCE_FILES hidden-node-problem.cc
    LIBRARIES_TO_LINK
        ${libirs}
        ${libwifi}
        ${libinternet}
        ${libapplications}
        ${libflow-monitor}
)

build_lib_example(
    NAME irs-rem
    SOURCE_FILES irs-rem.cc
    LIBRARIES_TO_LINK
        ${libirs}
        ${libwifi}
        ${liblte}
)

build_lib_example(
    NAME generate-irs-lookuptable
    SOURCE_FILES generate-irs-lookuptable.cc
    LIBRARIES_TO_LINK
        ${libirs}
)
