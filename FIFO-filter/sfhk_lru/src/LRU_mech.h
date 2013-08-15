/* ***************************************************************
   ***************************************************************
   **                                                           **
   ** LRU Mechanism Implementation                              **
   **                                                           **
   ** Author: Douglas Van Wieren                                **
   ** Date:  April 8, 1995   Revised:  November, 1995           **
   **                                                           **
   ** Description:                                              **
   **                                                           **
   **      This header file contains an LRU mechanism and a     **
   ** related structure based on an avl tree.                   **
   **                                                           **
   ***************************************************************
   *************************************************************** 

   $Source: /v/jive/v1/oops/locality-group/sfkaplan/RCS/LRU_mech.h,v $
   $RCSfile: LRU_mech.h,v $
   $Revision: 1.1 $
   $Date: 1996/06/13 16:32:30 $
   $Author: dvw $
   $State: Exp $
   $Log: LRU_mech.h,v $
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

#ifndef dvw_LRU_mech_header
#define dvw_LRU_mech_header

#ifndef dvw_general_header
#include "general.h"
#endif
#ifndef dvw_inout_header
#include "inout.h"
#endif
#ifndef dvw_inout_static_header
#include "inout_static.h"
#endif
#ifndef dvw_hash_header
#include "hash.h"
#endif

/*
   EXTENDED COMMENTS:

   Position Manipulation

   This is mechanism based on an avl tree; however, it's is quite a bit
   different than a standard avl tree in several ways:

       a)  Each node has a pointer to it's parent.
       b)  Each node contains a count of the number of nodes
           in the tree indicated by it left child.
       c)  We want to hand hand out pointers to nodes in this
           tree to external structures (thus, we cannot arbitrarily
           break the connection between an element of data and
           the node in which it is contained.
       d)  We mark the descendents of the root which can be found purely
           descending (recursively) into the left subtree.

   The reason for this is that the intention of the tree is different.
   Instead of merely containing data, allowing searchs, updates, etc.,
   it's purpose is record the relative postion of items in an LRU
   queue.

   Thus, the tree is ordered from left to right with the most recently
   seen items occuring to the left.

   The "insert" operation, to create a new node and place in the leftmost
   position, requires order log(n) time where n is the number of nodes in
   the tree.  The essential "look-up" operation is, given a pointer to a
   node within the tree, to find out it's postion, one plus the number of
   items existing to the left.  The "update" operation is to move a node
   to the leftmost position.  In other words, an external structure asks
   for the creation of and keeps "keys", the pointers to nodes within the
   tree, and asks for the current_position of a key and/or requests that
   a key be "moved" to the leftmost position.

   The time for a look-up operation is no worse than log(p) where p is
   the position of a node.  The movement operation is similarly log(p)
   worst case.  However, the operations which restore the balance of the
   tree *can be* log(n) where n is the total number of nodes in the
   tree.  In practice, it's more likely to a constant --- regardless of
   the patterns in the data accesses.  An amortized analysis is possible
   with many patterns, and some more reasoning should be done with
   "catastrophic patterns.

   A good comparison would to replace the balancing mechanism with that
   of a splay tree (items a), b), c), and d) would continue to apply).
   Without a firm notion of the probability of individual accesses, it
   would be difficult to make a strong comparison; however, supposing
   that the probability of accessing a particular key occurs with
   monotonically decreasing probability --- geometrically or exponentially
   decreasing, the splay would begin to resemble a linked list.  Given
   that "shape", the insert operation would become nearly constant; the
   update and move operations would be order p.  Although the implied
   probability distribution would give us a constant expected time, the
   same data model also gives us constant expected time for identical
   operations with the avl tree version.

   In any case, implementaing a splay version *seems* reasonable given
   certain well-conditioned patterns in the data, and there may be some
   potential for performance gain; however, certain ill-conditioned
   probabilities can make the behavior of a splay tree version *MUCH*
   worse than that of an avl tree.   Importantly, a simple loop can be
   used to demonstrate a "bad" pattern for splay trees while having only
   a very limited affect on the performance of avl trees.

   If time allows, a splay tree version should be implemented.  We don't
   expect a big win, but our understanding of the involved patterns is
   extremely limited and even a small difference in performance might
   yield interesting clues.

*/
template <class datatype>
class position_manipulation
{
public:

   class pm_node
   {
   public:
      datatype         data;

      UInt32           size_of_left_subtree;
      avl_status       node_avl_status;
      pm_node        * parent;
      pm_node        * left_child;
      pm_node        * right_child;
      boolean          left_primogenesis;
   };
   typedef void (*simple_func)(UInt32, datatype);
   typedef void (*print_func)(FILE *, UInt32, datatype);


/* global variables (for the routines in this class instantiation) */

