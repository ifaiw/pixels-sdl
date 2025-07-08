#include <stdio.h>
#include "text_stuff.h"

struct Point{
    int x;
    int y;
};

struct Circles{
    struct Point* points;
};

// struct Circle{
//     struct Point*
#pragma pack(1)
struct Header {
	const char s[10];
	int a;
	int b;
};



int main(int num_args, char* arg[]) {
	const char* t = "one {2} {three} four";

    char* t2 = replace_once(t, "two");
    printf("t2: %s\n", t2);
    printf("t2: %s\n", t2+4);
    char* t3 = replace_once(t2, "3");

    printf("%s\n", t3);

    return 0;	
	
	struct Header h = {
		.s = "blahblah\n",
		.a = 1,
		.b = 2
	
	};
	printf("The size of h is %ld\n", sizeof(struct Header));


	
}

