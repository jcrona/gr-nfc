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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "modified_miller_decoder_impl.h"

#define MILLER_PULSE_DURATION				2,5 // us
#define MILLER_PULSE_WIDTH				((d_sample_rate/1000000) * MILLER_PULSE_DURATION)
#define MILLER_PULSE_WIDTH_MIN				(MILLER_PULSE_WIDTH - MILLER_PULSE_WIDTH/2)
#define MILLER_PULSE_WIDTH_MAX				(MILLER_PULSE_WIDTH + MILLER_PULSE_WIDTH/2)

#define MILLER_GAP_LONG_DURATION			16 // us
#define MILLER_GAP_MEDIUM_DURATION			11 // us
#define MILLER_GAP_SHORT_DURATION			6 // us
#define MILLER_GAP_LONG_WIDTH				((d_sample_rate/1000000) * MILLER_GAP_LONG_DURATION)
#define MILLER_GAP_MEDIUM_WIDTH				((d_sample_rate/1000000) * MILLER_GAP_MEDIUM_DURATION)
#define MILLER_GAP_SHORT_WIDTH				((d_sample_rate/1000000) * MILLER_GAP_SHORT_DURATION)
#define MILLER_GAP_START_WIDTH_THRESHOLD		(MILLER_GAP_LONG_WIDTH * 2)
#define MILLER_GAP_LONG_WIDTH_THRESHOLD			(MILLER_GAP_LONG_WIDTH - MILLER_GAP_LONG_WIDTH/8)
#define MILLER_GAP_MEDIUM_WIDTH_THRESHOLD		(MILLER_GAP_MEDIUM_WIDTH - MILLER_GAP_MEDIUM_WIDTH/8)
#define MILLER_GAP_SHORT_WIDTH_THRESHOLD		(MILLER_GAP_SHORT_WIDTH - MILLER_GAP_SHORT_WIDTH/8)

/* Enable this to display the decoding process */
//#define DEBUG


namespace gr {
  namespace nfc {

    enum miller_state {
        WAIT_FOR_START,
        LAST_BIT_ZERO_OR_START,
        LAST_BIT_ONE,
        END_OF_FRAME,
    };

    static enum miller_state current_state = WAIT_FOR_START;
    static unsigned int count_one = 0;
    static unsigned int count_zero = 0;
    static unsigned int decoded_bit_num = 0;
    static unsigned char current_frame[1000] = { 0 };

    modified_miller_decoder::sptr
    modified_miller_decoder::make(double sample_rate)
    {
      return gnuradio::get_initial_sptr
        (new modified_miller_decoder_impl(sample_rate));
    }

    /*
     * The private constructor
     */
    modified_miller_decoder_impl::modified_miller_decoder_impl(double sample_rate)
      : gr::block("modified_miller_decoder",
              gr::io_signature::make(1, 1, sizeof(char)),
              gr::io_signature::make(1, 1, sizeof(char))),
        d_sample_rate(sample_rate)
    {
#ifdef DEBUG
       std::cout << "MILLER_PULSE_WIDTH_MIN = " << MILLER_PULSE_WIDTH_MIN << std::endl;
       std::cout << "MILLER_PULSE_WIDTH_MAX = " << MILLER_PULSE_WIDTH_MAX << std::endl;
       std::cout << "MILLER_GAP_START_WIDTH_THRESHOLD = " << MILLER_GAP_START_WIDTH_THRESHOLD << std::endl;
       std::cout << "MILLER_GAP_SHORT_WIDTH_THRESHOLD = " << MILLER_GAP_SHORT_WIDTH_THRESHOLD << std::endl;
       std::cout << "MILLER_GAP_MEDIUM_WIDTH_THRESHOLD = " << MILLER_GAP_MEDIUM_WIDTH_THRESHOLD << std::endl;
       std::cout << "MILLER_GAP_LONG_WIDTH_THRESHOLD = " << MILLER_GAP_LONG_WIDTH_THRESHOLD << std::endl;
#endif
    }

    /*
     * Our virtual destructor.
     */
    modified_miller_decoder_impl::~modified_miller_decoder_impl()
    {
    }

    void
    modified_miller_decoder_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
        ninput_items_required[0] = (noutput_items * 8 * d_sample_rate)/1000000;
    }

