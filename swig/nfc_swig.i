/* -*- c++ -*- */

#define NFC_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "nfc_swig_doc.i"

%{
#include "nfc/modified_miller_decoder.h"
%}

%include "nfc/modified_miller_decoder.h"
GR_SWIG_BLOCK_MAGIC2(nfc, modified_miller_decoder);