   pm_node *     root;
   pm_node *     leftmost;
   UInt32        node_count;

   boolean       limit_number_of_nodes;
   UInt32        maximum_pos_number;

protected:
/*
   Recursive routines to check the assumptions on a valid
   position_manipulation tree.  See test_for_corruption for
   the base conditions regarding the root.
*/

   void check_parental_pointers_condition(pm_node * temp_root,
                                          pm_node * parent)
   {
      if (temp_root != NULL)
      {
         if (temp_root->parent != parent)
         {
            error_msg("Possible corruption in LRU mechanism\n");
            error_msg("Parental pointers not maintained\n");
            error_msg("Terminating prematurely \n");
            alt_exit(1);
         };
         check_parental_pointers_condition(temp_root->left_child, temp_root);
         check_parental_pointers_condition(temp_root->right_child, temp_root);
      };
   };

   void check_avl_condition(pm_node * temp_root,
                            UInt32 &  height)
   {
      if (temp_root == NULL)
      {
         height = 0;
      }
      else
      {
         UInt32 left_height;
         UInt32 right_height;
         check_avl_condition(temp_root->left_child, left_height);
         check_avl_condition(temp_root->right_child, right_height);
         boolean test;
         switch (temp_root->node_avl_status)
         {
            case LEFT_TALLER:
            {
               test   = (boolean)(left_height == right_height + 1);
               height = left_height + 1;
            }; break;
            case RIGHT_TALLER:
            {
               test   = (boolean)(left_height + 1 == right_height);
               height = right_height + 1;
            }; break;
            case BALANCED:
            {
               test   = (boolean)(left_height == right_height);
               height = right_height + 1;
            }; break;
            default:
            {
               test = LOGICAL_FALSE;
            }
         };
         if (!test)
         {
            error_msg("Possible corruption in LRU mechanism\n");
            error_msg("avl condition check failed\n");
            error_msg("Terminating prematurely \n");
            alt_exit(1);
         };
      };
   };

   void check_left_subtree_size_condition(pm_node * temp_root,
                                          UInt32 & weight)
   {
      if (temp_root == NULL)
      {
         weight = 0;
      }
      else
      {
         UInt32 left_weight;
         UInt32 right_weight;
         check_left_subtree_size_condition(temp_root->left_child,
                                           left_weight);
         check_left_subtree_size_condition(temp_root->right_child,
                                           right_weight);

         weight = left_weight + right_weight + 1;
         if  (temp_root->size_of_left_subtree != left_weight)
         {
            error_msg("Possible corruption in LRU mechanism\n");
            error_msg("Left weight condition not maintained.\n");
            error_msg("Terminating prematurely \n");
            alt_exit(1);
         };
      };
   };

   void check_left_primogenesis_condition(pm_node * temp_root,
                                          boolean expected)
   {
      if (temp_root != NULL)
      {
         if (expected != temp_root->left_primogenesis)
         {
            error_msg("Possible corruption in LRU mechanism\n");
            error_msg("Left primogenesis condition not maintained.\n");
            error_msg("Terminating prematurely \n");
            alt_exit(1);
         };
         check_left_primogenesis_condition(temp_root->left_child,
                                           temp_root->left_primogenesis);
         check_left_primogenesis_condition(temp_root->right_child,
                                           LOGICAL_FALSE);
      };
   };


/*
   Common bit of code
*/
   void replace_root(pm_node * new_root)
   {
      root = new_root;
      if (new_root == NULL) /* tree will be empty */
      {
         leftmost = NULL;
      }
      else
      {
         if (new_root->left_child == NULL)
         {
            leftmost = new_root;
         };
      };
   };
/*
   Routines to restore the avl condition.  Notice that these do include
   methods to *maintain* the proper values in the size_of_left_subtree
   fields, but NOT methods to *restore* those values if they become
   inaccurate.
*/

   void assert_left_parentage(pm_node * temp_root)
   {
      if (temp_root->left_child != NULL)
      {
         (temp_root->left_child)->parent = temp_root;
      };
   };

   void assert_right_parentage(pm_node * temp_root)
   {
      if (temp_root->right_child != NULL)
      {
         (temp_root->right_child)->parent = temp_root;
      };
   };

   void replace_child(pm_node * parent,
                      pm_node * old_child,
                      pm_node * new_child)
   {
      if (parent == NULL)
      {
         replace_root(new_child);
      }
      else if (parent->left_child == old_child)
      {
         parent->left_child = new_child;
      }
      else
      {
         parent->right_child = new_child;
      };
   };

