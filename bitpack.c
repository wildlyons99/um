/* bitpack.c
 * Authors: Tom Lyons (tlyons01) 
 *          Noah Stiegler (nstieg01)
 * Date:    10/24/2023
 * CS40 Project 4 Arith
 * Tufts University
 * 
 * Provides functionality for performing bitpacking operations on a 64 bit 
 * word. Clients can tell whether a given value can be represented in an
 * a signed or unsigned number of width bits, read the value of specified bits
 * within a word, and update the value of specified bits within a word. 
 *
 * The file also defines static helper implementation functions to perfrom 
 * left shift, right arithmatic shift, and right logical shift operations 
 *
 */ 


/* C Std Libs*/
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

/* Hanson Libs */
#include <assert.h>
#include <except.h>

/* Our Libs */
#include <bitpack.h>

/* Some useful macros for readability */
#define and &&

/* We support 64 bit words */
static const unsigned WORD_LEN = 64;

/* Static helper function definitions */
static uint64_t shift_left(uint64_t word, unsigned shift);
static uint64_t shift_right_l(uint64_t word, unsigned shift);
static int64_t shift_right_a(int64_t word, unsigned shift);

/* Define the exception we throw if trying to put a number which is too large
 * in too few bits */
Except_T Bitpack_Overflow = { "Overflow packing bits" };


/* Bitpack_fitsu
 * Purpose:
 *      Tell whether the argument n can be represented in an unsigned number 
 *      of width bits
 * Arguments: 
 *      (uint64_t) n:     The unsigned integer to try to fit in width bits
 *      (unsigned) width: The number of bits to represent n in
 * Returns:
 *      (bool) Whether or not n fits in width bits
 * Notes:
 *      - CRE if width > 64 (only support up to 64 bit words)
 *      - As no information can be represented with 0 bits, trying to represent
 *        any number in 0 bits (width of 0) returns false
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert(width <= WORD_LEN); 

        /* Can't represent anything in 0 bits */
        if (width == 0) {
                return false;
        }
        
        /* The largest unsigned number in width bits is 2^width - 1 */
        uint64_t maxval = shift_left(1, width) - 1;
        return (maxval >= n); 
}


/* Bitpack_fitss
 * Purpose:
 *      Tell whether the argument n can be represented in an signed number 
 *      of width bits
 * Arguments: 
 *      (uint64_t) n:     The signed integer to try to fit in width bits
 *      (unsigned) width: The number of bits to represent n in
 * Returns:
 *      (bool) Whether or not n fits in width bits
 * Notes:
 *      - CRE if width > 64 (only support up to 64 bit words)
 *      - As no information can be represented with 0 bits, trying to represent
 *        any number in 0 bits (width of 0) returns false
 */
bool Bitpack_fitss(int64_t n, unsigned width) 
{
        assert(width <= WORD_LEN);

        /* Can't represent anything in 0 or 1 bits */
        if (width == 0) {
                return false;
        }
        
        /* For a signed num, the lowest num which can fit in w bits is -2^(w-1) 
         * and the highest is 2^(w-1) - 1 */
        int64_t pos_limit = shift_left(1, width - 1) - 1;
        int64_t neg_limit = -1 * shift_left(1, width - 1);
        return (neg_limit <= n) and (n <= pos_limit);
}


