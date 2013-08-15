/* ***************************************************************
   ***************************************************************
   **                                                           **
   ** Replacements for specific instances of fprintf, scanf,    **
   ** etc.                                                      **
   **                                                           **
   ** Author: Douglas Van Wieren                                **
   **                                                           **
   ** Description:                                              **
   **                                                           **
   **      This header file contains some routines designed to  **
   ** move data to and from files more quickly than the very    **
   ** general methods provided by standard C formatted IO.      **
   ** A second intent is avoiding malloc, yet making the files  **
   ** human readable.                                           **
   **                                                           **
   ** Notes:                                                    **
   **                                                           **
   **      Speed concerns have prioritized hex output without   **
   ** the preceding "0x".  Thus, it's necessary to check the    **
   ** intent of each routine carefully to be sure the format is **
   ** what you intend.                                          **
   **                                                           **
   **      We assume fputc, fputs, fgetc, and fgets are         **
   ** implemented using buffers.  The internal buffer is        **
   ** provided for the purpose of making line output            **
   ** tractable as a process.                                   **
   **                                                           **
   **      The coding style here is bad --- we use those        **
   ** disgusting c macros to hide repetitive coding.  A better  **
   ** implementation would be to expand those macros (using     **
   ** the compiler) and hand optimize the resulting code.       **
   **                                                           **
   ***************************************************************
   ***************************************************************

   $Source: /u/dvw/ss/sfkaplan/RCS/inout.h,v $
   $RCSfile: inout.h,v $
   $Revision: 2.0 $
   $Date: 1996/06/24 18:27:05 $
   $Author: dvw $
   $State: Exp $
   $Log: inout.h,v $
   Revision 2.0  1996/06/24 18:27:05  dvw
   Adjusted to local RCS version

   Revision 1.1  1996/06/13 16:32:30  dvw
   Initial revision

   $Locker:  $

   Copyright (c) 1995

   Object Oriented Memory Policy Studies (OOMPS)
   [a division of OOPS under the direction of Paul R. Wilson],
   Department of Computer Science,
   University of Texas at Austin

   and

   The Advanced Computer Architecture Laboratory (ACAL),
   Department of Electrical Engineering and Computer Science,
   University of Michigan at Ann Arbor

   All rights reserved.
*/
#ifndef dvw_inout_header
#define dvw_inout_header

#ifndef dvw_general_header
#include "general.h"
#endif
#ifndef dvw_mystring_header
#include "mystring.h"
#endif

static const char   hexdigit[]       = "0123456789abcdef";
static       char   inout_common_buffer[MAX_CHARACTER_BUFFER_LENGTH];
/*
   Ouput buffer handlers  --- these are macros to prevent repetitive
   coding.  Note deliberate extra brackets.
*/

#define clear_io_buffer  \
   {\
   register unsigned int inout_common_buffer_index = 0

#define put_char_in_buffer(value) \
    inout_common_buffer[inout_common_buffer_index] = value; \
    inout_common_buffer_index++;

#define put_nibble_in_buffer(value) \
    inout_common_buffer[inout_common_buffer_index] = hexdigit[(value) & 15]; \
    inout_common_buffer_index++;

#define mask_leading_zeros \
   {\
   register boolean zero_not_leading = LOGICAL_FALSE


#define put_nibble_in_buffer_cond(value) \
   if (zero_not_leading)\
   {\
      inout_common_buffer[inout_common_buffer_index] = hexdigit[(value) & 15]; \
      inout_common_buffer_index++; \
   }\
   else\
   {\
      register unsigned int nibble = (value);\
      if (nibble != 0)\
      {\
         inout_common_buffer[inout_common_buffer_index] = hexdigit[nibble]; \
         inout_common_buffer_index++; \
         zero_not_leading = LOGICAL_TRUE; \
      };\
   }

#define end_mask_leading_zeros \
   }

#define flush_io_buffer_and_return \
   inout_common_buffer[inout_common_buffer_index] = (char)0;\
   return fputs(inout_common_buffer, output_stream);\
   }

#define dump_buffer_to_buffer_and_return \
   inout_common_buffer[inout_common_buffer_index] = (char)0;\
   return strcpy(inout_common_buffer, output_buffer);\
   }

#define finish_buffer_and_return_common \
   inout_common_buffer[inout_common_buffer_index] = (char)0;\
   return inout_common_buffer;\
   }

