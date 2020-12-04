INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_NFC nfc)

FIND_PATH(
    NFC_INCLUDE_DIRS
    NAMES nfc/api.h
    HINTS $ENV{NFC_DIR}/include
        ${PC_NFC_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    NFC_LIBRARIES
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

include("${CMAKE_CURRENT_LIST_DIR}/nfcTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NFC DEFAULT_MSG NFC_LIBRARIES NFC_INCLUDE_DIRS)
MARK_AS_ADVANCED(NFC_LIBRARIES NFC_INCLUDE_DIRS)
