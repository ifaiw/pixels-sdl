#include <math.h>
#include <stdio.h>

int main(int argc, char* argv[]){
    for(double d = 0; d < 5; d+=0.1) {
        printf("d=%f round=%f int(round)=%d\n", d, round(d), (int)round(d));
    }
}

