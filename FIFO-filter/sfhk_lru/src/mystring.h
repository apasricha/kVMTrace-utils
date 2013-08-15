/* ***************************************************************
   ***************************************************************
   **                                                           **
   ** Horrible C String Handlers                                **
   **                                                           **
   ** Authors: Douglas Van Wieren                               **
   **          Paul Wilson                                      **
   **                                                           **
   ** Description:                                              **
   **                                                           **
   **      Back in the good ol' days, there was SNOBOL for      **
   ** handling strings, and, of course, we've all seen C++      **
   ** classes which implement most of the functionality of      **
   ** that sadly-missed language.  This isn't one of those.     **
   **                                                           **
   **      No, this is something mind-bogglingly depraved.      **
   ** It's a set of routines which actually use the stuff       **
   ** defined in string.h to handle strings.  See those two     **
   ** names up there?  The *other* guy did this.  Not me.       **
   ** Not on a bet.  And even if I did do this, I wouldn't      **
   ** admit to it.  Not now.  Not ever.                         **
   **                                                           **
   **      The only saving grace this code has is that it's     **
   ** short.  Not much of a claim, is it?                       **
   **                                                           **
   ***************************************************************
   ***************************************************************

   $Source: /u/dvw/ss/sfkaplan/RCS/mystring.h,v $
   $RCSfile: mystring.h,v $
   $Revision: 2.0 $
   $Date: 1996/06/24 18:27:08 $
   $Author: dvw $
   $State: Exp $
   $Log: mystring.h,v $
   Revision 2.0  1996/06/24 18:27:08  dvw
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
#ifndef dvw_mystring_header
#define dvw_mystring_header

#ifndef dvw_general_header
#include "general.h"
#endif
#include <stdarg.h>

/*
   Our notion of strings as character buffers.
*/

#define MAX_CHARACTER_BUFFER_LENGTH  (1024)
typedef char standard_buffer_type[MAX_CHARACTER_BUFFER_LENGTH];

inline void treat_buffer_as_string(standard_buffer_type standard_buffer)
{
   standard_buffer[MAX_CHARACTER_BUFFER_LENGTH - 1] = 0;
};

inline void treat_buffer_as_string(char * buffer, int buffer_size)
{
   buffer[buffer_size - 1] = 0;
};

inline char * get_standard_buffer(FILE * input_stream,
                                  standard_buffer_type standard_buffer)
{
   return fgets(standard_buffer, MAX_CHARACTER_BUFFER_LENGTH, input_stream);
};

/*
   A triviality --- returns 0 if failure
*/

char last_in_string(const char * string)
{
   int position = strlen(intended_string(string));
   if (position > 0)
   {
      return string[position - 1];
   }
   else
   {
      return 0;
   };
};

/*
   One of many omissions in the usual string handling routines is a
   function to find the last occurence of any of a set of characters
   within a string.  Examine the man pages for strpbrk and strrchr.
*/
char * strrpbrk(const char * string, const char * break_set)
{
   if ((break_set == NULL) || (string == NULL))
   {
      return NULL;
   }
   else
   {
      char * string_index    = (char *)string + strlen(string);
      char * break_set_last  = (char *)break_set + strlen(break_set) - 1;
      char * break_set_index = break_set_last + 0;
      bool test = 1;
      while ((test) && (string_index > string))
      {
         string_index--;
         test = (*string_index != *break_set_index);
         while ((test) && break_set_index > break_set)
         {
            break_set_index--;
            test = (*string_index != *break_set_index);
         };
         break_set_index = break_set_last;
      };
      if (test)
      {
         return NULL;
      }
      else
      {
         return string_index;
      };
   };
};

inline void standard_strip_path(standard_buffer_type buffer)
{
   treat_buffer_as_string(buffer);
   char * temp = strrchr(buffer, '/');
   if (temp != NULL)
   {
      int characters_to_move = strlen(temp);
      memmove(buffer, temp + 1, characters_to_move);
   };
};

inline void strip_path(char * buffer, int buffer_size)
{
   treat_buffer_as_string(buffer, buffer_size);
   char * temp = strrchr(buffer, '/');
   if (temp != NULL)
   {
      int characters_to_move = strlen(temp);
      memmove(buffer, temp + 1, characters_to_move);
   };
};

inline char * weak_strip_path(const char * string)
{
   static standard_buffer_type common_buffer;
   strcpy(common_buffer, string);
   standard_strip_path(common_buffer);
   return common_buffer;
};

