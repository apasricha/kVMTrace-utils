/* ***************************************************************
   ***************************************************************
   **                                                           **
   ** Second Quick hack made for Scott                          **
   **                                                           **
   ** Author: Douglas Van Wieren                                **
   **                                                           **
   ** Description:                                              **
   **                                                           **
   **    Blah, blah                                             **
   **                                                           **
   ***************************************************************
   ***************************************************************

   $Source: /v/jive/v1/oops/locality-group/sfkaplan/RCS/quick_two.c,v $
   $RCSfile: quick_two.c,v $
   $Revision: 1.1 $
   $Date: 1996/07/13 02:49:57 $
   $Author: user_dvw_dir_trace_gatherers $
   $State: Exp $
   $Log: quick_two.c,v $
   Revision 1.1  1996/07/13 02:49:57  user_dvw_dir_trace_gatherers
   Initial revision

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
#ifndef dvw_LRU_mech_header
#include "LRU_mech.h"
#endif

#ifndef dvw_avl_header
#include "avl.h"
#endif

#ifndef dvw_inout_header
#include "inout.h"
#endif

typedef avl_tree<UInt32, UInt32> counting_mechanism;
counting_mechanism                   count_mech_inst;
UInt32                               address;
UInt32                               count;
 
UInt32                               running_sum;

void sum_to_right(UInt32 & x_value, UInt32 & y_value)
{
   deliberately_ignore_argument(x_value);
   running_sum += y_value;
   y_value = running_sum;
};

void invert_direction(UInt32 & x_value, UInt32 & y_value)
{
   deliberately_ignore_argument(x_value);
   y_value = running_sum - y_value;
};

void put_xy_double(FILE * output_stream, UInt32 x_value, UInt32 y_value)
{
/*
   put_UInt32(output_stream, x_value);
   put_space(output_stream);
   put_UInt32(output_stream, y_value);
   put_newline(output_stream);
*/
   fprintf(output_stream, "%ld %ld\n", x_value, y_value);
};

main()
{
   UInt32 maxsize = 0;
   count_mech_inst.reset();
   while (get_two_std_values(stdin, address, count) == SUCCESS)
   {
      count_mech_inst.increment(address, count);
      if (maxsize < address)
      { 
         maxsize = address;
      };
   };
   for (UInt32 loop = 0; loop <= maxsize + 1; loop ++)
   {
      count_mech_inst.increment(loop, 0);
   };
   running_sum = 0;
   count_mech_inst.apply_to_all(&sum_to_right);
   count_mech_inst.apply_to_all(&invert_direction);

   count_mech_inst.print_all(stdout, &put_xy_double);
   count_mech_inst.kill();
};
