/*
 * Mst.h
 *
 *  Created on: Oct 14, 2013
 *      Author: sagar
 */

#ifndef MST_H_
#define MST_H_

#include <algorithm>
#include <iostream>
#include <vector>
#include <cstring>
#include <sstream>
#include <fstream>
#include <string>
#include <list>
#include "Global.h"
#include "RandomGraph.h"
#include "FibonacciHeap.hpp"

static sVertex vertices[MAX_NODES];
static bool bUserInputMode = false,bSimpleScheme = false;
static int numOfNodes = 0, density = 0, numOfEdges = 0;

void printHelp();
void printGraph(sVertex* vertices, const uint numOfNodes);
void resetVisited(sVertex* vertices, const uint numOfNodes);
bool populateDataFromFile(const string* fileName,sVertex* vertices);
bool generateMSTSimpleScheme(sVertex* vertices, const uint numOfNodes);
bool generateMSTFibonacciScheme(sVertex* vertices, const uint numOfNodes);

#endif /* MST_H_ */
