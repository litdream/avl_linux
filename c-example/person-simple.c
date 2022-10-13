// compile:   gcc -I.. -Wall -g person-simple.c ../avl.o

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <avl.h>
#include <stddef.h>

#define NAMELEN 50

typedef struct person {
	char name[NAMELEN + 1];
	int age;
	char city[NAMELEN + 1];

	avl_node_t  my_link;
} Person;

// No error checking.  Send data in good length.
void setPerson(Person* p, const char *name, int age, const char *city)
{
	strncpy(p->name, name, NAMELEN);
	p->name[NAMELEN] = '\0';
	p->age = age;
	strncpy(p->city, city, NAMELEN);
	p->city[NAMELEN] = '\0';
}

int comparePerson(const void *a, const void *b)
{
	Person *_a = (Person *)a;
	Person *_b = (Person *)b;
	if ( strncmp(_a->name, _b->name, NAMELEN) < 0 )
		return -1;
	else if ( strncmp(_a->name, _b->name, NAMELEN) > 0 )
		return 1;
	return 0;
}

static char* Person2str(Person *p)
{
	static char buf[1000];
	static char elm[100];
	
	memset(buf, '\0', 1000);
	memset(elm, '\0', 100);
	
	strcat(buf, p->name);
	strcat(buf, " | ");
	sprintf(elm, "%d | ", p->age);
	strcat(buf, elm);
	strcat(buf, p->city);

	return buf;
	       
}

int main(int argc, char **argv)
{
	Person a,b,c,d;
	setPerson(&a, "Raymond", 49, "Busan");
	setPerson(&b, "Jake", 17, "Annandale");
	setPerson(&c, "Jaron", 15, "Annandale");
	setPerson(&d, "Justin", 12, "Johns Creek");

	avl_tree_t avl;
	avl_create(&avl, comparePerson, sizeof(Person), offsetof(Person, my_link));

	avl_add(&avl, &a);
	avl_add(&avl, &b);
	avl_add(&avl, &c);
	avl_add(&avl, &d);

	Person *first = avl_first(&avl);
	Person *last = avl_last(&avl);
	
	printf("%s\n", Person2str( first ) );
	printf("%s\n", Person2str( AVL_NEXT(&avl, first)) );
	printf("%s\n", Person2str( AVL_NEXT(&avl, AVL_NEXT(&avl, first)))   );
	printf("%s\n", Person2str( last ) );
	
	return 0;
}