inline void standard_strip_mangling(standard_buffer_type buffer)
{
   treat_buffer_as_string(buffer);
   char * mangle_char_ptr = strrpbrk(buffer, "_-:.;,=+/\\\"\'");
   if (mangle_char_ptr != NULL)
   {
      *mangle_char_ptr = 0;
   };
};

inline void strip_mangling(char * buffer, int buffer_size)
{
   treat_buffer_as_string(buffer, buffer_size);
   char * mangle_char_ptr = strrpbrk(buffer, "_-:.;,=+/\\\"\'");
   if (mangle_char_ptr != NULL)
   {
      *mangle_char_ptr = 0;
   };
};

inline char * weak_strip_mangling(const char * string)
{
   static standard_buffer_type common_buffer;
   strcpy(common_buffer, string);
   standard_strip_mangling(common_buffer);
   return common_buffer;
};

inline void standard_strip_comment(standard_buffer_type buffer)
{
   treat_buffer_as_string(buffer);
   char * comment_char_ptr = strchr(buffer, '#');
   if (comment_char_ptr != NULL)
   {
      *comment_char_ptr = 0;
   };
};

inline void strip_comment(char * buffer, int buffer_size)
{
   treat_buffer_as_string(buffer, buffer_size);
   char * comment_char_ptr = strchr(buffer, '#');
   if (comment_char_ptr != NULL)
   {
      *comment_char_ptr = 0;
   };
};

inline char * weak_strip_comment(const char * string)
{
   static standard_buffer_type common_buffer;
   strcpy(common_buffer, string);
   standard_strip_comment(common_buffer);
   return common_buffer;
};

inline void standard_strip_return(standard_buffer_type buffer)
{
   treat_buffer_as_string(buffer);
   char * return_char_ptr = strchr(buffer, '\n');
   if (return_char_ptr != NULL)
   {
      *return_char_ptr = 0;
   };
};

inline void strip_return(char * buffer, int buffer_size)
{
   treat_buffer_as_string(buffer, buffer_size);
   char * return_char_ptr = strchr(buffer, '\n');
   if (return_char_ptr != NULL)
   {
      *return_char_ptr = 0;
   };
};

inline char * weak_strip_return(const char * string)
{
   static standard_buffer_type common_buffer;
   strcpy(common_buffer, string);
   standard_strip_return(common_buffer);
   return common_buffer;
};

inline void standard_strip_leading_blanks(standard_buffer_type buffer)
{
   treat_buffer_as_string(buffer);
   int characters_to_ignore = strspn(buffer, " ");
   if (characters_to_ignore != 0)
   {
      int characters_to_move   = strlen(buffer) - characters_to_ignore;
      memmove(buffer, buffer + characters_to_ignore, characters_to_move);
   };
};

inline void strip_leading_blanks(char * buffer, int buffer_size)
{
   treat_buffer_as_string(buffer, buffer_size);
   int characters_to_ignore = strspn(buffer, " ");
   if (characters_to_ignore != 0)
   {
      int characters_to_move   = strlen(buffer) - characters_to_ignore;
      memmove(buffer, buffer + characters_to_ignore, characters_to_move);
   };
};

inline char * weak_strip_leading_blanks(const char * string)
{
   static standard_buffer_type common_buffer;
   strcpy(common_buffer, string);
   standard_strip_leading_blanks(common_buffer);
   return common_buffer;
};


inline void standard_strip_trailing_blanks(standard_buffer_type buffer)
{
   treat_buffer_as_string(buffer);
   char * last = buffer + strlen(buffer) - 1;
   while ((last >= buffer) && (*last == ' '))
   {
      *last = 0;
      last--;
   };
};

inline void strip_trailing_blanks(char * buffer, int buffer_size)
{
   treat_buffer_as_string(buffer, buffer_size);
   char * last = buffer + strlen(buffer) - 1;
   while ((last >= buffer) && (*last == ' '))
   {
      *last = 0;
      last--;
   };
};

inline char * weak_strip_trailing_blanks(const char * string)
{
   static standard_buffer_type common_buffer;
   strcpy(common_buffer, string);
   standard_strip_trailing_blanks(common_buffer);
   return common_buffer;
};

inline void standard_convert_tabs(standard_buffer_type buffer)
{
   treat_buffer_as_string(buffer);
   for (char * curr = buffer + strlen(buffer) - 1; curr >= buffer; curr--)
   {
      if (*curr == '\t')
      {
         *curr = ' ';
      };
   };
};

