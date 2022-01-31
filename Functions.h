//
// Created by Emad on 31/01/2022.
//

#ifndef TEST_GRAPHICS_FUNCTIONS_H
#define TEST_GRAPHICS_FUNCTIONS_H

// generates random between mn and mx
int RAND(int mn, int mx){
    return mn+(rand()%(mx-mn));
}

// returns the distance between 2 points (^2)
int DISTANCE(int x1,int y1,int x2,int y2){
    return (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2);
}

// checks if two boxes collide
int COLLIDE(int x1,int y1,int w1,int h1,int x2,int y2,int w2,int h2){
    if(x1>x2+w2 || x1+w1<x2) return 0;
    if(y1>y2+h2 || y1+h1<y2) return 0;
    return 1;
}

#endif //TEST_GRAPHICS_FUNCTIONS_H
