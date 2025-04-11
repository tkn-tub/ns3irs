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
    NAME irs-multiuser
    SOURCE_FILES irs-multiuser.cc
    LIBRARIES_TO_LINK
        ${libirs}
        ${libwifi}
        ${libinternet}
        ${libapplications}
)
