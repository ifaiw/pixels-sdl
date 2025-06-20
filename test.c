#include <stdio.h>

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
	const char s[9];
	int a;
	int b;
};



int main(int num_args, char* arg[]) {
	struct Header h = {
		.s = "blahblah\n",
		.a = 1,
		.b = 2
	
	};
	printf("The size of h is %ld\n", sizeof(struct Header));


	
}

