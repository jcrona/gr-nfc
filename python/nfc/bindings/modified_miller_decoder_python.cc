/*
 * Copyright 2022 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(modified_miller_decoder.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(d8b5ebc9b48097a09f5ee88266b100f8)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/nfc/modified_miller_decoder.h>
// pydoc.h is automatically generated in the build directory
#include <modified_miller_decoder_pydoc.h>

void bind_modified_miller_decoder(py::module& m)
{

    using modified_miller_decoder    = ::gr::nfc::modified_miller_decoder;


    py::class_<modified_miller_decoder, gr::block, gr::basic_block,
        std::shared_ptr<modified_miller_decoder>>(m, "modified_miller_decoder", D(modified_miller_decoder))

        .def(py::init(&modified_miller_decoder::make),
           py::arg("sample_rate"),
           D(modified_miller_decoder,make)
        )
        



        ;




}