/* Bitpack_getu
 * Purpose:
 *      Gets the unsigned value packed within a 64 bit word with given least 
 *      significant bit (lsb) and width
 * Arguments:
 *      (uint64_t) word:  The word to get value from 
 *      (unsigned) width: The number of bits in the value to get
 *      (unsigned) lsb:   The index of the least significant bit (lsb) of
 *                        the value to get
 * Returns:
 *      (uint64t) The unsigned number represented in width bits at lsb
 * Notes: 
 *      - Throws a CRE if width > 64 or if width + lsb > 64 as we only support 
 *        up to 64 bit words
 *      - As no information can be represented in 0 bits, if a width of 0 is
 *        supplied, we return 0
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb) 
{
        assert(width <= WORD_LEN); 
        assert(width + lsb <= WORD_LEN);

        /* A width of 0 is defined to return a value 0 */
        if (width == 0) {
                return 0;
        }

        /* Number of bits between MSB of the bits we're getting and the MSB of
         * the word */
        unsigned distance_to_msb = (WORD_LEN - (lsb + width));

        /* LSL to get appropriate sign bit as MSB */
        int64_t unsigned_word = shift_left(word, distance_to_msb);
        
        /* ASR to get the bits we're getting in their correct position and 
         * propagate their sign bit (MSB) in case it's negative */
        return shift_right_l(unsigned_word, lsb + distance_to_msb);
}


/* Bitpack_gets
 * Purpose:
 *      Gets the signed value packed within a 64 bit word with given least 
 *      significant bit (lsb) and width
 * Arguments:
 *      (uint64_t) word:  The word to get value from 
 *      (unsigned) width: The number of bits in the value to get
 *      (unsigned) lsb:   The index of the least significant bit (lsb) of
 *                        the value to get
 * Returns:
 *      (int64t) The signed number represented in width bits at lsb 
 * Notes: 
 *      - Throws a CRE if width > 64 or if width + lsb > 64 as we only support 
 *        up to 64 bit words
 *      - As no information can be represented in 0 bits, if a width of 0 is
 *        supplied, we return 0
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb) { 
        assert(width <= WORD_LEN);
        assert(width + lsb <= WORD_LEN);
        
        /* A width of 0 is defined to return a value 0 */
        if (width == 0) {
                return 0;
        }
        
        /* Number of bits between MSB of the bits we're getting and the MSB of
         * the word */
        unsigned distance_to_msb = (WORD_LEN - (lsb + width));

        /* LSL to get appropriate sign bit as MSB */
        int64_t signed_word = shift_left(word, distance_to_msb);
        
        /* ASR to get the bits we're getting in their correct position and 
         * propagate their sign bit (MSB) in case it's negative */
        return shift_right_a(signed_word, lsb + distance_to_msb);
}


/* Bitpack_newu
 * Purpose: 
 *      Update a field of the bitpacked word to the specified value
 * Arguments:
 *      (uint64_t) word : the 64 bit word to update
 *      (unsigned) width: the number of bits to update
 *      (unsigned) lsb  : the index of the lsb to start updating from
 *      (uint64_t) value : the bits used to update the word
 * Returns: 
 *      (uint64_t) The unsigned word with the updated value
 * Notes:
 *      - Throws a CRE if width is greater than word length or if 
 *        width + lsb is greater than word length
 *      - Raises the Bitpack_Overflow exception if the value the user is
 *        trying to write would not fit the the given width
 */
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                      uint64_t value)
{
        assert(width <= WORD_LEN);
        assert(width + lsb <= WORD_LEN);

        /* Check if the value fits in the bits they're asking us for */
        if (!Bitpack_fitsu(value, width)) {
                RAISE(Bitpack_Overflow);
        }

        /* Make a number which is width 1's */
        uint64_t mask = shift_left(1, width) - 1;

        /* Then get it in position by aligning it with the bits to clear */
        mask = shift_left(mask, lsb);

        /* Then clear those bits lining up with the 1's in the mask */
        word &= ~mask;

        /* Place value into the word. Because it's unsigned all bits to the
         * left of the width bits in value will be 0 so not affect word */
        return word | shift_left(value, lsb);
}


