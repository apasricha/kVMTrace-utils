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
   **                                                           **
   ***************************************************************
   *************************************************************** 

   $Source: /u/dvw/ss/sfkaplan/RCS/inout_static.h,v $
   $RCSfile: inout_static.h,v $
   $Revision: 1.1 $
   $Date: 1996/06/13 16:32:30 $
   $Author: dvw $
   $State: Exp $
   $Log: inout_static.h,v $
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
#ifndef dvw_inout_static_header
#define dvw_inout_static_header

#ifndef dvw_inout_header
#include "inout.h"
#endif

static void put_UInt32_UInt32_pair(FILE * output_stream,
                                   UInt32 key,
                                   UInt32 data)
{
   put_UInt32(output_stream, key);
   put_space(output_stream);
   put_UInt32(output_stream, data);
   put_newline(output_stream);
};

static void put_UInt32_UInt64_pair(FILE * output_stream,
                                   UInt32 key,
                                   UInt64 data)
{
   put_UInt32(output_stream, key);
   put_space(output_stream);
   put_UInt64(output_stream, data);
   put_newline(output_stream);
};

static void put_UInt64_UInt64_pair(FILE * output_stream,
                                   UInt64 key,
                                   UInt64 data)
{
   put_UInt64(output_stream, key);
   put_space(output_stream);
   put_UInt64(output_stream, data);
   put_newline(output_stream);
};
#endif