   pm_node * rotate_right(pm_node * temp_root)
   {
      pm_node * save_root = temp_root;
      temp_root = temp_root->left_child;

      temp_root->parent = save_root->parent;
      replace_child(temp_root->parent, save_root, temp_root);

      save_root->left_child = temp_root->right_child;
      assert_left_parentage(save_root);

      temp_root->right_child = save_root;
      save_root->parent = temp_root;

      save_root->left_primogenesis = LOGICAL_FALSE;

      save_root->size_of_left_subtree = save_root->size_of_left_subtree
                                            - temp_root->size_of_left_subtree
                                            - 1;
      return temp_root;
   };

   pm_node * rotate_left(pm_node * temp_root)
   {
      pm_node * save_root = temp_root;
      temp_root = temp_root->right_child;

      temp_root->parent = save_root->parent;
      replace_child(temp_root->parent, save_root, temp_root);

      save_root->right_child = temp_root->left_child;
      assert_right_parentage(save_root);

      temp_root->left_child = save_root;
      save_root->parent = temp_root;

      temp_root->left_primogenesis = save_root->left_primogenesis;
      temp_root->size_of_left_subtree = save_root->size_of_left_subtree
                                            + temp_root->size_of_left_subtree
                                            + 1;
      return temp_root;
   };

   /* the following two routines assume a valid avl tree and
      return a valid avl tree of the same height without "heaviness"
      on a particular side. */

   void avoid_left_heavy(pm_node * temp_root)
   {
      if (temp_root->node_avl_status == LEFT_TALLER)
      {
         avoid_right_heavy(temp_root->left_child);
         temp_root = rotate_right(temp_root);
         if (temp_root->node_avl_status == BALANCED)
         {
            temp_root->node_avl_status = RIGHT_TALLER;
            (temp_root->right_child)->node_avl_status = BALANCED;
         }
         else
         {
            temp_root->node_avl_status = RIGHT_TALLER;
            (temp_root->right_child)->node_avl_status = RIGHT_TALLER;
         }
      };
   };

   void avoid_right_heavy(pm_node * temp_root)
   {
      if (temp_root->node_avl_status == RIGHT_TALLER)
      {
         avoid_left_heavy(temp_root->right_child);
         temp_root = rotate_left(temp_root);
         if (temp_root->node_avl_status == BALANCED)
         {
            temp_root->node_avl_status = LEFT_TALLER;
            (temp_root->left_child)->node_avl_status = BALANCED;
         }
         else
         {
            temp_root->node_avl_status = LEFT_TALLER;
            (temp_root->left_child)->node_avl_status = LEFT_TALLER;
         }
      };
   };

   pm_node * left_child_increased(pm_node * temp_root, change_flag & cf)
   {
      if (temp_root->node_avl_status == LEFT_TALLER)
      {
         avoid_right_heavy(temp_root->left_child);
         temp_root = rotate_right(temp_root);
         if (temp_root->node_avl_status == BALANCED)
         {
            temp_root->node_avl_status = RIGHT_TALLER;
            (temp_root->right_child)->node_avl_status = LEFT_TALLER;
            cf = INCREASED;
         }
         else /* (temp_root->increased == LEFT_TALLER) */
         {
            temp_root->node_avl_status = BALANCED;
            (temp_root)->right_child->node_avl_status = BALANCED;
            cf = UNCHANGED;
         };
      }
      else if (temp_root->node_avl_status == BALANCED)
      {
         temp_root->node_avl_status = LEFT_TALLER;
         cf = INCREASED;
      }
      else /* (temp_root->node_avl_status == RIGHT_TALLER) */
      {
         temp_root->node_avl_status = BALANCED;
         cf = UNCHANGED;
      };
      return temp_root;
   };

   /* Don't really need this one, but, for completeness . . . */
   pm_node * right_child_increased(pm_node * temp_root, change_flag & cf)
   {
      if (temp_root->node_avl_status == RIGHT_TALLER)
      {
         avoid_left_heavy(temp_root->right_child);
         temp_root = rotate_left(temp_root);
         if (temp_root->node_avl_status == BALANCED)
         {
            temp_root->node_avl_status = LEFT_TALLER;
            (temp_root->left_child)->node_avl_status = RIGHT_TALLER;
            cf = INCREASED;
         }
         else /* (temp_root == RIGHT_TALLER) */
         {
            temp_root->node_avl_status = BALANCED;
            (temp_root->left_child)->node_avl_status = BALANCED;
            cf = UNCHANGED;
         };
      }
      else if (temp_root->node_avl_status == BALANCED)
      {
         temp_root->node_avl_status = RIGHT_TALLER;
         cf = INCREASED;
      }
      else /* (temp_root->node_avl_status == LEFT_TALLER) */
      {
         temp_root->node_avl_status = BALANCED;
         cf = UNCHANGED;
      };
      return temp_root;
   };