    static unsigned char
    compute_even_parity (unsigned char c)
    {
            unsigned int i;
            unsigned char parity = 0;

            for (i = 0; i < 8; i++) {
                parity ^= (c & (0x1 << i)) >> i;
            }

            return parity;
    }

    static void
    set_next_bit (unsigned char bit)
    {
        if (bit) {
            /* 1 */
            current_frame[decoded_bit_num] = 1;
        } else {
            /* 0 */
            current_frame[decoded_bit_num] = 0;
        }

        decoded_bit_num++;
    }

    static unsigned char
    get_bit (unsigned int n)
    {
        return current_frame[n];
    }

    static unsigned char
    get_last_bit (void)
    {
        return get_bit(decoded_bit_num - 1);
    }

    static void
    remove_last_bit (void)
    {
            decoded_bit_num--;
            current_frame[decoded_bit_num] = 0;
    }

    static void
    clear_frame (void)
    {
            memset(current_frame, 0, sizeof(current_frame));
            decoded_bit_num = 0;
    }

    int
    modified_miller_decoder_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
        const unsigned char *in = (const unsigned char *) input_items[0];
        unsigned char *out = (unsigned char *) output_items[0];
        int decoded_bytes_num = 0;
        unsigned char no_parity_mode = 0;

        /* The modified Miller code (LSB first) is :
         *  - Start -> _---
         *  - 1 -> --_-
         *  - 0 -> _--- if the previous bit was 0 or a Start
         *  - 0 -> ---- if the previous bit was 1
         *  - End -> logical 0 then ----
         * 
         * In order to ease the decoding work, we will measure the time between one pulse and the next one,
         * using this equivalent table of truth :
         *  - Short Gap --> 0 or a Start if the previous bit was 0, a Start, or nothing
         *  - Short Gap --> 1 if the previous bit was 1
         *  - Medium Gap --> 1 if the previous bit was 0 or a Start
         *  - Medium Gap --> 00 if the previous bit was 1
         *  - Long Gap --> 01 (the previous bit is always 1)
         */

