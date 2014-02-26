/*
 * RandomGraph.h
 *
 *  Created on: Oct 14, 2013
 *      Author: sagar
 */

#ifndef RANDOMGRAPH_H_
#define RANDOMGRAPH_H_

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <map>
#include <set>
#include <stdint.h>
#include "Global.h"

bool generateRandomGraph(sVertex* vertices, const uint numOfNodes, const uint density);
uint dfs(sVertex* vertices, uint vertIndex);

#endif /* RANDOMGRAPH_H_ */