   pm_node * left_child_decreased(pm_node * temp_root, change_flag & cf)
   {
      if (temp_root->node_avl_status == RIGHT_TALLER)
      {
         avoid_left_heavy(temp_root->right_child);
         temp_root = rotate_left(temp_root);
         if (temp_root->node_avl_status == BALANCED)
         {
            temp_root->node_avl_status = LEFT_TALLER;
            (temp_root->left_child)->node_avl_status = RIGHT_TALLER;
            cf = UNCHANGED;
         }
         else /* (temp_root == RIGHT_TALLER) */
         {
            temp_root->node_avl_status = BALANCED;
            (temp_root->left_child)->node_avl_status = BALANCED;
            cf = DECREASED;
         };
      }
      else if (temp_root->node_avl_status == BALANCED)
      {
         temp_root->node_avl_status = RIGHT_TALLER;
         cf = UNCHANGED;
      }
      else /* (temp_root->node_avl_status == LEFT_TALLER) */
      {
         temp_root->node_avl_status = BALANCED;
         cf = DECREASED;
      };
      return temp_root;
   };

   pm_node * right_child_decreased(pm_node * temp_root, change_flag & cf)
   {
      if (temp_root->node_avl_status == LEFT_TALLER)
      {
         avoid_right_heavy(temp_root->left_child);
         temp_root = rotate_right(temp_root);
         if (temp_root->node_avl_status == BALANCED)
         {
            temp_root->node_avl_status = RIGHT_TALLER;
            (temp_root->right_child)->node_avl_status = LEFT_TALLER;
            cf = UNCHANGED;
         }
         else /* (temp_root == LEFT_TALLER) */
         {
            temp_root->node_avl_status = BALANCED;
            (temp_root->right_child)->node_avl_status = BALANCED;
            cf = DECREASED;
         };
      }
      else if (temp_root->node_avl_status == BALANCED)
      {
         temp_root->node_avl_status = LEFT_TALLER;
         cf = UNCHANGED;
      }
      else /* (temp_root->node_avl_status == RIGHT_TALLER) */
      {
         temp_root->node_avl_status = BALANCED;
         cf = DECREASED;
      };
      return temp_root;
   };

   void height_increased(pm_node * start_here)
   {
      pm_node *   current       = start_here->parent;
      pm_node *   last          = start_here;
      change_flag height_change = INCREASED;

      while (height_change == INCREASED)
      {
         if (current == NULL)
         {
            height_change = UNCHANGED;
         }
         else if (current->left_child == last)
         {
            last = left_child_increased(current, height_change);
            current = last->parent;
         }
         else
         {
            last = right_child_increased(current, height_change);
            current = last->parent;
         };
      };
   };

   void height_decreased(pm_node * start_here)
   {
      pm_node *   current       = start_here->parent;
      pm_node *   last          = start_here;
      change_flag height_change = DECREASED;

      while (height_change == DECREASED)
      {
         if (current == NULL)
         {
            height_change = UNCHANGED;
         }
         else if (current->left_child == last)
         {
            last = left_child_decreased(current, height_change);
            current = last->parent;
         }
         else
         {
            last = right_child_decreased(current, height_change);
            current = last->parent;
         };
      };
   };

   void leftmost_height_increased()
   {
      pm_node *   current       = leftmost->parent;
      change_flag height_change = INCREASED;

      while (height_change == INCREASED)
      {
         if (current == NULL)
         {
            height_change = UNCHANGED;
         }
         else
         {
            current = (left_child_increased(current, height_change))->parent;
         };
      };
   };


/*
   Separately, these routines prepare to restore the values in the
   size_of_left_subtree fields to be correct after a deletion
   or a move.

   Assumption:  the starting location is non-NULL
*/

   void prepare_deletion(pm_node * start_here)
   {
      pm_node *   current       = start_here->parent;
      pm_node *   last          = start_here;
      while (current != NULL)
      {
         if (current->left_child == last)
         {
            current->size_of_left_subtree--;
         };
         last = current;
         current = current->parent;
      };
   };

   void prepare_move_to_front(pm_node * start_here)
   {
      pm_node *   current       = start_here->parent;
      pm_node *   last          = start_here;
      if (current != NULL)
      {
         while (!(current->left_primogenesis))
         {
            if (current->left_child == last)
            {
               current->size_of_left_subtree--;
            };
            last = current;
            current = current->parent;
         };
         if (current->right_child == last)
         {
            current->size_of_left_subtree++;
         };
      }
      else
      {
         current = start_here;
      };

      current = current->left_child;
      while (current != NULL)
      {
         current->size_of_left_subtree++;
         current = current->left_child;
      };
   };

/*
   General purpose routines
*/
   pm_node * find_leftmost(pm_node * temp_root)
   {
      if ((temp_root != NULL) && (temp_root->left_child != NULL))
      {
         pm_node * temp_leftmost = temp_root->left_child;
         while (temp_leftmost->left_child != NULL)
         {
            temp_leftmost = temp_leftmost->left_child;
         };
         return temp_leftmost;
      }
      else
      {
         return temp_root;
      };
   };

