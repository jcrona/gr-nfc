find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_NFC gnuradio-nfc)

FIND_PATH(
    GR_NFC_INCLUDE_DIRS
    NAMES gnuradio/nfc/api.h
    HINTS $ENV{NFC_DIR}/include
        ${PC_NFC_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_NFC_LIBRARIES
    NAMES gnuradio-nfc
    HINTS $ENV{NFC_DIR}/lib
        ${PC_NFC_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-nfcTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_NFC DEFAULT_MSG GR_NFC_LIBRARIES GR_NFC_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_NFC_LIBRARIES GR_NFC_INCLUDE_DIRS)
