/* ***************************************************************
   ***************************************************************
   **                                                           **
   ** General Stuff                                             **
   **                                                           **
   ** Author: Douglas Van Wieren                                **
   **                                                           **
   ** Description:                                              **
   **                                                           **
   **    This file contains things common enough to be included **
   ** in almost every project.                                  ** 
   **                                                           **
   ***************************************************************
   ***************************************************************

   $Source: /u/dvw/ss/sfkaplan/RCS/general.h,v $
   $RCSfile: general.h,v $
   $Revision: 2.1 $
   $Date: 1996/07/05 19:03:02 $
   $Author: dvw $
   $State: Exp $
   $Log: general.h,v $
   Revision 2.1  1996/07/05 19:03:02  dvw
   Adjusted header message.

   $Locker:  $

   Copyright (c) 1996

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
/*
   $Id: general.h,v 2.1 1996/07/05 19:03:02 dvw Exp $
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
#ifndef dvw_general_header
#define dvw_general_header
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <memory.h>
#include <math.h>
#include <unistd.h>

/* ***************************************************************
   ***************************************************************
   **                                                           **
   ** General Definitions                                       **
   **                                                           **
   ** Author: Douglas Van Wieren                                **
   ** Date:  November 1, 1995                                   **
   **                                                           **
   ** Description:                                              **
   **                                                           **
   **      This header file contains some very simple and       **
   ** extremely general routines to facilitate implementation   **
   ** of large structures and complex code.                     **
   **                                                           **
   **      In addition, there are general-purpose enumerated    **
   ** types and frequently-used (and presumably useful) macros. **
   **                                                           **
   **      The #define NCHECK will change the behavior of many  **
   ** items here --- reducing and eliminating error checking.   **
   **                                                           **
   ***************************************************************
   *************************************************************** */

/*
   Running under shade, we may have to redirect stderr, possibly to
   /device/null, so we cannot depend on having stderr available.
   Unfortunately, we can depend on bugs existing in *any* code.  So, we
   need a means of communicating error messages to the user.  Choosing
   a specific file and/or keeping a channel open is a bad idea since
   we may have several processes running at a given time and there may
   be name conflicts.  Using tmpnam() (see man entry) has it's own
   problems.  This is a reasonable compromise.  The process specifies
   a preferred name and we set an ordered list of fixed name alternates,
   and we open the first available file only when necessary.

   This gives the user a good chance to spot the error file by it's
   name, and gives us a reasonable of getting the message out without
   other problems.
*/

#ifdef DVW_SHADE_ANALYZER
#define ALT_ERROR_IS_REALLY_ALTERNATE
#endif

#ifdef ALT_ERROR_IS_REALLY_ALTERNATE
FILE * alterr                         = NULL;
char * alternate_error_filename       = NULL;

/*
   If alterr is already open, then the following code has no effect.

   If alterr is not open, and it becomes necessary to send messages
   there, then the first thing tried will be this file --- provided
   the pointer to it isn't NULL (either because the user mistakenly
   sent us a NULL pointer or because strdup failed).
*/

void set_alternate_error_filename(const char * filename)
{
   if (alternate_error_filename != NULL)
   {
      free(alternate_error_filename);
   };
   alternate_error_filename = strdup(filename);
};

void force_switch_error()
{
   if ((alterr != NULL) &&
       (alterr != stderr) &&
       (alternate_error_filename != NULL))
   {
      fclose(alterr);
   };
};

void paranoid_verify_error_initialization()
{
   alterr                         = NULL;
   alternate_error_filename       = NULL;
};

#else
#define alterr stderr
#endif

/*
   For some routines which we expect to call a lot during execution, it's
   a good idea to declare them as inline.  That's not the same as a routine
   where we expect lots of references in the code, but very few actual
   calls.  This is a good example.  References will be strewn throughout the
   code, but it'll most likely only be called a few times (just before the
   process aborts).  In fact, if we could declare this one as "outline" (with
   the obvious intended meaning), we would.

   Note also that this and the following routine will not return if alterr
   is not open.  Thus, a call to one of these insures that alterr is
   valid stream.
*/