   pm_node * find_rightmost(pm_node * temp_root)
   {
      if ((temp_root != NULL) && (temp_root->right_child != NULL))
      {
         pm_node * temp_rightmost = temp_root->right_child;
         while (temp_rightmost->right_child != NULL)
         {
            temp_rightmost = temp_rightmost->right_child;
         };
         return temp_rightmost;
      }
      else
      {
         return temp_root;
      };
   };

/*
   Assumes that this_node and that_node are not adjacent in the
   tree structure.  If either is leftmost, then this routine
   will *NOT* adjust the value of leftmost properly.
*/
   void swap_positions(pm_node * this_node,
                       pm_node * that_node)
   {
      swap(that_node->size_of_left_subtree, this_node->size_of_left_subtree);
      swap(that_node->parent,               this_node->parent);
      swap(that_node->left_child,           this_node->left_child);
      swap(that_node->right_child,          this_node->right_child);
      swap(that_node->node_avl_status,      this_node->node_avl_status);
      swap(that_node->left_primogenesis,    this_node->left_primogenesis);
      replace_child(that_node->parent, this_node, that_node);
      assert_left_parentage(that_node);
      assert_right_parentage(that_node);
      replace_child(this_node->parent, that_node, this_node);
      assert_left_parentage(this_node);
      assert_right_parentage(this_node);
   };


/*
   Routines to extract a node from the tree structure and subsequently
   move it to the leftmost position
*/

   void attach_to_front(pm_node * extracted_node)
   {
      extracted_node->size_of_left_subtree = 0;
      extracted_node->node_avl_status      = BALANCED;
      extracted_node->parent               = leftmost;
      extracted_node->left_child           = NULL;
      extracted_node->right_child          = NULL;
      extracted_node->left_primogenesis    = LOGICAL_TRUE;
      if (leftmost == NULL)
      {
         replace_root(extracted_node);
      }
      else
      {
         leftmost->left_child = extracted_node;
         leftmost = extracted_node;
         leftmost_height_increased();
      };
   };

/*
   this_node is *any* non-NULL node
   this_node->left_child assumed to be NULL
   this_node->right_child assumed to be NULL
*/
   void extract_and_move_leaf(pm_node * this_node)
   {
      if (this_node->parent != NULL)
      {
         pm_node *   parent        = this_node->parent;
         change_flag height_change;

         prepare_move_to_front(this_node);
         if (parent->right_child == this_node)
         {
            parent->right_child = NULL;
            parent = right_child_decreased(parent, height_change);
         }
         else
         {
            parent->left_child = NULL;
            parent = left_child_decreased(parent, height_change);
         };
         if (height_change == DECREASED)
         {
            height_decreased(parent);
         };
         attach_to_front(this_node);
      }; /* else the leaf is the root and there's nothing to do */
   };

/*
   this_node is *any* non-NULL node
   child is one of {this_node->left_child, this_node->right_child}
   and assumed to be non-NULL, the other child is assumed to be NULL
*/
   void extract_and_move_child_inherits(pm_node * this_node, pm_node * child)
   {
      prepare_move_to_front(this_node);
      replace_child(this_node->parent, this_node, child);
      child->parent = this_node->parent;
      height_decreased(child);
      attach_to_front(this_node);
   };


/*
   Now, if this_node has non-NULL children and is not the leftmost node,
   then we can find an immediate neighbor in the overall order to
   move to the front and subsequently swap positions with.

   Extracting and moving that node insures that this_node and
   that node cannot be adjacent in the tree.  Hence, . . .
*/

   void extract_and_move(pm_node * this_node)
   {
      if (this_node->right_child == NULL)
      {
         if (this_node->left_child == NULL)
         {
            extract_and_move_leaf(this_node);
         }
         else
         {
            extract_and_move_child_inherits(this_node, this_node->left_child);
         };
      }
      else if (this_node->left_child == NULL)
      {
         extract_and_move_child_inherits(this_node, this_node->right_child);
      }
      else
      {
         pm_node * surrogate;
         if (this_node->node_avl_status == LEFT_TALLER)
         {
            surrogate = find_rightmost(this_node->left_child);
         }
         else
         {
            surrogate = find_leftmost(this_node->right_child);
         };
         extract_and_move(surrogate);
         swap_positions(this_node, surrogate);
         leftmost = this_node;
      };
   };

/*
   Deletion is a bit different, . . .
*/

