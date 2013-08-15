/* ***************************************************************
   ***************************************************************
   **                                                           **
   ** Simple hash table containing tree structures              **
   **                                                           **
   ** Author: Douglas Van Wieren                                **
   **                                                           **
   ** Description:                                              **
   **                                                           **
   **      This header file contains a veru simple structure    **
   ** suggested by Paul Wilson --- a quick and limited hash     **
   ** table whose substructures are trees.                      **
   **                                                           **
   ***************************************************************
   ***************************************************************

   $Source: /u/dvw/ss/sfkaplan/RCS/hash.h,v $
   $RCSfile: hash.h,v $
   $Revision: 2.1 $
   $Date: 1996/07/05 21:45:40 $
   $Author: user_dvw_dir_check_memory $
   $State: Exp $
   $Log: hash.h,v $
   Revision 2.1  1996/07/05 21:45:40  user_dvw_dir_check_memory
   Adjusted header and comments.

   $Locker: user_dvw_dir_trace_gatherers $

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
#ifndef dvw_hash_header
#define dvw_hash_header

#ifndef dvw_general_header
#include "general.h"
#endif
#ifndef dvw_avl_header
#include "avl.h"
#endif

#define NUMBER_OF_BUCKETS     (1024)
#define BUCKET_MASK           (NUMBER_OF_BUCKETS - 1)
#define EARLY_BITS_TO_IGNORE  (2)

template <class keytype, class datatype>
class general_hash_o_trees
{
public:
   typedef general_avl_tree<keytype, datatype> underlying_tree;
   underlying_tree bucket[NUMBER_OF_BUCKETS];

   typedef void (*simple_func)(keytype &, datatype &);
   typedef void (*print_func)(FILE *, keytype, datatype);

protected:
   int quick_hash(keytype address)
   {
      return (address >> EARLY_BITS_TO_IGNORE) & BUCKET_MASK;
   };

public:

   general_hash_o_trees()
   {
      reset();
   };

   void reset()
   {
      for (int loop = 0; loop < NUMBER_OF_BUCKETS; loop++)
      {
         bucket[loop].reset();
      };
   };

   void clean()
   {
      for (int loop = 0; loop < NUMBER_OF_BUCKETS; loop++)
      {
         bucket[loop].clean();
      };
   };

   UInt32 size()
   {
      UInt32 sum = 0;
      for (int loop = 0; loop < NUMBER_OF_BUCKETS; loop++)
      {
         sum += bucket[loop].size();
      };
      return sum;
   };

   void insert_entry(keytype key, datatype data)
   {

      bucket[quick_hash(key)].insert_entry(key, data);
   };

   datatype * locate_address_of_data(keytype key)
   {
      return bucket[quick_hash(key)].locate_address_of_data(key);
   };

   void apply_to_all(simple_func func)
   {
      for (int loop = 0; loop < NUMBER_OF_BUCKETS; loop++)
      {
         bucket[loop].apply_to_all(func);
      };
   };

   void print_all(FILE * output_stream, print_func func)
   {
      for (int loop = 0; loop < NUMBER_OF_BUCKETS; loop++)
      {
         bucket[loop].print_all(output_stream, func);
      };
   };

   void extract_entry(keytype key)
   {
      bucket[quick_hash(key)].extract_entry(key);
   };

   void kill()
   {
      for (int loop = 0; loop < NUMBER_OF_BUCKETS; loop++)
      {
         bucket[loop].kill();
      };
   };
};

template <class keytype, class datatype>
class hash_o_trees
{
public:
   typedef avl_tree<keytype, datatype> underlying_tree;
   underlying_tree bucket[NUMBER_OF_BUCKETS];

   typedef void (*simple_func)(keytype &, datatype &);
   typedef void (*print_func)(FILE *, keytype, datatype);

protected:
   int quick_hash(keytype address)
   {
      return (address >> EARLY_BITS_TO_IGNORE) & BUCKET_MASK;
   };

public:

   hash_o_trees()
   {
      reset();
   };

   void reset()
   {
      for (int loop = 0; loop < NUMBER_OF_BUCKETS; loop++)
      {
         bucket[loop].reset();
      };
   };

   void clean()
   {
      for (int loop = 0; loop < NUMBER_OF_BUCKETS; loop++)
      {
         bucket[loop].clean();
      };
   };

   UInt32 size()
   {
      UInt32 sum = 0;
      for (int loop = 0; loop < NUMBER_OF_BUCKETS; loop++)
      {
         sum += bucket[loop].size();
      };
      return sum;
   };

   void insert_entry(keytype key, datatype data)
   {

      bucket[quick_hash(key)].insert_entry(key, data);
   };

   datatype * locate_address_of_data(keytype key)
   {
      return bucket[quick_hash(key)].locate_address_of_data(key);
   };

   void apply_to_all(simple_func func)
   {
      for (int loop = 0; loop < NUMBER_OF_BUCKETS; loop++)
      {
         bucket[loop].apply_to_all(func);
      };
   };

   void print_all(FILE * output_stream, print_func func)
   {
      for (int loop = 0; loop < NUMBER_OF_BUCKETS; loop++)
      {
         bucket[loop].print_all(output_stream, func);
      };
   };

   void extract_entry(keytype key)
   {
      bucket[quick_hash(key)].extract_entry(key);
   };

   void kill()
   {
      for (int loop = 0; loop < NUMBER_OF_BUCKETS; loop++)
      {
         bucket[loop].kill();
      };
   };

/*
   New member functions for this class (compared to the general_hash_o_trees
   class).
*/

   void increment(keytype key)
   {
      bucket[quick_hash(key)].increment(key, (datatype)1);
   };

   void increment(keytype key, datatype amount)
   {
      bucket[quick_hash(key)].increment(key, amount);
   };

   datatype sum_of_entries()
   {
      datatype sum = 0;
      for (int loop = 0; loop < NUMBER_OF_BUCKETS; loop++)
      {
         sum += bucket[loop].sum_of_entries();
      };
      return sum;
   };

   attempt_flag find_key_extremes(keytype &  min_key,
                                  keytype &  max_key)
   {
      int loop = 0;
      while ((loop <= BUCKET_MASK) &&
             (bucket[loop].find_key_extremes(min_key, max_key) == FAILURE))
      {
         loop++;
      };
      if (loop >= BUCKET_MASK)
      {
         return FAILURE;
      }
      else
      {
         keytype min_candidate;
         keytype max_candidate;
         for (int loop2 = loop + 1; loop2 < NUMBER_OF_BUCKETS; loop2++)
         {
            if (bucket[loop2].find_key_extremes(min_candidate,
                                                max_candidate))
            {
               if (min_candidate < min_key)
               {
                  min_key = min_candidate;
               };
               if (max_candidate > max_key)
               {
                  max_key = max_candidate;
               };
            };
         };
         return SUCCESS;
      };
   };

   attempt_flag find_data_extremes(datatype &  min_data,
                                   datatype &  max_data)
   {
      int loop = 0;
      while ((loop <= BUCKET_MASK) &&
             (bucket[loop].find_data_extremes(min_data, max_data) == FAILURE))
      {
         loop++;
      };
      if (loop >= BUCKET_MASK)
      {
         return FAILURE;
      }
      else
      {
         datatype min_candidate;
         datatype max_candidate;
         for (int loop2 = loop + 1; loop2 < NUMBER_OF_BUCKETS; loop2++)
         {
            if (bucket[loop2].find_data_extremes(min_candidate,
                                                 max_candidate))
            {
               if (min_candidate < min_data)
               {
                  min_data = min_candidate;
               };
               if (max_candidate > max_data)
               {
                  max_data = max_candidate;
               };
            };
         };
         return SUCCESS;
      };
   };
};

/*
#undef NUMBER_OF_BUCKETS
#undef BUCKET_MASK
#undef EARLY_BITS_TO_IGNORE
*/
#endif