/*
   Ouput fixed-width values
*/

inline int put_nibble_fixed(FILE * output_stream, unsigned char value)
{
   return fputc(hexdigit[value & 15], output_stream);
};

inline int put_byte_fixed(FILE * output_stream, unsigned char value)
{
   clear_io_buffer;

   put_nibble_in_buffer(value >>  4);
   put_nibble_in_buffer(value      );

   flush_io_buffer_and_return;
};

inline int put_UInt16_fixed(FILE * output_stream, UInt16 value)
{
   clear_io_buffer;

   put_nibble_in_buffer(value >> 12);
   put_nibble_in_buffer(value >>  8);
   put_nibble_in_buffer(value >>  4);
   put_nibble_in_buffer(value      );

   flush_io_buffer_and_return;
};

inline int put_UInt32_fixed(FILE * output_stream, UInt32 value)
{
   clear_io_buffer;

   put_nibble_in_buffer(value >> 28);
   put_nibble_in_buffer(value >> 24);
   put_nibble_in_buffer(value >> 20);
   put_nibble_in_buffer(value >> 16);
   put_nibble_in_buffer(value >> 12);
   put_nibble_in_buffer(value >>  8);
   put_nibble_in_buffer(value >>  4);
   put_nibble_in_buffer(value      );

   flush_io_buffer_and_return;
};

inline int put_UInt64_fixed(FILE * output_stream, UInt64 value)
{
   clear_io_buffer;

   put_nibble_in_buffer(value >> 60);
   put_nibble_in_buffer(value >> 56);
   put_nibble_in_buffer(value >> 52);
   put_nibble_in_buffer(value >> 48);
   put_nibble_in_buffer(value >> 44);
   put_nibble_in_buffer(value >> 40);
   put_nibble_in_buffer(value >> 36);
   put_nibble_in_buffer(value >> 32);
   put_nibble_in_buffer(value >> 28);
   put_nibble_in_buffer(value >> 24);
   put_nibble_in_buffer(value >> 20);
   put_nibble_in_buffer(value >> 16);
   put_nibble_in_buffer(value >> 12);
   put_nibble_in_buffer(value >>  8);
   put_nibble_in_buffer(value >>  4);
   put_nibble_in_buffer(value      );

   flush_io_buffer_and_return;
};

/* COMMENTED OUT
inline char * put_UInt64_fixed(char * output_buffer, UInt64 value)
{
   clear_io_buffer;

   put_nibble_in_buffer(value >> 60);
   put_nibble_in_buffer(value >> 56);
   put_nibble_in_buffer(value >> 52);
   put_nibble_in_buffer(value >> 48);
   put_nibble_in_buffer(value >> 44);
   put_nibble_in_buffer(value >> 40);
   put_nibble_in_buffer(value >> 36);
   put_nibble_in_buffer(value >> 32);
   put_nibble_in_buffer(value >> 28);
   put_nibble_in_buffer(value >> 24);
   put_nibble_in_buffer(value >> 20);
   put_nibble_in_buffer(value >> 16);
   put_nibble_in_buffer(value >> 12);
   put_nibble_in_buffer(value >>  8);
   put_nibble_in_buffer(value >>  4);
   put_nibble_in_buffer(value      );

   dump_buffer_to_buffer_and_return;
};
*/
/*
   Ouput variable-width values
*/

inline int put_nibble(FILE * output_stream, unsigned char value)
{
   return fputc(hexdigit[value & 15], output_stream);
};

inline int put_byte(FILE * output_stream, unsigned char value)
{
   clear_io_buffer;
   mask_leading_zeros;

   put_nibble_in_buffer_cond(value >>  4);
   put_nibble_in_buffer_cond(value      );

   end_mask_leading_zeros;
   flush_io_buffer_and_return;
};

inline int put_UInt16(FILE * output_stream, UInt16 value)
{
   clear_io_buffer;
   mask_leading_zeros;

   put_nibble_in_buffer_cond(value >> 12);
   put_nibble_in_buffer_cond(value >>  8);
   put_nibble_in_buffer_cond(value >>  4);
   put_nibble_in_buffer     (value      );

   end_mask_leading_zeros;
   flush_io_buffer_and_return;
};