        //std::cout << "noutput_items " << noutput_items << ", ninput_items[0] " << ninput_items[0] << std::endl;
        for (int i = 0; i < ninput_items[0]; i++) {
            if (in[i] > 0) {
                if (count_zero > 0) {
                    /* A valid pulse is 2,5us +-50% */
                    if (count_zero >= MILLER_PULSE_WIDTH_MIN && count_zero <= MILLER_PULSE_WIDTH_MAX) {
                        /* Rising edge (end of pulse), lookup the previous bit(s) */

                        if (count_one > MILLER_GAP_START_WIDTH_THRESHOLD) {
                            if (current_state == WAIT_FOR_START) {
                                /* This is the first pulse of a frame (START) */
                                current_state = LAST_BIT_ZERO_OR_START;
#ifdef DEBUG
                                std::cout << "    Start" << std::endl;
#endif
                            }
                        } else if (count_one > MILLER_GAP_LONG_WIDTH_THRESHOLD) {
                            if (current_state == LAST_BIT_ONE) {
                                /* 01 */
                                set_next_bit(0);
                                set_next_bit(1);
#ifdef DEBUG
                                std::cout << "    01 (Long)" << std::endl;
#endif

                                current_state = LAST_BIT_ONE;
                            } else if (current_state == LAST_BIT_ZERO_OR_START) {
                                /* Invalid */
#ifdef DEBUG
                                std::cout << "    Invalid (Long after 0)" << std::endl;
#endif
                                current_state = END_OF_FRAME;
                            }
                        } else if (count_one > MILLER_GAP_MEDIUM_WIDTH_THRESHOLD) {
                            if (current_state == LAST_BIT_ONE) {
                                /* 00 */
                                set_next_bit(0);
                                set_next_bit(0);
#ifdef DEBUG
                                std::cout << "    00 (Medium)" << std::endl;
#endif

                                current_state = LAST_BIT_ZERO_OR_START;
                            } else if (current_state == LAST_BIT_ZERO_OR_START) {
                                /* 1 */
                                set_next_bit(1);
#ifdef DEBUG
                                std::cout << "    1 (Medium)" << std::endl;
#endif

                                current_state = LAST_BIT_ONE;
                            }
                        } else if (count_one > MILLER_GAP_SHORT_WIDTH_THRESHOLD) {
                            if (current_state == LAST_BIT_ONE) {
                                /* 1 */
                                set_next_bit(1);
#ifdef DEBUG
                                std::cout << "    1 (Short)" << std::endl;
#endif

                                current_state = LAST_BIT_ONE;
                            } else if (current_state == LAST_BIT_ZERO_OR_START) {
                                /* 0 */
                                set_next_bit(0);
#ifdef DEBUG
                                std::cout << "    0 (Short)" << std::endl;
#endif

                                current_state = LAST_BIT_ZERO_OR_START;
                            }
                        } else {
                            /* Shorter gaps (invalid)
                             * If this is noise somehow, it means that a valid pulse has been
                             * successfully divided into two valid pulses ! This should not happen,
                             * but if it does, let's stop the frame.
                             */
#ifdef DEBUG
                            std::cout << "    Invalid (Gap too short)" << std::endl;
#endif
                            current_state = END_OF_FRAME;
                        }

                        count_one = 0;
                    } else if (count_zero < MILLER_PULSE_WIDTH_MIN) {
                        /* Consider the zeros as ones (noise) */
                        count_one += count_zero;
                    }

                    count_zero = 0;
                }

                count_one++;

                if (current_state != WAIT_FOR_START && decoded_bit_num > 0) {
                    if (get_last_bit()) {
                        if (count_one > MILLER_GAP_START_WIDTH_THRESHOLD) {
                            /* End of frame */
#ifdef DEBUG
                            std::cout << "    End" << std::endl;
#endif
                            current_state = END_OF_FRAME;
                        }
                    } else {
                        if (count_one > MILLER_GAP_LONG_WIDTH_THRESHOLD) {
                            /* End of frame */
                            /* Remove the last 0 which is part of the end marker */
                            remove_last_bit();
#ifdef DEBUG
                            std::cout << "    End (-0)" << std::endl;
#endif
                            current_state = END_OF_FRAME;
                        }
                    }
                }
            } else {
                count_zero++;
            }

            if (current_state == END_OF_FRAME) {
                unsigned char tmp_byte = 0;
                unsigned int in_bit = 0, out_bit = 0;
                unsigned char parity_ok;

                if (decoded_bit_num > 0) {
                    /* Assume that the frame is in no parity mode if its length
                     * is valid in no parity mode, and not in Standard mode.
                     * NOTE: For a length of 9x8xN, the last known mode is used.
                     */
                    if ((decoded_bit_num % 72) != 0) {
                        no_parity_mode = (((decoded_bit_num % 9) != 0) && ((decoded_bit_num % 8) == 0));
                    }

                    /* Decode and print the frame */
                    printf("Reader ->");
#ifdef DEBUG
                    printf(" ");
#endif
                    while (in_bit < decoded_bit_num) {
                        out[decoded_bytes_num] |= get_bit(in_bit) << out_bit;
#ifdef DEBUG
                        printf("%u", get_bit(in_bit));
#endif
                        in_bit++;
                        out_bit++;

                        if (!no_parity_mode && (out_bit == 8 && in_bit < decoded_bit_num)) {
                            /* Check parity if needed */
                            parity_ok = (compute_even_parity(out[decoded_bytes_num]) == !get_bit(in_bit));
#ifdef DEBUG
                            printf("-%u", get_bit(in_bit));
#endif
                            in_bit++;
                        }

                        if (out_bit == 8 || in_bit == decoded_bit_num) {
                            /* Print the byte */
                            if (decoded_bit_num == 7) {
                                /* Short command */
                                printf(" [%02X]", out[decoded_bytes_num]);
                            } else if (out_bit < 8 || (decoded_bit_num == 8 && !no_parity_mode)) {
                                /* Broken */
                                printf(" /%02X\\", out[decoded_bytes_num]);
                            } else if (parity_ok || no_parity_mode) {
                                printf("  %02X ", out[decoded_bytes_num]);
                            } else {
                                printf(" (%02X)", out[decoded_bytes_num]);
                            }
#ifdef DEBUG
                            printf(" ");
#endif

                            decoded_bytes_num++;
                            out_bit = 0;
                        }
                    }

                    if (no_parity_mode) {
                        printf(" (No parity)");
                    }

                    printf("\n");
                    clear_frame();
                }

                current_state = WAIT_FOR_START;
            }
        }

        consume_each (ninput_items[0]);

        // Tell runtime system how many output items we produced.
        return decoded_bytes_num;
    }
  } /* namespace nfc */
} /* namespace gr */