   void extract_and_delete_rightmost(datatype & dead_data_entry)
   {
      pm_node * rightmost = find_rightmost(root);
      if (rightmost != NULL)
      {
         if (rightmost->parent == NULL)
         {
            replace_root(rightmost->left_child);
            if (rightmost->left_child != NULL)
            {
               (rightmost->left_child)->parent = NULL;
            }
            else
            {
               leftmost = NULL;
            };
         }
         else
         {
            if (rightmost->left_child == NULL)
            {
               (rightmost->parent)->right_child = NULL;
            }
            else
            {
               (rightmost->parent)->right_child = rightmost->left_child;
               (rightmost->left_child)->parent  = rightmost->parent;
            };
            change_flag cf;
            pm_node * temp; 
            temp = right_child_decreased(rightmost-> parent, cf);
            if (cf == DECREASED)
            {
               height_decreased(temp);
            };
         };
         dead_data_entry = rightmost->data;
         my_free(rightmost);
      };
   };

   void apply_to_all_in_tree(pm_node *    temp_root,
                             UInt32       external_nodes_to_the_left,
                             simple_func func)
   {
      if (temp_root != NULL)
      {
         UInt32 position = external_nodes_to_the_left +
                         temp_root->size_of_left_subtree + 1;
         apply_to_all_in_tree(temp_root->left_child,
                              external_nodes_to_the_left,
                              func);
         (*func)(position, temp_root->data);
         apply_to_all_in_tree(temp_root->right_child,
                              position,
                              func);
      };
   };

   void apply_to_all_in_tree_reversed(pm_node *    temp_root,
                                      UInt32       external_nodes_to_the_left,
                                      simple_func func)
   {
      if (temp_root != NULL)
      {
         UInt32 position = external_nodes_to_the_left +
                         temp_root->size_of_left_subtree + 1;
         apply_to_all_in_tree_reversed(temp_root->right_child,
                                       position, func);
         (*func)(position, temp_root->data);
         apply_to_all_in_tree_reversed(temp_root->left_child,
                                       external_nodes_to_the_left,
                                       func);
      };
   };

   void print_all_in_tree(FILE *     output_stream,
                          pm_node *  temp_root,
                          UInt32     external_nodes_to_the_left,
                          print_func func)
   {
      if (temp_root != NULL)
      {
         UInt32 position = external_nodes_to_the_left +
                         temp_root->size_of_left_subtree + 1;
         print_all_in_tree(output_stream,
                           temp_root->left_child,
                           external_nodes_to_the_left,
                           func);
         (*func)(output_stream, position, temp_root->data);
         print_all_in_tree(output_stream,
                           temp_root->right_child,
                           position,
                           func);
      };
   };

   void print_all_in_tree_reversed(FILE *       output_stream,
                                   pm_node *    temp_root,
                                   UInt32       external_nodes_to_the_left,
                                   print_func   func)
   {
      if (temp_root != NULL)
      {
         UInt32 position = external_nodes_to_the_left +
                           temp_root->size_of_left_subtree + 1;
         print_all_in_tree_reversed(output_stream,
                                    temp_root->right_child,
                                    position,
                                    func);
         (*func)(output_stream, position, temp_root->data);
         print_all_in_tree_reversed(output_stream,
                                    temp_root->left_child,
                                    external_nodes_to_the_left,
                                    func);
      };
   };

   void kill_tree(pm_node * & temp_root)
   {
      if (temp_root != NULL)
      {
         kill_tree(temp_root->left_child);
         kill_tree(temp_root->right_child);
         my_free(temp_root);
      };
   };

public:
   position_manipulation()
   {
      reset();
   };

   void reset()
   {
      node_count         = 0;
      root               = NULL;
      leftmost           = NULL;

      limit_number_of_nodes = LOGICAL_FALSE;
   };

   void clean()
   {
      kill();
      reset();
   };

   void set_limit(UInt32       max)
   {
      limit_number_of_nodes = LOGICAL_TRUE;
      maximum_pos_number    = max;
   };

   pm_node * make_new(datatype & data,
                      boolean  & node_died,
                      datatype & dead_data_entry)
   {
      pm_node * new_node_ptr;
      my_frail_malloc(new_node_ptr, pm_node);
      new_node_ptr->data = data;

      pm_node * current    = leftmost;
      while (current != NULL)
      {
         current->size_of_left_subtree++;
         current = current->parent;
      };
      attach_to_front(new_node_ptr);
      node_count++;

      if ((limit_number_of_nodes) &&
          (node_count > maximum_pos_number))
      {
         node_died = LOGICAL_TRUE;
         extract_and_delete_rightmost(dead_data_entry);
         node_count--;
      }
      else
      {
         node_died = LOGICAL_FALSE;
      };
      return new_node_ptr;
   };