static void error_msg()
{
#ifdef ALT_ERROR_IS_REALLY_ALTERNATE
   if ((alterr == NULL) &&
       ((alternate_error_filename == NULL) ||
        ((alterr = fopen(alternate_error_filename, "ab")) == NULL)) &&
       ((alterr = fopen("red_flag_0.err", "ab")) == NULL) &&
       ((alterr = fopen("red_flag_1.err", "ab")) == NULL) &&
       ((alterr = fopen("red_flag_2.err", "ab")) == NULL) &&
       ((alterr = fopen("red_flag_3.err", "ab")) == NULL) &&
       ((alterr = fopen("red_flag_4.err", "ab")) == NULL) &&
       ((alterr = fopen("red_flag_5.err", "ab")) == NULL) &&
       ((alterr = fopen("red_flag_6.err", "ab")) == NULL) &&
       ((alterr = fopen("red_flag_7.err", "ab")) == NULL))
   {
/*
   It is extremely unlikely that anyone will ever read this error
   message.  Particularly since it is sent to stderr, and if stderr
   were available for general error messages, this routine would be
   unnecessary.  Still, . . .  The thought counts.
*/
      fprintf(stderr, "While trying to write an error message\n");
      fprintf(stderr, "using the alternate error file protocol,\n");
      fprintf(stderr, "none of the usual file names could be opened\n");
      fprintf(stderr, "for update.\n");
      fprintf(stderr, "Aborting.\n");
      exit(1);
   }
   else
#endif
   {
      fputs("\a", alterr);
   };
};

static void error_msg(const char * msg)
{
#ifdef ALT_ERROR_IS_REALLY_ALTERNATE
   if ((alterr == NULL) &&
       ((alternate_error_filename == NULL) ||
        ((alterr = fopen(alternate_error_filename, "ab")) == NULL)) &&
       ((alterr = fopen("red_flag_0.err", "ab")) == NULL) &&
       ((alterr = fopen("red_flag_1.err", "ab")) == NULL) &&
       ((alterr = fopen("red_flag_2.err", "ab")) == NULL) &&
       ((alterr = fopen("red_flag_3.err", "ab")) == NULL) &&
       ((alterr = fopen("red_flag_4.err", "ab")) == NULL) &&
       ((alterr = fopen("red_flag_5.err", "ab")) == NULL) &&
       ((alterr = fopen("red_flag_6.err", "ab")) == NULL) &&
       ((alterr = fopen("red_flag_7.err", "ab")) == NULL))
   {
      fprintf(stderr, "While trying to write the error message:\n");
      fprintf(stderr, "\n   %s\n", msg);
      fprintf(stderr, "using the alternate error file protocol,\n");
      fprintf(stderr, "none of the usual file names could be opened\n");
      fprintf(stderr, "for update.\n");
      fprintf(stderr, "Aborting.\n");
      exit(1);
   }
   else
#endif
   {
      fputs(msg, alterr);
/* Yes, this is where that annoying bell code is */
#ifndef ALT_ERROR_IS_REALLY_ALTERNATE
      fputs("\a", alterr);
#endif
      fputs("\n", alterr);
      fflush(alterr);     /* Immediate flush because, if we're
                             reporting problems, things may die at
                             any moment */
   };
};

void alt_exit(int status)
{
   if ((alterr != NULL) || (status != 0))
   {
      char last_msg[80];
      sprintf(last_msg, "Program terminated with status %d\n", status);
      error_msg(last_msg);
#ifdef ALT_ERROR_IS_REALLY_ALTERNATE
      fclose(alterr);
#endif
   };
   exit(status);
};

inline const char * intended_string(const char * source)
{
   if (source != NULL)
   {
      return source;
   }
   else
   {
      return "";
   };
};

/*
   Now that we have a common of reporting errors, . . .
*/
#include "check_memory.h"

/*
   The prefixes frail, robust, and weak are intended to convey a sense of
   what's done with error checking for extremely common possible
   errors.  The phrase "common possible errors" can be interpreted
   two ways:

   For frail_ routines, the possibility of the error is
   common, the actual likelihood of an error is extremely small.
   We want to handle and acknowledge the possibility without writing
   repetitive statements in the code --- when recovering from the error
   is NOT feasible and determining the immediate cause of the error
   (as the location in the code where this routine was invoked) won't
   necessarily help us pinpoint the root cause.  So, if this this
   type of error occurs, we want the program to "break" at that point
   in the execution.  In fact, we can adjust coding style so that if
   a frail_ routine is called and it returns, we *know* the error
   condition does not exist.  Exhausted memory and missing files come
   easily to mind.

   For robust_ routines, the actual error is common --- either through
   mistakes in code or poorly designed general purpose routines.  Here,
   when there is trivial recovery and the root cause is unimportant, we
   simply want the program to recover and and move on efficiently.  Often,
   we won't care that the error is known NOT to occur in certain locations
   because checking is also trivial.  In fact, we often won't bother
   reporting the error because, in the presence of robust_ routines,
   we can make coding decisions which depend on the smooth and efficient
   recovery.  The easy example to trying to compare character pointers as
   strings where, for one reason or another, a character pointer might be
   null (and the intended meaning was a null string).

   For weak_ routines (a good example is in mystring.h and could not
   be separated to appear here --- weak_concat), the possible error(s)
   is(are) *deliberately* not checked.  In fact, careless usage may actually
   *cause* errors.  We're assuming that the arguments are pretested, the
   result is treated carefully and, in general, the usage is thought out well.
   (In the example, the result of a concat operation is a pointer to a
   common buffer.  If the buffer is not copied before the routine is
   invoked again, the contents are lost.)  The problems avoided here are
   those caused by needless string allocation (and bleeding memory) and/or
   wasted time (human and executable) in implementing more robust solutions
   which aren't strictly necessary.
*/


