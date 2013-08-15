/* ***************************************************************
   ***************************************************************
   **                                                           **
   ** General Memory Testing                                    **
   **                                                           **
   ** Author: Douglas Van Wieren                                **
   **                                                           **
   ** Description:                                              **
   **                                                           **
   **      Inclusion of this header plus a defintion of the     **
   ** a macro (#define MEMORY_TEST) will force a fairly robust  **
   ** test of allocation/deallocation behavior within the       **
   ** executable --- designed to detect coding errors.          **
   ** Omitting the macro will change the behavior of many       **
   ** items back to the default --- eliminating most error      **
   ** checking and reducing the remaining tests to the absolute **
   ** minimum.                                                  **
   **                                                           **
   ***************************************************************
   ***************************************************************

   $Source: /v/jive/v1/oops/locality-group/sfkaplan/RCS/check_memory.h,v $
   $RCSfile: check_memory.h,v $
   $Revision: 2.2 $
   $Date: 1996/07/05 21:45:40 $
   $Author: user_dvw_dir_check_memory $
   $State: Exp $
   $Log: check_memory.h,v $
   Revision 2.2  1996/07/05 21:45:40  user_dvw_dir_check_memory
   Adjusted header and comments.

   Revision 2.0  1996/06/24 18:26:56  dvw
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
#ifndef dvw_memory_header
#define dvw_memory_header
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void terminate_due_to_memory_problem(const char * filename, int line_number)
{
   static char temp_buffer[255];
   sprintf(temp_buffer, "Memory allocation problem at line %d in file %s\n",
                        line_number, filename);
   error_msg(temp_buffer);
   alt_exit(1);
};

#ifndef MEMORY_TEST

#define set_memory_milestone(anything)
inline void memory_probe() {};
#define my_new(class_type)              new class_type
#define my_new_array(class_type, count) new class_type[count]
#define my_delete(variable_name)        delete variable_name
#define my_delete_array(variable_name)  delete [] variable_name

#define my_frail_malloc(variable_name, type)                                  \
    if ((variable_name = (type *)malloc(sizeof(type))) == NULL)               \
    {                                                                         \
       terminate_due_to_memory_problem(__FILE__, __LINE__);                   \
    };

#define my_frail_malloc_array(variable_name, type, count)                     \
    if ((variable_name = (type *)malloc((count)*sizeof(type))) == NULL)       \
    {                                                                         \
       terminate_due_to_memory_problem(__FILE__, __LINE__);                   \
    };

#define my_frail_calloc_array(variable_name, class_type, count)               \
{                                                                             \
    int size = count * sizeof(class_type);                                    \
    if ((variable_name = (class_type *)malloc(size)) == NULL)                 \
    {                                                                         \
       terminate_due_to_memory_problem(__FILE__, __LINE__);                   \
    }                                                                         \
    else                                                                      \
    {                                                                         \
       char * as_char_array = (char *)variable_name;                          \
       for (int loop = 0; loop < size; loop++)                                \
       {                                                                      \
          as_char_array[loop] = 0;                                            \
       };                                                                     \
    };                                                                        \
 };

#define my_free(variable_name)                                                \
    {                                                                         \
       if (variable_name != NULL)                                             \
       {                                                                      \
          free(variable_name);                                                \
       };                                                                     \
       variable_name = NULL;                                                  \
    }

/*
   You won't believe what the library routine strdup does . . .
*/
char * underlying_string_duplicate(const char * source,
                                   const char * filename,
                                   int line_number)
{
   int length = strlen(source) + 1;
   char * string_ptr = (char *)malloc(length);
   if (string_ptr == NULL)
   {
       terminate_due_to_memory_problem(filename, line_number);
   };
   memmove(string_ptr, source, length);
   return string_ptr;
};

#define my_string_duplicate(source)                                          \
    underlying_string_duplicate(intended_string(source),                     \
                                __FILE__, __LINE__);

#else