   UInt32 query_position(pm_node * touched_node)
   {
      UInt32 position = touched_node->size_of_left_subtree + 1;
      if (touched_node->left_primogenesis != LOGICAL_TRUE)
      {
         pm_node * current  = touched_node->parent;
         pm_node * last     = touched_node;
         while (last->left_primogenesis != LOGICAL_TRUE)
         {
            if (current == NULL)
            {
               error_msg("Something wrong with position_tree\n");
               test_for_corruption();
               alt_exit(1);
            };
            if (current->right_child == last)
            {
               position = position + current->size_of_left_subtree + 1;
            };
            last     = current;
            current  = current->parent;
         };
      };
      return position;
   };

   void move_to_front(pm_node * touched_node)
   {
      if (touched_node != leftmost)
      {
         extract_and_move(touched_node);
      };
   };

   void apply_to_all(simple_func func)
   {
      apply_to_all_in_tree(root, 0, func);
   };

   void apply_to_all_reversed(simple_func func)
   {
      apply_to_all_in_tree_reversed(root, 0, func);
   };

   void print_all(FILE * output_stream, print_func func)
   {
      print_all_in_tree(output_stream, root, 0, func);
   };

   void print_all_reversed(FILE * output_stream, print_func func)
   {
      print_all_in_tree_reversed(output_stream, root, 0, func);
   };

/*
    We suppose that handing out pointers to things inside the structure
    may lead to corruption from an external source (i.e. a "bad"
    node is given back to us, and unpredictable things may happen).
    So, we want to check the tree completely before releasing it.

    Thus, if the tree *does* become corrupted, then we have a fair
    chance of finding the error without unsightly costs.
*/

   void test_for_corruption()
   {
      UInt32 height;
      UInt32 weight;
      check_parental_pointers_condition(root, NULL);
      check_avl_condition(root, height);
      check_left_subtree_size_condition(root, weight);
      check_left_primogenesis_condition(root, LOGICAL_TRUE);
      if (weight != node_count)
      {
         error_msg("Possible corruption in LRU mechanism\n");
         error_msg("Node count of whole tree is off.\n");
         error_msg("Terminating prematurely \n");
         alt_exit(1);
      };
      if (root != NULL)
      {
         if (leftmost != find_leftmost(root))
         {
            error_msg("Possible corruption in LRU mechanism\n");
            error_msg("leftmost pointer not set correctly.\n");
            error_msg("Terminating prematurely \n");
            alt_exit(1);
         };
      };
   };

protected:
   void diagnostic_print_node(FILE * output_stream, pm_node * temp_root)
   {
      if (temp_root == NULL)
      {
         fprintf(output_stream, "NULL ");
      }
      else
      {
         fprintf(output_stream, "%ld ", temp_root->data);
         fprintf(output_stream, "(slst %ld) ",
                         temp_root->size_of_left_subtree);
         if (temp_root->node_avl_status == LEFT_TALLER)
         {
            fprintf(output_stream, "L_T ");
         }
         else if (temp_root->node_avl_status == BALANCED)
         {
            fprintf(output_stream, "BAL ");
         }
         else if (temp_root->node_avl_status == RIGHT_TALLER)
         {
            fprintf(output_stream, "R_T ");
         }
         else
         {
            fprintf(output_stream, "avl screwed up ");
         };
         if (temp_root->left_primogenesis == LOGICAL_TRUE)
         {
            fprintf(output_stream, "LP ");
         }
         else if (temp_root->left_primogenesis == LOGICAL_FALSE)
         {
            fprintf(output_stream, "!LP ");
         }
         else
         {
            fprintf(output_stream, "LP screwed up ");
         };
      };
   };

   void diagnostic_print(FILE * output_stream, pm_node * temp_root, int depth)
   {
      if ((temp_root != NULL) && (depth < 20))
      {
         diagnostic_print(output_stream, temp_root->left_child, depth + 1);
         indent(output_stream, depth);
         diagnostic_print_node(output_stream, temp_root);
         fprintf(output_stream, "(P: ");
         diagnostic_print_node(output_stream, temp_root->parent);
         fprintf(output_stream, ")\n");
         diagnostic_print(output_stream, temp_root->right_child, depth + 1);
      };
   };

public:
   void diagnostic_print(FILE * output_stream, const char * temp)
   {
      fprintf(output_stream, "========================= %s \n", temp);
      diagnostic_print(output_stream, root,  1);
      fprintf(output_stream, "========================= \n");
      diagnostic_print(output_stream, leftmost,  1);
      fprintf(output_stream, "========================= \n");
      test_for_corruption();
   };
   void kill()
   {
      test_for_corruption();
      kill_tree(root);
   };
};