inline int robust_strcmp(const char * source1, const char * source2)
{
   return strcmp(intended_string(source1), intended_string(source2));
};

inline FILE * frail_open(const char * filename,
                         const char * open_code)
{
   FILE * return_dummy;
   if ((return_dummy = fopen(filename, open_code)) == NULL)
   {
      error_msg("Terminal error is file io.\n");
      fprintf(alterr, "Unable to open %s for io (%s).   Aborting.\n",
                      filename, open_code);
      alt_exit(1);
   };
   return return_dummy;
};

inline FILE * frail_reopen(const char * filename,
                           const char * open_code,
                           FILE * stdsomething)
{
   FILE * return_dummy;
   if ((return_dummy = freopen(filename, open_code, stdsomething)) == NULL)
   {
      error_msg("Terminal error is file io.\n");
      fprintf(alterr, "Unable to reopen fn %d as file %s (%s).   Aborting.\n",
                      fileno(stdsomething), filename, open_code);
      alt_exit(1);
   };
   return return_dummy;
};

/*
   When we have information which the user might want, but probably
   doesn't, we need to open a stream other than stderr, stdout, etc. for
   same reasons we wanted alterr.  Unlike the alterr stream, we're not
   specifying how and when this is to be opened.  We will, however, attempt to
   dump useless verbage on this channel if it is ever opened.

   A good example is the repetitive attempts to find proper files.
   This is interesting the first time you see it, it's mildly
   disturbing the second, and downright annoying afterwards.
*/
FILE * noise_stream                   = NULL;

inline FILE * verbose_attempt_open(const char * filename,
                                   const char * open_code)
{
   FILE * return_dummy;
   if ((return_dummy = fopen(filename, open_code)) == NULL)
   {
      if (noise_stream != NULL)
      {
         fprintf(noise_stream, "Unable to open %s for io (%s).\n",
                               filename, open_code);
      };
   }
   else
   {
      if (noise_stream != NULL)
      {
         fprintf(noise_stream, "Successfully opened %s for io (%s).\n",
                               filename, open_code);
      };
   };
   return return_dummy;
};

inline FILE * verbose_attempt_reopen(const char * filename,
                                     const char * open_code,
                                     FILE * stdsomething)
{
   FILE * return_dummy;
   if ((return_dummy = freopen(filename, open_code, stdsomething)) == NULL)
   {
      error_msg();
      fprintf(alterr, "Unable to reopen %s for io (%s).\n",
                      filename, open_code);
   }
   else
   {
      error_msg();
      fprintf(alterr, "Successfully reopened %s for io (%s).\n",
                      filename, open_code);
   };
   return return_dummy;
};

typedef signed   char          SInt8;
typedef unsigned char          UInt8;
typedef signed   short int     SInt16;
typedef unsigned short int     UInt16;
typedef signed   long int      SInt32;
typedef unsigned long int      UInt32;
typedef signed   long long int SInt64;
typedef unsigned long long int UInt64;
typedef double                 Float64;

template <class anytype>
inline void swap(anytype & A, anytype & B)
{
   anytype Save = A; A = B; B = Save;
};

template <class anytype>
inline int sign(anytype item)
{
   if (item < 0)
   {
      return -1;
   }
   else if (item == 0)
   {
      return 0;
   }
   else
   {
      return 1;
   };
};

template <class anytype>
inline anytype abs(anytype item)
{
   if (item > 0)
   {
      return item;
   }
   else if (item == 0)
   {
      return 0;
   }
   else
   {
      return -item;
   };
};

template <class anytype>
inline anytype min(anytype A, anytype B)
{
   return (A < B) ? A : B;
};

template <class anytype>
inline anytype max(anytype A, anytype B)
{
   return (A > B) ? A : B;
};

