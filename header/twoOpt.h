//==================================================================
// File			: twoOpt.h
// Description		: Perform optimizations on graph
//==================================================================
#ifndef MYGRAPH_H
#define MYGRAPH_H

#include <vector>
#include <stack>
#include <iostream>

using namespace std;

// Non-looping version of two-opt optimization heuristic
int twoOpt(int **graph, vector<int> &path, int &pathLength, int n);

// 2-Opt helper function: swap two nodes
bool is_path_shorter(int **graph, int v1, int v2, int v3, int v4, int &total_distance);

void reverse(vector<int> &path, int start, int end, int n);

#endif
