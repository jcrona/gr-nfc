/* -*- c++ -*- */
/* 
 * Copyright 2017 Jean-Christophe Rona <jc@rona.fr>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_NFC_MODIFIED_MILLER_DECODER_IMPL_H
#define INCLUDED_NFC_MODIFIED_MILLER_DECODER_IMPL_H

#include <gnuradio/nfc/modified_miller_decoder.h>

namespace gr {
  namespace nfc {

    class modified_miller_decoder_impl : public modified_miller_decoder
    {
     private:
      double d_sample_rate;

     public:
      modified_miller_decoder_impl(double sample_rate);
      ~modified_miller_decoder_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
    };

  } // namespace nfc
} // namespace gr

#endif /* INCLUDED_NFC_MODIFIED_MILLER_DECODER_IMPL_H */