inline int put_UInt32(FILE * output_stream, UInt32 value)
{
   clear_io_buffer;
   mask_leading_zeros;

   put_nibble_in_buffer_cond(value >> 28);
   put_nibble_in_buffer_cond(value >> 24);
   put_nibble_in_buffer_cond(value >> 20);
   put_nibble_in_buffer_cond(value >> 16);
   put_nibble_in_buffer_cond(value >> 12);
   put_nibble_in_buffer_cond(value >>  8);
   put_nibble_in_buffer_cond(value >>  4);
   put_nibble_in_buffer     (value      );

   end_mask_leading_zeros;
   flush_io_buffer_and_return;
};

inline int put_UInt64(FILE * output_stream, UInt64 value)
{
   clear_io_buffer;
   mask_leading_zeros;

   put_nibble_in_buffer_cond(value >> 60);
   put_nibble_in_buffer_cond(value >> 56);
   put_nibble_in_buffer_cond(value >> 52);
   put_nibble_in_buffer_cond(value >> 48);
   put_nibble_in_buffer_cond(value >> 44);
   put_nibble_in_buffer_cond(value >> 40);
   put_nibble_in_buffer_cond(value >> 36);
   put_nibble_in_buffer_cond(value >> 32);
   put_nibble_in_buffer_cond(value >> 28);
   put_nibble_in_buffer_cond(value >> 24);
   put_nibble_in_buffer_cond(value >> 20);
   put_nibble_in_buffer_cond(value >> 16);
   put_nibble_in_buffer_cond(value >> 12);
   put_nibble_in_buffer_cond(value >>  8);
   put_nibble_in_buffer_cond(value >>  4);
   put_nibble_in_buffer     (value      );

   end_mask_leading_zeros;
   flush_io_buffer_and_return;
};
/* COMMENTED OUT
inline char * put_UInt64(char * output_buffer, UInt64 value)
{
   clear_io_buffer;
   mask_leading_zeros;

   put_nibble_in_buffer_cond(value >> 60);
   put_nibble_in_buffer_cond(value >> 56);
   put_nibble_in_buffer_cond(value >> 52);
   put_nibble_in_buffer_cond(value >> 48);
   put_nibble_in_buffer_cond(value >> 44);
   put_nibble_in_buffer_cond(value >> 40);
   put_nibble_in_buffer_cond(value >> 36);
   put_nibble_in_buffer_cond(value >> 32);
   put_nibble_in_buffer_cond(value >> 28);
   put_nibble_in_buffer_cond(value >> 24);
   put_nibble_in_buffer_cond(value >> 20);
   put_nibble_in_buffer_cond(value >> 16);
   put_nibble_in_buffer_cond(value >> 12);
   put_nibble_in_buffer_cond(value >>  8);
   put_nibble_in_buffer_cond(value >>  4);
   put_nibble_in_buffer     (value      );

   end_mask_leading_zeros;
   dump_buffer_to_buffer_and_return;
};
*/
/*
   Output separators
*/

inline int put_space(FILE * output_stream)
{
   return fputc(' ', output_stream);
};

inline int put_newline(FILE * output_stream)
{
   return fputc('\n', output_stream);
};

/*
   Common output formats
*/

inline void put_dinero_record(FILE *       output_stream,
                              unsigned int dinero_code,
                              UInt32       address)
{
   put_nibble(output_stream, dinero_code);
   put_space(output_stream);
   put_UInt32(output_stream, address);
   put_newline(output_stream);
};

/* WEENER (OR WEINER) HERE --- replace with table look up */
/*  A little bit in the order we expect occurences */
inline int convert_hexdigit(char digit)
{
   if ((digit >= '0') && (digit <= '9'))
   {
      return digit - '0';
   }
   else if ((digit >= 'a') && (digit <= 'f'))
   {
      return digit - 'a' + 10;
   }
   else if ((digit == ' ') || (digit == '\n'))
   {
      return -1;
   }
   else if ((digit >= 'A') && (digit <= 'F'))
   {
      return digit - 'A' + 10;
   }
   else
   {
      return -1;
   }
};

/*
   Assumes string is non-NULL and returns index into the string
   following number
*/
template <class any_unsigned>
inline int get_one_hex_value(const char * string, any_unsigned & value)
{
   value = 0;
   register int current = 0;
   register int digit = convert_hexdigit(string[current]);
   while ((string[current] != 0) && (digit == -1))
   {
      current++;
      digit = convert_hexdigit(string[current]);
   };
   while ((string[current] != 0) && (digit != -1))
   {
      value = value*16 + digit;
      current++;
      digit = convert_hexdigit(string[current]);
   }
   return current;
};

