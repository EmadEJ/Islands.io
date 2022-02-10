//
// Created by Emad on 31/01/2022.
//
#pragma once

#ifndef TEST_GRAPHICS_FUNCTIONS_H
#define TEST_GRAPHICS_FUNCTIONS_H

#include <math.h>

// generates random between mn and mx
int RAND(int mn, int mx);

// returns the distance between 2 points (^2)
int DISTANCE(int x1,int y1,int x2,int y2);
// checks if two boxes collide
int COLLIDE(int x1,int y1,int w1,int h1,int x2,int y2,int w2,int h2);

char *TO_STRING(int x);

// my own minimum and maximum because C sucks and doesn't have one
int MIN(int a,int b);

int MAX(int a,int b);

#endif //TEST_GRAPHICS_FUNCTIONS_H