inline void convert_tabs(char * buffer, int buffer_size)
{
   treat_buffer_as_string(buffer, buffer_size);
   for (char * curr = buffer + strlen(buffer) - 1; curr >= buffer; curr--)
   {
      if (*curr == '\t')
      {
         *curr = ' ';
      };
   };
};

inline char * weak_convert_tabs(const char * string)
{
   static standard_buffer_type common_buffer;
   strcpy(common_buffer, string);
   standard_convert_tabs(common_buffer);
   return common_buffer;
};

inline void standard_clean_up(standard_buffer_type buffer)
{
   standard_strip_return(buffer);
   standard_strip_comment(buffer);
   standard_convert_tabs(buffer);
   standard_strip_trailing_blanks(buffer);
   standard_strip_leading_blanks(buffer);
};

inline void clean_up(char * buffer, int buffer_size)
{
   strip_return(buffer, buffer_size);
   strip_comment(buffer, buffer_size);
   convert_tabs(buffer, buffer_size);
   strip_trailing_blanks(buffer, buffer_size);
   strip_leading_blanks(buffer, buffer_size);
};

inline char * weak_clean_up(const char * string)
{
   static standard_buffer_type common_buffer;
   strcpy(common_buffer, string);
   standard_clean_up(common_buffer);
   return common_buffer;
};

inline char * weak_first_token(const char * string)
{
   static standard_buffer_type common_buffer;
   strcpy(common_buffer, string);
   standard_clean_up(common_buffer);
   return strtok(common_buffer, " ");
};

inline char * get_clean_standard_buffer(FILE *               input_stream,
                                        standard_buffer_type standard_buffer)
{
   char * return_value = standard_buffer;
   boolean still_looking = LOGICAL_TRUE;

   while (still_looking)
   {
      return_value = get_standard_buffer(input_stream, standard_buffer);
      if (return_value == NULL)
      {
         still_looking = LOGICAL_FALSE;
      }
      else
      {
         standard_clean_up(standard_buffer);
         if (standard_buffer[0] != 0)
         {
            still_looking = LOGICAL_FALSE;
         };
      };
   };
   return return_value;
};

attempt_flag bounded_strcpy(char * string1, const char * string2, UInt16 bound)
{
   if (strlen(string2) < bound)
   {
      strcpy(string1, string2);
      return SUCCESS;
   }
   else
   {
      return FAILURE;
   };
};

inline attempt_flag bounded_strcpy(standard_buffer_type string1,
                                   const char *         string2)
{
   return bounded_strcpy(string1, string2, MAX_CHARACTER_BUFFER_LENGTH);
};

attempt_flag bounded_strcat(char * string1, const char * string2, UInt16 bound)
{
   UInt16 newlength = strlen(string1) + strlen(string2) + 1;
   if (newlength < bound)
   {
      strcat(string1, string2);
      return SUCCESS;
   }
   else
   {
      return FAILURE;
   };
};

inline attempt_flag bounded_strcat(standard_buffer_type string1,
                                   const char *         string2)
{
   return bounded_strcat(string1, string2, MAX_CHARACTER_BUFFER_LENGTH);
};

attempt_flag bounded_strprecat(char *       string1,
                               const char * string2,
                               UInt16       bound)
{
   UInt16 newlength = strlen(string1) + strlen(string2) + 1;
   if (newlength < bound)
   {
      char * temp;
      my_frail_malloc_array(temp, char, newlength);
      strcpy(temp, string2);
      strcat(temp, string1);
      strcpy(string1, temp);
      return SUCCESS;
   }
   else
   {
      return FAILURE;
   };
};

inline attempt_flag bounded_strprecat(standard_buffer_type string1,
                                      const char *         string2)
{
   return bounded_strprecat(string1, string2, MAX_CHARACTER_BUFFER_LENGTH);
};



/* Have to be very careful how this is used . . . */
/*
char * weak_concat(...)
{
   static standard_buffer_type common_buffer;
   va_list ap;
   const char * substring_n;
   va_start(ap);
   substring_n = va_arg(ap, char *);
   strcpy(common_buffer, substring_n);
   while ((substring_n = va_arg(ap, const char *)) != NULL)
   {
      bounded_strcat(common_buffer, substring_n);
   };
   va_end(ap);
   return common_buffer;
};
*/
#endif
