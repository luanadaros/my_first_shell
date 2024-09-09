#include <stdio.h>

int main(){
    int counter = 0;

    for(int i = 0; i < 1000000000; i++){
        counter++;
    }

    printf("Counter result: %d\n", counter);

    return 0;
}