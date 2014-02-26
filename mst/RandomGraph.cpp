/*
 * RandomGraph.cpp
 *
 *  Created on: Oct 14, 2013
 *  Author: Sagar
 */
#include "RandomGraph.h"

using namespace std;

// Number of nodes visited by DFS (should be equal to numOfNodes)
static uint numOfNodesVisitedByDfs = 0;
// boolean array returning if v1->v2 are connected
static bool	bGenVert[MAX_NODES][MAX_NODES] = {};
/*
 * Generates a random graph and checks for connectivity using DFS
 */
bool generateRandomGraph(sVertex* vertices,const uint numOfNodes, const uint density) {
	uint v1=0,v2=0,cost=0,numOfEdges = 0,numOfEdgeToGen=0;
	float numOfEdgeToGenFloat = 0;
	sEdge e1;

	// Calculate number of edges to generate as per density
	numOfEdgeToGenFloat = ((numOfNodes * (numOfNodes-1)/2) * density);
	numOfEdgeToGenFloat = numOfEdgeToGenFloat/100;
	numOfEdgeToGenFloat = ceil(numOfEdgeToGenFloat);

	numOfEdgeToGen = (int) numOfEdgeToGenFloat;

	if(numOfEdgeToGen < numOfNodes-1) {
		cout << "  Error: can't create the graph with less than n-1 edges" << endl;
		exit(EXIT_FAILURE);
	}
	cout << "--> Generating graph with " << numOfNodes << " vertices and " << numOfEdgeToGen << " edges ..." << endl;
	do {
		/* initialize random seed */
		srand (time(NULL));
		numOfNodesVisitedByDfs = 0;
		numOfEdges = 0;
		do {
			do {
				v1 = rand() % numOfNodes;
				v2 = rand() % numOfNodes;
			} while(v1 == v2);

			if(!bGenVert[v1][v2] &&  !bGenVert[v2][v1]) {

				bGenVert[v1][v2] = true;
				bGenVert[v2][v1] = true;

				cost = rand() % MAX_COST + 1;

				e1.vertexStart = v1;
				e1.vertexEnd = v2;
				e1.cost = cost;
				e1.visited = false;
				vertices[v1].id = v1;
				vertices[v1].visited = false;
				vertices[v1].adj.push_back(e1);

				e1.vertexStart = v2;
				e1.vertexEnd = v1;
				e1.cost = cost;
				e1.visited = false;
				vertices[v2].id = v2;
				vertices[v2].visited = false;
				vertices[v2].adj.push_back(e1);

				numOfEdges++;
			}
		} while (numOfEdges != numOfEdgeToGen);
		// check connectivity using DFS. Start from 0 and all the nodes should be visited
		cout << "--> Checking connectivity ..." << endl;

		numOfNodesVisitedByDfs = dfs(vertices,0);
#ifdef LOG_ON
	cout << "numOfNodesVisitedByDfs =" << numOfNodesVisitedByDfs << endl;
#endif
	} while(numOfNodesVisitedByDfs != numOfNodes);

	cout << "--> Graph generated ..." << endl;
	return EXIT_SUCCESS;
}

/*
 * Recursive DFS to check the graph connectivity
 */
uint dfs(sVertex* vertices, uint vertIndex) {
	if(vertices[vertIndex].visited)
		return 0;
	vertices[vertIndex].visited = true;
	numOfNodesVisitedByDfs++;
#ifdef LOG_ON
	cout << "visiting nodeID =" << vertIndex << endl;
#endif
	list<sEdge>::iterator it = vertices[vertIndex].adj.begin();
	if (it == vertices[vertIndex].adj.end())
		return 0;
	for (; it != vertices[vertIndex].adj.end(); it++) {
		dfs(vertices,it->vertexEnd);
	}
	return numOfNodesVisitedByDfs;
}