/* Bitpack_news
 * Purpose: 
 *      Update a field of the bitpacked word to the specified value
 * Arguments:
 *      (uint64_t) word : the 64 bit word to update
 *      (unsigned) width: the number of bits to update
 *      (unsigned) lsb  : the index of the lsb to start updating from
 *      (int64_t)  value : the bits used to update the word
 * Returns: 
 *      (uint64_t) The unsigned word with the updated value
 * Notes:
 *      - Throws a CRE if width is greater than word length or if 
 *        width + lsb is greater than word length
 *      - Raises the Bitpack_Overflow exception if the value the user is
 *        trying to write would not fit the the given width
 */
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
                      int64_t value)
{
        assert(width <= WORD_LEN);
        assert(width + lsb <= WORD_LEN);

        /* Check if the value fits in the bits they're asking us for */
        if (!Bitpack_fitss(value, width)) {
                RAISE(Bitpack_Overflow);
        }

        /* Make a number which is width 1's */
        uint64_t mask = shift_left(1, width) - 1;

        /* Then get it in position by aligning it with the bits to clear */
        mask = shift_left(mask, lsb);

        /* Then clear those bits lining up with the 1's in the mask */
        word &= ~mask;

        /* Get the value in the right position to put in the word */
        value = shift_left(value, lsb);

        /* If value is negative, we want to get rid of leading 1's */
        value &= mask;

        /* Place value into the word. Anding with the mask made sure all bits
         * not in the lsb -> lsb + width are 0, so don't affect word */
        return word | value;
}


/* ------------------- Helper Functions ------------------------- */

/* shift_left
 * Purpose:
 *      Logically shifts the given word left by a certain shift amount
 * Arguments:
 *      (uint64_t) word:  A 64-bit word of data to shift
 *      (unsigned) shift: The amount to shift word by 
 * Returns:
 *      (uint64_t) The shifted word
 * Notes:
 *      - CRE for shift to be > 64
 *      - If user shifts by 64 bits, all information gets shifted out of range,
 *        and the word is filled with zeros, so the result is a word containing
 *        all 0's (0x0)
 * */
static uint64_t shift_left(uint64_t word, unsigned shift)
{
        assert(shift <= WORD_LEN);
        
        if (shift == WORD_LEN) {
                return 0;
        } else {
                return word << shift;
        }
}

/* shift_right_r
 * Purpose:
 *      Logically shifts the given word right by a certain shift amount
 * Arguments:
 *      (uint64_t) word:  A 64-bit word of data to shift
 *      (unsigned) shift: The amount to shift word by 
 * Returns:
 *      (uint64_t) The shifted word
 * Notes:
 *      - CRE for shift to be > 64
 *      - If user shifts by 64 bits, all information gets shifted out of range,
 *        and the word is filled with zeros, so the result is a word containing
 *        all 0's (0x0)
 */
static uint64_t shift_right_l(uint64_t word, unsigned shift)
{
        assert(shift <= WORD_LEN);

        if (shift == WORD_LEN) {
                return 0;
        } else {
                return word >> shift;
        }
}


/* shift_right_a
 * Purpose:
 *      Arithmetically shifts the given word right by a certain shift amount,
 *      extending its sign bit
 * Arguments:
 *      (int64_t) word:   A 64-bit word of data to shift
 *      (unsigned) shift: The amount to shift word by 
 * Returns:
 *      (int64_t) The shifted word
 * Notes:
 *      - CRE for shift to be > 64
 *      - If user shifts by 64 bits, all information gets shifted out of range.
 *        So, if the number is negative (has a 1 sign bit), it's continued all
 *        the way to bit 0, so the result is a word of all 1's, or -1 (signed).
 *        However, if the number is positive (has a 0 sign bit), this acts as
 *        a logical shift right, filling bits in with 0 from the left, so the
 *        result is 0x0.
 */
static int64_t shift_right_a(int64_t word, unsigned shift)
{
        assert(shift <= WORD_LEN);

        if (shift == WORD_LEN) {
                /* Check for whether we should propagate a 1 or 0 */
                if (word < 0) {
                        return -1;
                } else {
                        return 0;
                }
        } else {
                return word >> shift;
        }
}