template <class any_unsigned>
inline int get_one_dec_value(const char * string, any_unsigned & value)
{
   value = 0;
   register int current = 0;
   register int digit = convert_hexdigit(string[current]);
   while ((string[current] != 0) && (digit == -1))
   {
      current++;
      digit = convert_hexdigit(string[current]);
   };
   while ((string[current] != 0) && (digit != -1) && (digit < 10))
   {
      value = value*10 + digit;
      current++;
      digit = convert_hexdigit(string[current]);
   }
   return current;
};

template <class any_unsigned>
inline int get_one_std_value(const char * string, any_unsigned & value)
{
   if ((string[0] == '0') &&
       ((string[1] == 'x') || (string[1] == 'X')))
   {
      return get_one_hex_value(&string[2], value);
   }
   else
   {
      return get_one_dec_value(string, value);
   };
};


template <class any_unsigned>
inline attempt_flag get_one_hex_value(FILE * input_stream,
                                      any_unsigned & value)
{
   if (get_standard_buffer(input_stream, inout_common_buffer) != NULL)
   {
      get_one_hex_value(inout_common_buffer, value);
      return SUCCESS;
   }
   else
   {
      return FAILURE;
   };
};

template <class any_unsigned>
inline attempt_flag get_one_dec_value(FILE * input_stream,
                                      any_unsigned & value)
{
   if (get_standard_buffer(input_stream, inout_common_buffer) != NULL)
   {
      get_one_dec_value(inout_common_buffer, value);
      return SUCCESS;
   }
   else
   {
      return FAILURE;
   };
};

template <class any_unsigned>
inline attempt_flag get_one_std_value(FILE * input_stream,
                                      any_unsigned & value)
{
   if (get_standard_buffer(input_stream, inout_common_buffer) != NULL)
   {
      get_one_std_value(inout_common_buffer, value);
      return SUCCESS;
   }
   else
   {
      return FAILURE;
   };
};

template <class any_unsigned1, class any_unsigned2>
inline attempt_flag get_two_hex_values(FILE * input_stream,
                                       any_unsigned1 & value1,
                                       any_unsigned2 & value2)
{
   if (get_standard_buffer(input_stream, inout_common_buffer) != NULL)
   {
      int buffer_index = get_one_hex_value(inout_common_buffer, value1);
      get_one_hex_value(&inout_common_buffer[buffer_index], value2);
      return SUCCESS;
   }
   else
   {
      return FAILURE;
   };
};

template <class any_unsigned1, class any_unsigned2>
inline attempt_flag get_two_std_values(FILE * input_stream,
                                       any_unsigned1 & value1,
                                       any_unsigned2 & value2)
{
   if (get_standard_buffer(input_stream, inout_common_buffer) != NULL)
   {
      int buffer_index = get_one_std_value(inout_common_buffer, value1);
      get_one_std_value(&inout_common_buffer[buffer_index], value2);
      return SUCCESS;
   }
   else
   {
      return FAILURE;
   };
};

template <class any_unsigned1, class any_unsigned2, class any_unsigned3>
inline attempt_flag get_three_hex_values(FILE * input_stream,
                                         any_unsigned1 & value1,
                                         any_unsigned2 & value2,
                                         any_unsigned3 & value3)
{
   if (get_standard_buffer(input_stream, inout_common_buffer) != NULL)
   {
      int buffer_index = get_one_hex_value(inout_common_buffer, value1);
      buffer_index += get_one_hex_value(&inout_common_buffer[buffer_index],
                                       value2);
      get_one_hex_value(&inout_common_buffer[buffer_index], value3);
      return SUCCESS;
   }
   else
   {
      return FAILURE;
   };
};

template <class any_unsigned1, class any_unsigned2,
          class any_unsigned3, class any_unsigned4>
