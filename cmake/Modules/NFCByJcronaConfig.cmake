INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_NFCBYJCRONA NFCByJcrona)

FIND_PATH(
    NFCBYJCRONA_INCLUDE_DIRS
    NAMES NFCByJcrona/api.h
    HINTS $ENV{NFCBYJCRONA_DIR}/include
        ${PC_NFCBYJCRONA_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    NFCBYJCRONA_LIBRARIES
    NAMES gnuradio-NFCByJcrona
    HINTS $ENV{NFCBYJCRONA_DIR}/lib
        ${PC_NFCBYJCRONA_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/NFCByJcronaTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NFCBYJCRONA DEFAULT_MSG NFCBYJCRONA_LIBRARIES NFCBYJCRONA_INCLUDE_DIRS)
MARK_AS_ADVANCED(NFCBYJCRONA_LIBRARIES NFCBYJCRONA_INCLUDE_DIRS)