extern void set_memory_milestone(const char * new_milestone);
extern void memory_probe();
extern void * simulator_malloc(size_t size);
extern void simulator_free(void *ptr);

template <class class_type>
void clean_simulator_free(class_type * ptr)
{
   simulator_free(ptr);
   ptr = (class_type *)NULL;
}

extern void check_in_assignment_after_malloc(void * any_address,
                                   const char * filename,
                                   int          line_number,
                                   const char * variable_name,
                                   const char * class_type,
                                   int          size,
                                   int          number);
extern void check_in_expression_after_malloc(void * any_address,
                                const char * filename,
                                int          line_number,
                                const char * class_type,
                                int          size,
                                int          number);
extern void check_out_before_free(void * any_ptr,
                                   const char * filename,
                                   int          line_number,
                                   const char * variable_name);


void * drop_thru_frail_malloc(size_t size,
                              const char * filename,
                              int          line_number)
{
    void * variable_name;
    if ((variable_name = simulator_malloc(size)) == NULL)
    {
       set_memory_milestone("Malloc failure");
       static char temp_buffer[1024];
       sprintf(temp_buffer,
           "Malloc of %d bytes failed in file %s, at line %d --- aborting.\n",
               size, filename, line_number);
       fputs(temp_buffer, alterr);
       exit(1);
    };
    return variable_name;
}

/* This is a bad hack, but kind of reasonable

   Here's the deal, someone's called new (and/or it's replacement) and we
   intercept the call and allocated the memory ourselves using a special
   version of malloc.  We need to make sure that other assumptions of new
   are satisfied --- the member function tables are in the right place
   with the right values and the constructor function is called.

   Well, we really can't do everything, but this approximates it.  What
   might go wrong is that the constructor might do something which the natural
   destructor undoes (allocate memory, set static variables, etc.)  However,
   this will work in the sense that the member function tables will be right
   (or approximately so).  The weak claim is that something which is more
   robust will be a *lot* longer.

   So, the assumption for the hack_approx_setup routines is that the memory
   has been allocated, the pointer is cast into the "right" type, but the
   memory isn't really prepared in any way to be of the desired type.

   BTW, there's an even worse problem for delete calls.
*/

template <class class_type>
class_type * hacked_approx_setup(class_type * pointer, int realsize)
{
   class_type temp;
   /* Good item, mft set, constructor called */

   memmove(pointer, (void *)&temp, realsize);
   /* Exact copy made in our memory */

   return pointer;
};  /* temp goes out of scope and possibly messes up our copy */


template <class class_type>
class_type * hacked_approx_setup(class_type * pointer,
                                 int count, int realsize)
{
   for (int loop = 0; loop < count; loop++)
   {
      char * individual = ((char *)pointer) + loop * realsize;
      hacked_approx_setup((class_type *)individual, realsize);
   };
   return pointer;
};

template <class class_type>
class_type * clear_mem(class_type * pointer, int size)
{
   char * as_char_array = (char *)pointer;
   for (int loop = 0; loop < size; loop++)
   {
      as_char_array[loop] = 0;
   };
   return pointer;
};


template <class class_type>
inline class_type * pass_thru_assignment_check_in(class_type * any_variable_ptr,
                                       const char * filename,
                                       int          line_number,
                                       const char * variable_name,
                                       const char * class_type,
                                       int          size,
                                       int          count)
{
   check_in_assignment_after_malloc((void *)any_variable_ptr,
                                    filename, line_number,
                                    variable_name,
                                    class_type, size, count);
   return any_variable_ptr;
};


template <class class_type>
inline class_type * pass_thru_expression_check_in(class_type * any_variable_ptr,
                                       const char * filename,
                                       int          line_number,
                                       const char * class_type,
                                       int          size,
                                       int          count)
{
   check_in_expression_after_malloc((void *)any_variable_ptr,
                       filename, line_number,
                       class_type, size, count);
   return any_variable_ptr;
};

