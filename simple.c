#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include "avl.h"

#define NAME_LEN 36
#define PHONE_LEN 10

// assuming dtype has a link member as node type (avl_node_t)
#define OFFSETOF(data) (sizeof((data)->name) + sizeof((data)->phone) )

typedef struct person {
	char name[ NAME_LEN + 1];
	char phone[ PHONE_LEN + 1];
	avl_node_t  my_link;
} Person;

int comparePerson(const void *a, const void *b) 
{
	Person *_a = (Person *)a;
	Person *_b = (Person *)b;
	if ( strncmp(_a->name, _b->name, NAME_LEN) < 0 )
		return -1;
	else if ( strncmp(_a->name, _b->name, NAME_LEN) > 0 )
		return 1;
	return 0;
}

Person* searchByName(avl_tree_t *tree, const char *name)
{
	Person *cur = AVL_NODE2DATA(tree->avl_root, tree->avl_offset);
	avl_node_t *p;
	while (cur) {
		if (strcmp(cur->name, name) == 0)
			return cur;
		else if (strcmp(name, cur->name) <0) {
			p = AVL_DATA2NODE( cur, tree->avl_offset );
			p = p->avl_child[0];
		}
		else {
			p = AVL_DATA2NODE( cur, tree->avl_offset );
			p = p->avl_child[1];
		}
		cur = p ? AVL_NODE2DATA( p, tree->avl_offset) : NULL;
	}
	return NULL;
}

void printTree(avl_tree_t *tree)
{
	int i = 0;
	Person *cur;

	printf("Total nodes in tree: %ld\n", avl_numnodes(tree));
	printf("Root: %s\n", ((Person *)(AVL_NODE2DATA( tree->avl_root, tree->avl_offset)))->name);

	cur = avl_first(tree);
	while (cur) {
		printf("%d : %s(%s)\n", i, cur->name, cur->phone);
		cur = AVL_NEXT(tree, cur);
		i++;
	}
	printf("---------------------------------------------------------------------\n");
}

int main(int argc, char **argv)
{
	avl_tree_t avl;

	Person a;
	memset(&a, '\0', sizeof a);
	strncpy(a.name, "Micheal Smith", 14);
	strncpy(a.phone, "1112223333", 11);
	avl_create(&avl, comparePerson, sizeof(Person), OFFSETOF(&a));
	avl_add(&avl, &a);

	Person b;
	memset(&b, '\0', sizeof b);
	strncpy(b.name, "Jack Stuart", 12);
	strncpy(b.phone, "2223334444", 11);
	avl_add(&avl, &b);

	//traverse 
	printTree(&avl);

	/**
	 *          Mike
	 *          /  
	 *        Jack
	 */

	// If I add Andrew, it's unbalanced.
	Person c;
	memset(&c, '\0', sizeof c);
	strcpy(c.name, "Andrew Kudos");
	strcpy(c.phone, "3334445555");
	avl_add(&avl, &c);

	//traverse!!  root must be Jack.
	printTree(&avl);

	// Searching.
	Person *search;
	search = searchByName(&avl, "Jack Stuart");
	if (search) 
		printf("Jack found: %s\n", search->name);
	else
		printf("Jack not found\n");

	search = searchByName(&avl, "lalalalala");
	if (search)
		printf("lala found:\n");
	else
		printf("lala not found\n");

	return 0;
}
