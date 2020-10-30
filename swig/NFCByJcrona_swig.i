/* -*- c++ -*- */

#define NFCBYJCRONA_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "NFCByJcrona_swig_doc.i"

%{
#include "NFCByJcrona/nfc.h"
%}

%include "NFCByJcrona/nfc.h"
GR_SWIG_BLOCK_MAGIC2(NFCByJcrona, nfc);
