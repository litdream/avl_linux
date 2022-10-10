/*
  My original avl from kernel has an issue.
  It is only usable for C, with already known size objects.

  If this is used in C++, there are 2 problems:
    1. It's hard to use generic type <template T>,  because size is not determined.
    2. To mitigate #1, we can attach calculation logics against T, for everything related to size.
       But, then, we have to use malloc().
       This makes AVL tree initialization static allocation malloc().
       Then, later when I have to allocate AVLNode, it has to be malloc()
    3. We can abstract this in AVLTree.
       So, the user simply provide (std::string id, C++ Object) as a pair.
       This will be interpreted as (const char *id, T *data) internally, wrapping this as an AVLNode.
         - unfortunately, id must be copied, and Only can be char[]

       (10/7/2022)  
       Still working on this idea, if that is feasible.
       But, I am seeing lots of challenge.
       If we abstract malloc/free from the user, that means we can't use smart-pointer.
       I have to handle malloc/free inside of AVLTree, correctly.  Not leaking memory.

       
 */

#ifndef __AVL_HPP
#define __AVL_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include "../avl.h"

#define MAX_ID_LEN 100
#define OFFSETOF(node) ( sizeof(node->id) + sizeof(void *) )

extern int errno;


// static:  Only necessary for AVLTree wrapper.  No need to export this type.
static struct AVLNode {
    char id[MAX_ID_LEN+1];
    T *data;
    avl_note_t  my_link;      // for AVL node relation.
};

static int compareAVLNode(const void *a, const void *b)
{
    AVLNode *_a = (AVLNode *)a;
    AVLNode *_b = (AVLNode *)b;
    if ( strncmp(_a->id, _b->id, NAME_LEN) < 0 )
        return -1;
    else if ( strncmp(_a->id, _b->id, NAME_LEN) > 0 )
        return 1;
    return 0;
}

template <typename T>
class AVLTree
{
    AVLTree() {
        struct AVLNode tmpnode;   // only used to calculate offset
        avl_create( &(this->avl), compareAVLNode, sizeof(struct AVLNode), OFFSETOF(&tmpnode));
    }

    bool insertNode(const char *id, T *data) {
        AVLNode *ins = malloc(sizeof(AVLNode));
        if (ins == NULL) {
            errno = ENOMEM;   // allocation fail
            return false;
        }
        
        size_t cpn = strncpy(ins->id, id, MAX_ID_LEN);
        ins->id[cpn] = '\0';
        if ( this->getNode(ins->id) == NULL ) {
            errno = ENOTUNIQ;  // id already exists.
            goto insert_fail;
        }
 
        avl_add(&(this->avl), ins);
        return true;
        
    insert_fail:
        free(ins);
        return false;
    }

    T* getNode(const char *id) {
        // do not allow duplicate!
        AVLNode *srch = avl_find( &(this->avl), id, NULL);
        if (srch)
            return srch->data;
        else
            return NULL;
    }

    T* deleteNode(const char *id) {
        if (this->getNode(id)) {
            AVLNode *srch = avl_find( &(this->avl), id, NULL);
            T *rtn = srch->data;
            free(srch);
            return rtn;
        }
        else {
            return NULL;
        }
        
    }

    void debugPrintTree() {
        using std::cerr, std::endl;
    }
    
private:
    avl_tree_t avl;
};

#endif  // __AVL_HPP
