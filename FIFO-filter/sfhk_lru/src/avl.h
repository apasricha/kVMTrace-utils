/* ***************************************************************
   ***************************************************************
   **                                                           **
   ** Simple avl-tree Structures                                **
   **                                                           **
   ** Author: Douglas Van Wieren                                **
   **                                                           **
   ** Description:                                              **
   **                                                           **
   **      This header file contains some very simple           **
   ** structures based on avl trees.                            **
   **                                                           **
   ***************************************************************
   ***************************************************************

   $Source: /v/jive/v1/oops/locality-group/sfkaplan/RCS/avl.h,v $
   $RCSfile: avl.h,v $
   $Revision: 2.1 $
   $Date: 1996/06/24 19:44:17 $
   $Author: dvw $
   $State: Exp $
   $Log: avl.h,v $
   Revision 2.1  1996/06/24 19:44:17  dvw
   Adjusted typo in comments

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
#ifndef dvw_avl_header
#define dvw_avl_header

#ifndef dvw_general_header
#include "general.h"
#endif

template <class keytype, class datatype>
class general_avl_tree
{
public:
   class avl_tree_node
   {
   public:
      avl_tree_node *  right_child;
      avl_tree_node *  left_child;
      keytype          key;
      datatype         data;
      avl_status       node_avl_status;
   };
   avl_tree_node * root;
   unsigned long int internal_size;
   typedef void (*simple_func)(keytype &, datatype &);
   typedef void (*print_func)(FILE *, keytype, datatype);

   unsigned long int size()
   {
      return internal_size;
   };

protected:
   void rotate_right(avl_tree_node * & temp_root)
   {
      avl_tree_node * save_root = temp_root;
      temp_root              = temp_root->left_child;
      save_root->left_child   = temp_root->right_child;
      temp_root->right_child  = save_root;
   };
   void rotate_left(avl_tree_node * & temp_root)
   {
      avl_tree_node * save_root = temp_root;
      temp_root              = temp_root->right_child;
      save_root->right_child  = temp_root->left_child;
      temp_root->left_child   = save_root;
   };
   /* the following two routines assume a valid avl tree and
      return a valid avl tree of the same height without "heaviness"
      on a particular side. */
   void avoid_left_heavy(avl_tree_node * & temp_root)
   {
      if (temp_root->node_avl_status == LEFT_TALLER)
      {
         avoid_right_heavy(temp_root->left_child);
         rotate_right(temp_root);
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
   void avoid_right_heavy(avl_tree_node * & temp_root)
   {
      if (temp_root->node_avl_status == RIGHT_TALLER)
      {
         avoid_left_heavy(temp_root->right_child);
         rotate_left(temp_root);
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

   /* these routines return a flag if the tree has grown in height */
   change_flag left_child_increased(avl_tree_node * & temp_root)
   {
      if (temp_root->node_avl_status == LEFT_TALLER)
      {
         avoid_right_heavy(temp_root->left_child);
         rotate_right(temp_root);
         if (temp_root->node_avl_status == BALANCED)
         {
            temp_root->node_avl_status               = RIGHT_TALLER;
            (temp_root->right_child)->node_avl_status = LEFT_TALLER;
            return INCREASED;
         }
         else /* (temp_root == LEFT_TALLER) */
         {
            temp_root->node_avl_status               = BALANCED;
            (temp_root)->right_child->node_avl_status = BALANCED;
            return UNCHANGED;
         };
      }
      else if (temp_root->node_avl_status == BALANCED)
      {
         temp_root->node_avl_status = LEFT_TALLER;
         return INCREASED;
      }
      else /* (temp_root->node_avl_status == RIGHT_TALLER) */
      {
         temp_root->node_avl_status = BALANCED;
         return UNCHANGED;
      };
   };

   change_flag right_child_increased(avl_tree_node * & temp_root)
   {
      if (temp_root->node_avl_status == RIGHT_TALLER)
      {
         avoid_left_heavy(temp_root->right_child);
         rotate_left(temp_root);
         if (temp_root->node_avl_status == BALANCED)
         {
            temp_root->node_avl_status              = LEFT_TALLER;
            (temp_root->left_child)->node_avl_status = RIGHT_TALLER;
            return INCREASED;
         }
         else /* (temp_root == RIGHT_TALLER) */
         {
            temp_root->node_avl_status              = BALANCED;
            (temp_root->left_child)->node_avl_status = BALANCED;
            return UNCHANGED;
         };
      }
      else if (temp_root->node_avl_status == BALANCED)
      {
         temp_root->node_avl_status = RIGHT_TALLER;
         return INCREASED;
      }
      else /* (temp_root->node_avl_status == LEFT_TALLER) */
      {
         temp_root->node_avl_status = BALANCED;
         return UNCHANGED;
      };
   };

   change_flag left_child_decreased(avl_tree_node * & temp_root)
   {
      if (temp_root->node_avl_status == RIGHT_TALLER)
      {
         avoid_left_heavy(temp_root->right_child);
         rotate_left(temp_root);
         if (temp_root->node_avl_status == BALANCED)
         {
            temp_root->node_avl_status              = LEFT_TALLER;
            (temp_root->left_child)->node_avl_status = RIGHT_TALLER;
            return UNCHANGED;
         }
         else /* (temp_root == RIGHT_TALLER) */
         {
            temp_root->node_avl_status              = BALANCED;
            (temp_root->left_child)->node_avl_status = BALANCED;
            return DECREASED;
         };
      }
      else if (temp_root->node_avl_status == BALANCED)
      {
         temp_root->node_avl_status = RIGHT_TALLER;
         return UNCHANGED;
      }
      else /* (temp_root->node_avl_status == LEFT_TALLER) */
      {
         temp_root->node_avl_status = BALANCED;
         return DECREASED;
      };
   };

   change_flag right_child_decreased(avl_tree_node * & temp_root)
   {
      if (temp_root->node_avl_status == LEFT_TALLER)
      {
         avoid_right_heavy(temp_root->left_child);
         rotate_right(temp_root);
         if (temp_root->node_avl_status == BALANCED)
         {
            temp_root->node_avl_status               = RIGHT_TALLER;
            (temp_root->right_child)->node_avl_status = LEFT_TALLER;
            return UNCHANGED;
         }
         else /* (temp_root == LEFT_TALLER) */
         {
            temp_root->node_avl_status               = BALANCED;
            (temp_root->right_child)->node_avl_status = BALANCED;
            return DECREASED;
         };
      }
      else if (temp_root->node_avl_status == BALANCED)
      {
         temp_root->node_avl_status = LEFT_TALLER;
         return UNCHANGED;
      }
      else /* (temp_root->node_avl_status == RIGHT_TALLER) */
      {
         temp_root->node_avl_status = BALANCED;
         return DECREASED;
      };
   };

   change_flag insert_tree(avl_tree_node * & temp_root, keytype key, datatype data)
   {
      if (temp_root == NULL)
      {
         temp_root = my_new(avl_tree_node);
         internal_size++;
         temp_root->key             = key;
         temp_root->data            = data;
         temp_root->right_child     = NULL;
         temp_root->left_child      = NULL;
         temp_root->node_avl_status = BALANCED;
         return INCREASED;
      }
      else if (key < temp_root->key)
      {
         if (insert_tree(temp_root->left_child, key, data) == INCREASED)
         {
            return left_child_increased(temp_root);
         };
      }
      else if (key > temp_root->key)
      {
         if (insert_tree(temp_root->right_child, key, data) == INCREASED)
         {
            return right_child_increased(temp_root);
         }
      }
      else /* (key == temp_root->key) */
      {
         temp_root->data = data;
      };
      return UNCHANGED;
   };

   avl_tree_node * find(avl_tree_node * temp_root, keytype key)
   {
      if (temp_root == NULL)
      {
         return NULL;
      }
      else
      {
         if (key < temp_root->key)
         {
            return find(temp_root->left_child, key);
         }
         else if (key == temp_root->key)
         {
            return temp_root;
         }
         else /* (key > temp_root->key) */
         {
            return find(temp_root->right_child, key);
         };
      };
   };

   avl_tree_node * find_leftmost(avl_tree_node * temp_root)
   {
      if ((temp_root != NULL) && (temp_root->left_child != NULL))
      {
         avl_tree_node * temp_leftmost = temp_root->left_child;
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

   avl_tree_node * find_rightmost(avl_tree_node * temp_root)
   {
      if ((temp_root != NULL) && (temp_root->right_child != NULL))
      {
         avl_tree_node * temp_rightmost = temp_root->right_child;
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

   change_flag extract_root(avl_tree_node * & temp_root, keytype key)
   {
      if ((temp_root->left_child  == NULL) &&
          (temp_root->right_child == NULL))
      {
         my_free(temp_root);
         internal_size--;
         temp_root = NULL;
         return DECREASED;
      }
      else if (temp_root->node_avl_status == LEFT_TALLER)
      {
         avl_tree_node * condemned = find_rightmost(temp_root->left_child);
         swap(condemned->key,  temp_root->key);
         swap(condemned->data, temp_root->data);
         if (extract_tree(temp_root->left_child, key) == DECREASED)
         {
            return left_child_decreased(temp_root);
         };
      }
      else
      {
         avl_tree_node * condemned = find_leftmost(temp_root->right_child);
         swap(condemned->key,  temp_root->key);
         swap(condemned->data, temp_root->data);
         if (extract_tree(temp_root->right_child, key) == DECREASED)
         {
            return right_child_decreased(temp_root);
         };
      };
      return UNCHANGED;
   };

   change_flag extract_tree(avl_tree_node * & temp_root, keytype key)
   {
      if (temp_root != NULL)
      {
         if (key == temp_root->key)
         {
            return extract_root(temp_root, key);
         }
         else if (key < temp_root->key)
         {
            if (extract_tree(temp_root->left_child, key) == DECREASED)
            {
               return left_child_decreased(temp_root);
            };
         }
         else /* (key >temp_root->key) */
         {
            if (extract_tree(temp_root->right_child, key) == DECREASED)
            {
               return right_child_decreased(temp_root);
            };
         }
      };
      return UNCHANGED;
   };

   void kill_tree(avl_tree_node * & temp_root)
   {
      if (temp_root != NULL)
      {
         kill_tree(temp_root->left_child);
         kill_tree(temp_root->right_child);
         my_free(temp_root);
         internal_size--;
         temp_root = NULL;
      };
   };

   void apply_to_all_in_tree(avl_tree_node * & temp_root, simple_func func)
   {
      if (temp_root != NULL)
      {
         apply_to_all_in_tree(temp_root->left_child, func);
         (*func)(temp_root->key, temp_root->data);
         apply_to_all_in_tree(temp_root->right_child, func);
      };
   };

   void print_all_in_tree(FILE * output_stream,
                          avl_tree_node * temp_root,
                          print_func func)
   {
      if (temp_root != NULL)
      {
         print_all_in_tree(output_stream, temp_root->left_child, func);
         (*func)(output_stream, temp_root->key, temp_root->data);
         print_all_in_tree(output_stream, temp_root->right_child, func);
      };
   };

public:

   general_avl_tree()
   {
      reset();
   };

   UInt32 count()
   {
      return internal_size;
   };

   void reset()
   {
      root = NULL;
      internal_size = 0;
   };

   void clean()
   {
      kill();
      reset();
   };

   void insert_entry(keytype key, datatype data)
   {
      insert_tree(root, key, data);
   };

   datatype * locate_address_of_data(keytype key)
   {
      avl_tree_node * temp = find(root, key);
      if (temp == NULL)
      {
         return NULL;
      }
      else
      {
         return (& (temp->data));
      };
   };

   keytype * locate_address_of_key(keytype key)
   {
      avl_tree_node * temp = find(root, key);
      if (temp == NULL)
      {
         return NULL;
      }
      else
      {
         return (& (temp->key));
      };
   };
   void extract_entry(keytype key)
   {
      extract_tree(root, key);
   };

   void apply_to_all(simple_func func)
   {
      apply_to_all_in_tree(root, func);
   };

   void print_all(FILE * output_stream, print_func func)
   {
      print_all_in_tree(output_stream, root, func);
   };

   void kill()
   {
      kill_tree(root);
   };
};

/*
   Now, if we assume the data is an integer, long integer, etc. (the
   usual usage), we need to include member functions to handle certain
   aspects of this condition efficently.  Rather than deriving a new
   class in the usual way, we've really just replicated the code.

   This was done deliberately.  There are possible compiler
   inefficiencies with inheritance and templates that we're trying to
   avoid.
*/
template <class keytype, class datatype>
class avl_tree
{
public:
   class avl_tree_node
   {
   public:
      keytype          key;
      datatype         data;
      avl_tree_node *  right_child;
      avl_tree_node *  left_child;
      avl_status       node_avl_status;
   };
   avl_tree_node * root;
   unsigned long int internal_size;
   typedef void (*simple_func)(keytype &, datatype &);
   typedef void (*print_func)(FILE *, keytype, datatype);

   unsigned long int size()
   {
      return internal_size;
   };

protected:
   void rotate_right(avl_tree_node * & temp_root)
   {
      avl_tree_node * save_root = temp_root;
      temp_root              = temp_root->left_child;
      save_root->left_child   = temp_root->right_child;
      temp_root->right_child  = save_root;
   };
   void rotate_left(avl_tree_node * & temp_root)
   {
      avl_tree_node * save_root = temp_root;
      temp_root              = temp_root->right_child;
      save_root->right_child  = temp_root->left_child;
      temp_root->left_child   = save_root;
   };
   /* the following two routines assume a valid avl tree and
      return a valid avl tree of the same height without "heaviness"
      on a particular side. */
   void avoid_left_heavy(avl_tree_node * & temp_root)
   {
      if (temp_root->node_avl_status == LEFT_TALLER)
      {
         avoid_right_heavy(temp_root->left_child);
         rotate_right(temp_root);
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
   void avoid_right_heavy(avl_tree_node * & temp_root)
   {
      if (temp_root->node_avl_status == RIGHT_TALLER)
      {
         avoid_left_heavy(temp_root->right_child);
         rotate_left(temp_root);
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

   /* these routines return a flag if the tree has grown in height */
   change_flag left_child_increased(avl_tree_node * & temp_root)
   {
      if (temp_root->node_avl_status == LEFT_TALLER)
      {
         avoid_right_heavy(temp_root->left_child);
         rotate_right(temp_root);
         if (temp_root->node_avl_status == BALANCED)
         {
            temp_root->node_avl_status               = RIGHT_TALLER;
            (temp_root->right_child)->node_avl_status = LEFT_TALLER;
            return INCREASED;
         }
         else /* (temp_root == LEFT_TALLER) */
         {
            temp_root->node_avl_status               = BALANCED;
            (temp_root)->right_child->node_avl_status = BALANCED;
            return UNCHANGED;
         };
      }
      else if (temp_root->node_avl_status == BALANCED)
      {
         temp_root->node_avl_status = LEFT_TALLER;
         return INCREASED;
      }
      else /* (temp_root->node_avl_status == RIGHT_TALLER) */
      {
         temp_root->node_avl_status = BALANCED;
         return UNCHANGED;
      };
   };

   change_flag right_child_increased(avl_tree_node * & temp_root)
   {
      if (temp_root->node_avl_status == RIGHT_TALLER)
      {
         avoid_left_heavy(temp_root->right_child);
         rotate_left(temp_root);
         if (temp_root->node_avl_status == BALANCED)
         {
            temp_root->node_avl_status              = LEFT_TALLER;
            (temp_root->left_child)->node_avl_status = RIGHT_TALLER;
            return INCREASED;
         }
         else /* (temp_root == RIGHT_TALLER) */
         {
            temp_root->node_avl_status              = BALANCED;
            (temp_root->left_child)->node_avl_status = BALANCED;
            return UNCHANGED;
         };
      }
      else if (temp_root->node_avl_status == BALANCED)
      {
         temp_root->node_avl_status = RIGHT_TALLER;
         return INCREASED;
      }
      else /* (temp_root->node_avl_status == LEFT_TALLER) */
      {
         temp_root->node_avl_status = BALANCED;
         return UNCHANGED;
      };
   };

   change_flag left_child_decreased(avl_tree_node * & temp_root)
   {
      if (temp_root->node_avl_status == RIGHT_TALLER)
      {
         avoid_left_heavy(temp_root->right_child);
         rotate_left(temp_root);
         if (temp_root->node_avl_status == BALANCED)
         {
            temp_root->node_avl_status              = LEFT_TALLER;
            (temp_root->left_child)->node_avl_status = RIGHT_TALLER;
            return UNCHANGED;
         }
         else /* (temp_root == RIGHT_TALLER) */
         {
            temp_root->node_avl_status              = BALANCED;
            (temp_root->left_child)->node_avl_status = BALANCED;
            return DECREASED;
         };
      }
      else if (temp_root->node_avl_status == BALANCED)
      {
         temp_root->node_avl_status = RIGHT_TALLER;
         return UNCHANGED;
      }
      else /* (temp_root->node_avl_status == LEFT_TALLER) */
      {
         temp_root->node_avl_status = BALANCED;
         return DECREASED;
      };
   };

   change_flag right_child_decreased(avl_tree_node * & temp_root)
   {
      if (temp_root->node_avl_status == LEFT_TALLER)
      {
         avoid_right_heavy(temp_root->left_child);
         rotate_right(temp_root);
         if (temp_root->node_avl_status == BALANCED)
         {
            temp_root->node_avl_status               = RIGHT_TALLER;
            (temp_root->right_child)->node_avl_status = LEFT_TALLER;
            return UNCHANGED;
         }
         else /* (temp_root == LEFT_TALLER) */
         {
            temp_root->node_avl_status               = BALANCED;
            (temp_root->right_child)->node_avl_status = BALANCED;
            return DECREASED;
         };
      }
      else if (temp_root->node_avl_status == BALANCED)
      {
         temp_root->node_avl_status = LEFT_TALLER;
         return UNCHANGED;
      }
      else /* (temp_root->node_avl_status == RIGHT_TALLER) */
      {
         temp_root->node_avl_status = BALANCED;
         return DECREASED;
      };
   };

   change_flag insert_tree(avl_tree_node * & temp_root, keytype key, datatype data)
   {
      if (temp_root == NULL)
      {
         temp_root = my_new(avl_tree_node);
         internal_size++;
         temp_root->key             = key;
         temp_root->data            = data;
         temp_root->right_child     = NULL;
         temp_root->left_child      = NULL;
         temp_root->node_avl_status = BALANCED;
         return INCREASED;
      }
      else if (key < temp_root->key)
      {
         if (insert_tree(temp_root->left_child, key, data) == INCREASED)
         {
            return left_child_increased(temp_root);
         };
      }
      else if (key > temp_root->key)
      {
         if (insert_tree(temp_root->right_child, key, data) == INCREASED)
         {
            return right_child_increased(temp_root);
         }
      }
      else /* (key == temp_root->key) */
      {
         temp_root->data = data;
      };
      return UNCHANGED;
   };

   avl_tree_node * find(avl_tree_node * temp_root, keytype key)
   {
      if (temp_root == NULL)
      {
         return NULL;
      }
      else
      {
         if (key < temp_root->key)
         {
            return find(temp_root->left_child, key);
         }
         else if (key == temp_root->key)
         {
            return temp_root;
         }
         else /* (key > temp_root->key) */
         {
            return find(temp_root->right_child, key);
         };
      };
   };

   avl_tree_node * find_leftmost(avl_tree_node * temp_root)
   {
      if ((temp_root != NULL) && (temp_root->left_child != NULL))
      {
         avl_tree_node * temp_leftmost = temp_root->left_child;
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

   avl_tree_node * find_rightmost(avl_tree_node * temp_root)
   {
      if ((temp_root != NULL) && (temp_root->right_child != NULL))
      {
         avl_tree_node * temp_rightmost = temp_root->right_child;
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

   change_flag extract_root(avl_tree_node * & temp_root, keytype key)
   {
      if ((temp_root->left_child  == NULL) &&
          (temp_root->right_child == NULL))
      {
         my_free(temp_root);
         internal_size--;
         temp_root = NULL;
         return DECREASED;
      }
      else if (temp_root->node_avl_status == LEFT_TALLER)
      {
         avl_tree_node * condemned = find_rightmost(temp_root->left_child);
         swap(condemned->key,  temp_root->key);
         swap(condemned->data, temp_root->data);
         if (extract_tree(temp_root->left_child, key) == DECREASED)
         {
            return left_child_decreased(temp_root);
         };
      }
      else
      {
         avl_tree_node * condemned = find_leftmost(temp_root->right_child);
         swap(condemned->key,  temp_root->key);
         swap(condemned->data, temp_root->data);
         if (extract_tree(temp_root->right_child, key) == DECREASED)
         {
            return right_child_decreased(temp_root);
         };
      };
      return UNCHANGED;
   };

   change_flag extract_tree(avl_tree_node * & temp_root, keytype key)
   {
      if (temp_root != NULL)
      {
         if (key == temp_root->key)
         {
            return extract_root(temp_root, key);
         }
         else if (key < temp_root->key)
         {
            if (extract_tree(temp_root->left_child, key) == DECREASED)
            {
               return left_child_decreased(temp_root);
            };
         }
         else /* (key >temp_root->key) */
         {
            if (extract_tree(temp_root->right_child, key) == DECREASED)
            {
               return right_child_decreased(temp_root);
            };
         }
      };
      return UNCHANGED;
   };

   void kill_tree(avl_tree_node * & temp_root)
   {
      if (temp_root != NULL)
      {
         kill_tree(temp_root->left_child);
         kill_tree(temp_root->right_child);
         my_free(temp_root);
         internal_size--;
         temp_root = NULL;
      };
   };

   void apply_to_all_in_tree(avl_tree_node * & temp_root,
                             simple_func func)
   {
      if (temp_root != NULL)
      {
         apply_to_all_in_tree(temp_root->left_child, func);
         (*func)(temp_root->key, temp_root->data);
         apply_to_all_in_tree(temp_root->right_child, func);
      };
   };

   void print_all_in_tree(FILE * output_stream,
                          avl_tree_node * temp_root,
                          print_func func)
   {
      if (temp_root != NULL)
      {
         print_all_in_tree(output_stream, temp_root->left_child, func);
         (*func)(output_stream, temp_root->key, temp_root->data);
         print_all_in_tree(output_stream, temp_root->right_child, func);
      };
   };

/*
   New member functions for this class (compared to the general_avl_tree
   class).
*/

   change_flag increment_data(avl_tree_node * & temp_root,
                              keytype key, datatype amount)
   {
      if (temp_root == NULL)
      {
         temp_root = my_new(avl_tree_node);
         internal_size++;
         temp_root->key             = key;
         temp_root->data            = amount;
         temp_root->right_child     = NULL;
         temp_root->left_child      = NULL;
         temp_root->node_avl_status = BALANCED;
         return INCREASED;
      }
      else if (key < temp_root->key)
      {
         if (increment_data(temp_root->left_child, key, amount) == INCREASED)
         {
            return left_child_increased(temp_root);
         };
      }
      else if (key > temp_root->key)
      {
         if (increment_data(temp_root->right_child, key, amount) == INCREASED)
         {
            return right_child_increased(temp_root);
         }
      }
      else /* (key == temp_root->key) */
      {
         temp_root->data = temp_root->data + amount;
      };
      return UNCHANGED;
   };

   datatype sum_of_entries(avl_tree_node * temp_root)
   {
      if (temp_root != NULL)
      {
         return temp_root->data + sum_of_entries(temp_root->left_child)
                                + sum_of_entries(temp_root->right_child);
      }
      else
      {
         return (datatype)0;
      };
   };

   void find_data_extremes(avl_tree_node * temp_root,
                           datatype &  min_data,
                           datatype &  max_data)
   {
      if (temp_root != NULL)
      {
         if (min_data > temp_root->data) min_data = temp_root->data;
         if (max_data < temp_root->data) max_data = temp_root->data;
         find_data_extremes(temp_root->left_child, min_data, max_data);
         find_data_extremes(temp_root->right_child, min_data, max_data);
      };
   };

/*
   End of new member functions.
*/


public:

   avl_tree()
   {
      reset();
   };

   UInt32 count()
   {
      return internal_size;
   };

   void reset()
   {
      root = NULL;
      internal_size = 0;
   };

   void clean()
   {
      kill();
      reset();
   };

   void insert_entry(keytype key, datatype data)
   {
      insert_tree(root, key, data);
   };

   datatype * locate_address_of_data(keytype key)
   {
      avl_tree_node * temp = find(root, key);
      if (temp == NULL)
      {
         return NULL;
      }
      else
      {
         return (& (temp->data));
      };
   };

   void apply_to_all(simple_func func)
   {
      apply_to_all_in_tree(root, func);
   };

   void print_all(FILE * output_stream, print_func func)
   {
      print_all_in_tree(output_stream, root, func);
   };

   void print_all_in_tree(int depth,
                          FILE * output_stream,
                          avl_tree_node * temp_root,
                          print_func func)
   {
      if (temp_root != NULL)
      {
         print_all_in_tree(depth + 1, output_stream, temp_root->left_child, func);
         indent(output_stream, depth);
         fprintf(output_stream, "(%d) ", depth);
         (*func)(output_stream, temp_root->key, temp_root->data);
         print_all_in_tree(depth + 1, output_stream, temp_root->right_child, func);
      };
   };

   void diagnostic_print_all(FILE * output_stream, print_func func)
   {
      fprintf(output_stream, "@@@@@@@@@@@@@\n");
      print_all_in_tree(0, output_stream, root, func);
      fprintf(output_stream, "@@@@@@@@@@@@@\n");
   };

   void extract_entry(keytype key)
   {
      extract_tree(root, key);
   };

   void kill()
   {
      kill_tree(root);
   };

/*
   New member functions for this class (compared to the general_avl_tree
   class).
*/

   void increment(keytype key)
   {
      increment_data(root, key, (datatype)1);
   };

   void increment(keytype key, datatype amount)
   {
      increment_data(root, key, amount);
   };

   datatype sum_of_entries()
   {
      return sum_of_entries(root);
   };

   attempt_flag find_key_extremes(keytype &  min_key,
                                  keytype &  max_key)
   {
      if (root != NULL)
      {
         avl_tree_node * temp_root = root;
         while (temp_root->left_child != NULL)
         {
            temp_root = temp_root->left_child;
         };
         min_key = temp_root->key;
         temp_root = root;
         while (temp_root->right_child != NULL)
         {
            temp_root = temp_root->right_child;
         };
         max_key = temp_root->key;
         return SUCCESS;
      }
      else
      {
         return FAILURE;
      };
   };

   attempt_flag find_data_extremes(datatype &  min_data,
                                   datatype &  max_data)
   {
      if (root == NULL)
      {
         return FAILURE;
      }
      else
      {
         min_data = root->data;
         max_data = root->data;
         find_data_extremes(root->left_child, min_data, max_data);
         find_data_extremes(root->right_child, min_data, max_data);
         return SUCCESS;
      };
   };
/*
   End of new member functions.
*/

};


#endif