/*
   Common testing routines for internal arithmetic on numeric classes
   (Needed in testing only --- left commented out in general);

template<class anytype1, class anytype2>
anytype1 my_mod(anytype1 value1, anytype2 value2)
{
   if (value2 == 0)
   {
      error_msg("Warning:  bad internal arithmetic --- modulus zero.");
      return 0;
   }
   else
   {
      anytype1 bad_modulus = (anytype1)(value1 - value2 * (value1/value2));
      if ((bad_modulus == 0) || (sign(bad_modulus) == sign(value2)))
      {
         return bad_modulus;
      }
      else
      {
         return (anytype1)(bad_modulus + value2);
      };
   };
};

template<class anytype1, class anytype2>
anytype1 my_div(anytype1 value1, anytype2 value2)
{
   if (value2 == 0)
   {
      error_msg("Warning:  bad internal arithmetic --- division by zero.");
      return 0;
   }
   else
   {
      return (anytype1)((value1 - my_mod(value1, value2))/value2);
   };
};

template<class anytype1, class anytype2>
anytype1 my_revdiv(anytype1 value1, anytype2 value2)
{
   if (value2 == 0)
   {
      error_msg("Warning:  bad internal arithmetic --- division by zero.");
      return 0;
   }
   else
   {
      anytype1 temp = my_mod(value1, value2);
      if (temp != 0)
      {
         return (anytype1)((value1 + value2 - temp)/value2);
      }
      else
      {
         return (anytype1)(value1/value2);
      };
   };
};

template<class anytype1, class anytype2>
anytype1 my_gravity(anytype1 value1, anytype2 value2)
{
   if (value2 == 0)
   {
      return value1;
   }
   else
   {
      return (anytype1)(value2 * my_div(value1, value2));
   }
};
*/

/*
By making the following enumerated types, we can turn on and off range
checking with compiler commands --- compare this to

typedef int boolean;
#define LOGICAL_TRUE  (1)
#define LOGICAL_FALSE (0)

and the standard type bool.  Also, we can create meaning within
the boolean which isn't always clear from context --- the result of some
attempt, SUCCESS or FAILURE, is very different from a switch requirement,
ON or OFF.  By forcing ourselves to recognize the subtle differences with
explicit casts, we often save HUGE headaches in debugging.  At least,
that's my claim.
*/

#ifdef NCHECK
#define boolean bool
#define LOGICAL_TRUE  1
#define LOGICAL_FALSE 0
#else
enum boolean
{
   LOGICAL_TRUE  = 1,
   LOGICAL_FALSE = 0
};
#endif

/*
   Any reasonable compiler should be to completely remove these functions if
   attempt_flag is of type bool.
*/

inline boolean logical_true_when(int variable)
{
   return (boolean)((bool)variable);
};

inline boolean logical_false_when(int variable)
{
   return (boolean)(!(bool)(variable));
};

#ifdef NCHECK
#define attempt_flag bool
#define SUCCESS      1
#define FAILURE      0
#else
enum attempt_flag
{
   SUCCESS = 1,
   FAILURE = 0
};
#endif

/*
   Any reasonable compiler should be to completely remove these functions if
   attempt_flag is of type bool.
*/
inline attempt_flag success_if(int variable)
{
   return (attempt_flag)((bool)variable);
};

inline attempt_flag failure_if(int variable)
{
   return (attempt_flag)(!(bool)(variable));
};

inline attempt_flag success_if(boolean variable)
{
   return (attempt_flag)(variable);
};

inline attempt_flag failure_if(boolean variable)
{
   return (attempt_flag)(!(variable));
};

inline boolean successful(attempt_flag variable)
{
   return (boolean)(variable);
};

inline boolean unsuccessful(attempt_flag variable)
{
   return (boolean)(!(variable));
};


/*
   These other enum types carry even more meaning --- best to leave these
   as enums.
*/

enum relationship
{
   LESSTHAN    = -1,
   EQUAL       =  0,
   GREATERTHAN =  1,
   ERROR
};

enum change_flag
{
   DECREASED = -1,
   UNCHANGED =  0,
   INCREASED =  1
};

enum avl_status
{
   LEFT_TALLER  = -1,
   BALANCED     =  0,
   RIGHT_TALLER =  1
};

enum argument_constraint
{
   REQUIRED   = 1,
   OPTIONAL   = 0
};

enum record_directive
{
   RECORD      = 1,
   DONT_RECORD = 0
};

enum simple_orthogonal_direction
{
   HORIZONTAL,
   VERTICAL
};

template <class any_float>
inline any_float round(any_float arg)
{
   return floor(arg + 0.5);
};

template <class any_float>
inline any_float log2(any_float arg)
{
   return log(arg) * M_LOG2E;
};

template <class any_int>
inline void indent(FILE * output_stream, any_int level)
{
   if (level > 0)
   {
      for (any_int loop = 0; loop <= level; loop++)
      {
         fputs("   ", output_stream);
      };
   };
};

#ifdef NCHECK
#define deliberately_ignore_argument(ignored) {}
#else
#pragma -Wno-unused
inline void ignore(void * )
{
};
#pragma +Wno-unused

template <class anytype>
inline void deliberately_ignore_argument(anytype & ignored)
{
   ignore((void *) (&ignored));
};
#endif
#endif
