/* ***************************************************************
   ***************************************************************
   **                                                           **
   ** Quick Hack made for Scott                                 **
   **                                                           **
   ** Author: Douglas Van Wieren                                **
   **                                                           **
   ** Description:                                              **
   **                                                           **
   **    Blah, blah                                             **
   **                                                           **
   ***************************************************************
   ***************************************************************

   $Source: /v/jive/v1/oops/locality-group/sfkaplan/RCS/quick.c,v $
   $RCSfile: quick.c,v $
   $Revision: 1.2 $
   $Date: 1996/07/13 02:49:57 $
   $Author: user_dvw_dir_trace_gatherers $
   $State: Exp $
   $Log: quick.c,v $
   Revision 1.2  1996/07/13 02:49:57  user_dvw_dir_trace_gatherers
   Adjusted comments.

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

#ifndef dvw_hash_header
#include "hash.h"
#endif

#ifndef dvw_inout_header
#include "inout.h"
#endif

typedef hash_o_trees<UInt32, UInt32> counting_mechanism;
counting_mechanism                   count_mech_inst;
typedef LRU_policy                   policy;
policy                               policy_inst;
UInt32                               address;
UInt32                               position;
 
void put_xy_triple(FILE * output_stream, UInt32 x_value, UInt32 y_value)
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
   policy_inst.reset();
   count_mech_inst.reset();
   while (get_one_hex_value(stdin, address) == SUCCESS)
   {
      policy_inst.record_touch(address, position);
      count_mech_inst.increment(position);
   };
   count_mech_inst.print_all(stdout, &put_xy_triple);
   policy_inst.kill();
   count_mech_inst.kill();
};