/*  "Old" comments:

    Least Recently Used Policy Mechanism

    This class implements some of the behavior of the least recently
    eviction policy for all sizes of physical memory up to a
    user-provided limit.  In particular, given page numbers in sequence,
    it specifies, for each page number "touch", what position that page
    would have had  at that time.

    Here, the position with respect to this policy refers to an unsigned
    integer  value.  Returned, it means that, for all sizes of physical
    memory less than that value, this policy would have had a page-fault.

    The least-recently-used policy dictates that whenever a page must be
    faulted into physical memory, the page which is evicted is one which
    has not been not for the longest time.

    Because this policy is inclusive, the set of pages kept by
    this policy when the size of physical memory is N is a subset
    of those kept when the size is N+1.  This allows us to uniquely
    identify all pages which have been touched before by a position
    number.  (There are positions beyond our ability to care, and we
    lump those pages together with those which have never been seen
    before.)

    Restating, the position number associated with a page number
    at a particular time, indicates that the page is in physical memory
    if and only if the size of physical memory is greater than the
    position number.  We may think of these positions as being indexes
    into an array, positions in a linked list, etc.  However, it is
    important not to confuse these abstractions with the structures actually
    used to implement the mechanism.

    When each page is touched, it moves to the first position (no matter
    what the size of physical memory is, this page must in physical
    memory).  This policy dictates that it moves in the following way:

        Assume that the page being touched is page N and page N has
        position P.  (This is the value returned by the record_touch
        member and it means that, "If the size of physical memory
        is P or greater, there are no page faults.")

        Page N moves to position 1, and the all the pages originally
        in positions 1 through P - 1 move to the next higher position.


*/

class LRU_policy
{
protected:
   typedef position_manipulation<UInt32>::pm_node * pos_key;
   position_manipulation<UInt32>                    pos_structure;
   general_hash_o_trees<UInt32, pos_key>            page_structure;

   void new_page(UInt32 page)
   {
      UInt32 dead_page;
      boolean page_died;

      pos_key new_pos_key = pos_structure.make_new(page,
                                                   page_died,
                                                   dead_page);
      if (page_died)
      {
         page_structure.extract_entry(dead_page);
      };
      page_structure.insert_entry(page, new_pos_key);
   };

public:
   LRU_policy()
   {
      reset();
   };

   void set_limit(UInt32 max_size)
   {
      pos_structure.set_limit(max_size);
   };

   void reset()
   {
      pos_structure.reset();
      page_structure.reset();
   };

   void clean()
   {
      pos_structure.clean();
      page_structure.clean();
   };

   void kill()
   {
      pos_structure.reset();
      page_structure.reset();
   };


   UInt32 query_position(UInt32 page)
   {
      pos_key * pos_key_ptr = page_structure.locate_address_of_data(page);
      if (pos_key_ptr == NULL)
      {
         return 0;
      }
      else
      {
         UInt32 temp = pos_structure.query_position(*pos_key_ptr);
         return temp;
      };
   };

   void record_touch(UInt32   page,
                     UInt32 & position)
   {
      pos_key * pos_key_ptr = page_structure.locate_address_of_data(page);
      if (pos_key_ptr == NULL)
      {
         new_page(page);
         position = 0;
      }
      else
      {
         position = pos_structure.query_position(*pos_key_ptr);
         pos_structure.move_to_front(*pos_key_ptr);
      };
   };

   void save_state(FILE * output_stream)
   {
      put_UInt32(output_stream, pos_structure.node_count);
      fprintf(output_stream, " (hex) values in LRU-state\n");
      pos_structure.print_all_reversed(output_stream, &put_UInt32_UInt32_pair);
   };

   void restore_state(FILE * input_stream)
   {
      UInt32 number_of_nodes;
      if (get_one_hex_value(input_stream, number_of_nodes) != SUCCESS)
      {
         error_msg("Unable to read state --- file not in expected\n");
         error_msg("format.  Missing/corrupted size.  Aborting.\n");
         alt_exit(1);
      }
      else if (number_of_nodes > 0)
      {
         UInt32 loop;
         UInt32 position;
         UInt32 page;
         for (loop = number_of_nodes; loop > 0; loop--)
         {
            if (get_two_hex_values(input_stream, position, page) != SUCCESS)
            {
               error_msg("Unable to read state --- file not in expected\n");
               error_msg("format.  Missing/corrupted entry.  Aborting.\n");
               alt_exit(1);
            };
            if (position != loop)
            {
               error_msg("Unable to read state --- file not in expected\n");
               error_msg("format.  Order corrupted.  Aborting.\n");
               alt_exit(1);
            };
            new_page(page);
         };
      };
   };

};

#endif