template <class class_type>
inline class_type * pass_thru_check_out(class_type * any_variable_ptr,
                                        const char * filename,
                                        int          line_number,
                                        const char * variable_name)
{
   check_out_before_free((void *)any_variable_ptr,
                          filename,
                          line_number,
                          variable_name);
   return any_variable_ptr;
};

#define my_new(class_type)                                                    \
    pass_thru_expression_check_in(                                            \
       hacked_approx_setup(                                                   \
          (class_type *)drop_thru_frail_malloc(sizeof(class_type),            \
                                             __FILE__,                        \
                                             __LINE__),                       \
           sizeof(class_type)                                                 \
       ),                                                                     \
       __FILE__, __LINE__, #class_type, sizeof(class_type), 1)

#define my_new_array(class_type, count)                                       \
    pass_thru_expression_check_in(                                            \
       hacked_approx_setup(                                                   \
          (class_type *)drop_thru_frail_malloc(sizeof(class_type) * count,    \
                                             __FILE__,                        \
                                             __LINE__),                       \
          count, sizeof(class_type)                                           \
       ),                                                                     \
       __FILE__, __LINE__, #class_type, sizeof(class_type), count)


#define my_delete(variable_name)                                              \
    clean_simulator_free(                                                     \
       pass_thru_check_out(variable_name,                                     \
                           __FILE__,                                          \
                           __LINE__,                                          \
                           #variable_name))


#define my_delete_array(variable_name)                                        \
    clean_simulator_free(                                                     \
       pass_thru_check_out(variable_name,                                     \
                           __FILE__,                                          \
                           __LINE__,                                          \
                           #variable_name))

#define my_frail_malloc(variable_name, class_type)                            \
{                                                                             \
    variable_name =                                                           \
    pass_thru_assignment_check_in(                                            \
       (class_type *)drop_thru_frail_malloc(sizeof(class_type),               \
                                          __FILE__,                           \
                                          __LINE__),                          \
        __FILE__, __LINE__, #variable_name, #class_type,                      \
        sizeof(class_type), 1);                                               \
}

#define my_frail_malloc_array(variable_name, class_type, count)               \
{                                                                             \
    variable_name =                                                           \
    pass_thru_assignment_check_in(                                            \
       (class_type *)drop_thru_frail_malloc(sizeof(class_type) * count,       \
                                          __FILE__,                           \
                                          __LINE__),                          \
        __FILE__, __LINE__, #variable_name, #class_type,                      \
        sizeof(class_type), count);                                           \
}

#define my_frail_calloc_array(variable_name, class_type, count)               \
{                                                                             \
    variable_name =                                                           \
    pass_thru_assignment_check_in(                                            \
       clear_mem(                                                             \
          (class_type *)drop_thru_frail_malloc(sizeof(class_type) * count,    \
                                             __FILE__,                        \
                                             __LINE__),                       \
          sizeof(class_type) * count),                                        \
       __FILE__, __LINE__, #variable_name, #class_type,                       \
       sizeof(class_type), count);                                            \
}

#define my_free(variable_name)                                                \
    clean_simulator_free(                                                     \
       pass_thru_check_out(variable_name,                                     \
                           __FILE__,                                          \
                           __LINE__,                                          \
                           #variable_name))


char * underlying_string_duplicate(const char * source,
                                   const char * filename,
                                   int line_number)
{
   int length = strlen(source) + 1;
   char * string_ptr = (char *)simulator_malloc(length);
   if (string_ptr == NULL)
   {
       terminate_due_to_memory_problem(filename, line_number);
   };
   memmove(string_ptr, source, length);
   check_in_expression_after_malloc((void *)string_ptr,
                       filename, line_number,
                       "string", length, 1);
   return string_ptr;
};

#define my_string_duplicate(source)                                          \
    underlying_string_duplicate(intended_string(source),                     \
                                __FILE__, __LINE__);

#endif

/* Might as well have a matching one of these */
#define my_string_free(variable_name)  my_free(variable_name)

#endif
