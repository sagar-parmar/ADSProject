/*
 * global.h
 *
 *  Created on: Oct 14, 2013
 *      Author: sagar
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#define MAX_NODES 10000
#define MAX_COST 10000
//Uncomment to start in debug mode
//#define LOG_ON 1

#include <sys/time.h>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <iomanip>

using namespace std;

// Structure to represent an Edge in a graph
struct sEdge {
	int vertexStart;
	int vertexEnd;
	int cost;
	bool visited;

	sEdge():
		vertexStart(0),vertexEnd(0),cost(0),visited(false) {}

	inline bool operator>(const sEdge& e) {
		if(e.cost > cost)
			return true;
		return false;
	}
	inline bool operator<(const sEdge& e) {
		if(e.cost < cost)
			return true;
		return false;
	}
	inline bool operator==(const sEdge& e) {
		if(e.cost == cost)
			return true;
		return false;
	}
	friend ostream& operator<<(ostream& out, const sEdge& e){
		return(cout << "[" << e.vertexStart << ", " << e.vertexEnd << ", "<< e.cost <<"]");
	}
};

// Structure to represent a vertex(node) in a graph
struct sVertex {
	int id;
	bool visited;
    list<sEdge> adj;   // adjacency list of (edge,cost)

    sVertex():
    	id(0),visited(false),adj(0) {}

    friend ostream& operator<< (ostream& out, const sVertex& n) {
		out << "[VertexId=" << n.id << "]";
		return out;
    }
};

#endif /* GLOBAL_H_ */