inline attempt_flag get_four_hex_values(FILE * input_stream,
                                         any_unsigned1 & value1,
                                         any_unsigned2 & value2,
                                         any_unsigned3 & value3,
                                         any_unsigned4 & value4)
{
   if (get_standard_buffer(input_stream, inout_common_buffer) != NULL)
   {
      int buffer_index = get_one_hex_value(inout_common_buffer, value1);
      buffer_index += get_one_hex_value(&inout_common_buffer[buffer_index],
                                       value2);
      buffer_index += get_one_hex_value(&inout_common_buffer[buffer_index],
                                       value3);
      get_one_hex_value(&inout_common_buffer[buffer_index], value4);
      return SUCCESS;
   }
   else
   {
      return FAILURE;
   };
};

template <class any_unsigned1, class any_unsigned2, class any_unsigned3>
inline attempt_flag get_three_std_values(FILE * input_stream,
                                         any_unsigned1 & value1,
                                         any_unsigned2 & value2,
                                         any_unsigned3 & value3)
{
   if (get_standard_buffer(input_stream, inout_common_buffer) != NULL)
   {
      int buffer_index = get_one_std_value(inout_common_buffer, value1);
      buffer_index += get_one_std_value(&inout_common_buffer[buffer_index],
                                       value2);
      get_one_std_value(&inout_common_buffer[buffer_index], value3);
      return SUCCESS;
   }
   else
   {
      return FAILURE;
   };
};

template <class any_unsigned1, class any_unsigned2,
          class any_unsigned3, class any_unsigned4>
inline attempt_flag get_four_std_values(FILE * input_stream,
                                        any_unsigned1 & value1,
                                        any_unsigned2 & value2,
                                        any_unsigned3 & value3,
                                        any_unsigned4 & value4)
{
   if (get_standard_buffer(input_stream, inout_common_buffer) != NULL)
   {
      int buffer_index = get_one_std_value(inout_common_buffer, value1);
      buffer_index += get_one_std_value(&inout_common_buffer[buffer_index],
                                       value2);
      buffer_index += get_one_std_value(&inout_common_buffer[buffer_index],
                                       value3);
      get_one_std_value(&inout_common_buffer[buffer_index], value4);
      return SUCCESS;
   }
   else
   {
      return FAILURE;
   };
};

inline int get_dinero_record(FILE * input_stream,
                             UInt8 & dinero_code,
                             UInt32 & address)
{
   if (get_standard_buffer(input_stream, inout_common_buffer) != NULL)
   {
      dinero_code = (int)(inout_common_buffer[0] - '0');

      char & digit = inout_common_buffer[2];
      if ((digit < 'a') || (digit > 'f'))
      {
         address = digit - '0';
      }
      else
      {
         address = digit - 'a' + 10;
      };
      register int term = strlen(inout_common_buffer) - 1;
      for (register int pos = 3; pos < term; pos++)
      {
          address *= 16;
          char & digit = inout_common_buffer[pos];
          if ((digit < 'a') || (digit > 'f'))
          {
             address += digit - '0';
          }
          else
          {
             address += digit - 'a' + 10;
          };
      };
      return 1;
   }
   else
   {
      return 0;
   };
};

/*
   In some cases, we may want io in the natural binary code of the
   machine; then, . . .
*/

struct extended_dinero_record
{
   unsigned int trace_count;
   unsigned int dinero_code;
   unsigned int address;
};

inline int put_extended_dinero_record(FILE * output_stream,
                                      unsigned int trace_count,
                                      unsigned int dinero_code,
                                      unsigned int address)
{
   extended_dinero_record rec;
   rec.trace_count = trace_count;
   rec.dinero_code = dinero_code;
   rec.address     = address;
   return fwrite(& rec,
                 sizeof(extended_dinero_record),
                 1,
                 output_stream);
};

inline int get_extended_dinero_record(FILE * input_stream,
                                      unsigned int & trace_count,
                                      unsigned int & dinero_code,
                                      unsigned int & address)
{
   extended_dinero_record rec;
   int return_dummy = fread(& rec,
                            sizeof(extended_dinero_record),
                            1,
                            input_stream);
   trace_count = rec.trace_count;
   dinero_code = rec.dinero_code;
   address     = rec.address;
   return return_dummy;
};

#undef clear_io_buffer
#undef put_char_in_buffer /* (value) */
#undef put_nibble_in_buffer /* (value) */
#undef mask_leading_zeros
#undef put_nibble_in_buffer_cond /* (value) */
#undef end_mask_leading_zeros
#undef flush_io_buffer_and_return
#undef dump_buffer_to_buffer_and_return
#undef finish_buffer_and_return_common
#endif
