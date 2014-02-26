//============================================================================
// Name        : Mst.cpp
// Author      : Sagar Parmar
// Version     : 1.0
// Description : ADS Project
//============================================================================

#include "Mst.h"
#include "RandomGraph.h"

/*
 * Main function - process arguments
 */
int main(int argc, char **argv) {
	//vector - range constructor
	vector<string> args(argv + 1, argv + argc);
	string strFileName;
	stringstream ss;

	if (argc == 1){
		printHelp();
		return EXIT_SUCCESS;
	}

	// Loop over command-line args
	for (vector<string>::iterator i = args.begin(); i != args.end(); ++i) {
		if (*i == "-h" || *i == "--help") {
			printHelp();
			return EXIT_SUCCESS;
		}
		else if (*i == "-s") {
			strFileName = *++i;
			bUserInputMode = true;
			bSimpleScheme = true;
		}
		else if (*i == "-f") {
			strFileName = *++i;
			bUserInputMode = true;
		}
		else if (*i == "-r") {
			ss.str(*++i);
			ss >> numOfNodes;
			ss.clear();
			ss.str(*++i);
			ss >> density;
			if(density > 100)
				density = 100;
			ss.clear();
		}
	}
	ss.flush();

	//Start processing as per the arguments
	if(bUserInputMode) {
		// User input mode
		// Populates data from file and generates a graph into vertices
		if(!populateDataFromFile(&strFileName,vertices)) {
#ifdef LOG_ON
				printGraph(vertices,numOfNodes);
#endif
			if (bSimpleScheme) {
				// If -s option was given use simple scheme
				return generateMSTSimpleScheme(vertices,numOfNodes);
			}
			else {
				// If -f option was given use f-heap scheme
				return generateMSTFibonacciScheme(vertices,numOfNodes);
			}
		}
	}
	else {
		// Random mode
		// Generates a random graph and checks for connectivity using DFS
		if(!generateRandomGraph(vertices,numOfNodes,density)) {

#ifdef LOG_ON
			printGraph(vertices,numOfNodes);
#endif
			/*
			 * In random mode, first generate MST using simple scheme and then generate
			 * using f-heap scheme. Both the functions will print time taken during
			 * the execution
			 */
			cout << "==============================" << endl;
			cout << "Simple Scheme:" << endl;
			if(!generateMSTSimpleScheme(vertices,numOfNodes)) {
				cout << "Fibonacci Scheme:" <<endl;
				resetVisited(vertices,numOfNodes);
				return generateMSTFibonacciScheme(vertices,numOfNodes);
			}
		}
	}
	return EXIT_FAILURE;
}

/*
 * Prints help. Gets called when user either selects -h or invalid option
 */
void printHelp() {
	cout << "mst -s file-name" << endl;
	cout << "mst -f file-name" << endl;
	cout << "mst -r n d \t n = number of nodes, d = density" << endl;
}

/*
 * Print graph function for debugging purpose
 */
void printGraph(sVertex* vertices, const uint numOfNodes) {
	for(uint i=0;i<numOfNodes;i++) {
		for (list<sEdge>::iterator it = vertices[i].adj.begin(); it != vertices[i].adj.end(); it++) {
			cout << "\t" << i << "->"<< it->vertexEnd << "  " << it->cost << endl;
		}
		cout << "-------------------------" << endl;
	}
}

/*
 * Reset all visited states to false
 */
void resetVisited(sVertex* vertices, const uint numOfNodes) {
	for(uint i=0;i<numOfNodes;i++) {
		vertices[i].visited = false;
		for (list<sEdge>::iterator it = vertices[i].adj.begin(); it != vertices[i].adj.end(); it++) {
			it->visited = false;
		}
	}
}

/*
 * Populates graph from the given file. It reads from the file line by line.
 */
bool populateDataFromFile(const string* fileName,sVertex* vertices) {
	ifstream file(fileName->c_str());
	string line,item;
	stringstream lineSS,itemSS;
	uint v1=0,v2=0;
	int cost=0;
	sEdge e1;

	if(file.good()) {
		getline(file,line);
		lineSS.str(line);

		// Get number of nodes
		getline(lineSS, item, ' ');
		itemSS.str(item);
		itemSS >> numOfNodes;
		itemSS.clear();

		// Get number of edges
		getline(lineSS, item, ' ');
		itemSS.str(item);
		itemSS >> numOfEdges;
		itemSS.clear();

		while (getline(file,line)) {
			// Process line
			lineSS.clear();
#ifdef LOG_ON
			cout << line << endl;
#endif
			// Get vertex start
			lineSS.str(line);
			getline(lineSS, item, ' ');
			itemSS.str(item);
			itemSS >> v1;
			itemSS.clear();

			// Get vertex end
			getline(lineSS, item, ' ');
			itemSS.str(item);
			itemSS >> v2;
			itemSS.clear();

			// Get vertex cost
			getline(lineSS, item, ' ');
			itemSS.str(item);
			itemSS >> cost;
			itemSS.clear();

#ifdef LOG_ON
			cout << "read:" << v1 << "," << v2 << "," << cost << endl;
#endif

			//Add this edge to graph
			e1.vertexStart = v1;
			e1.vertexEnd = v2;
			e1.cost = cost;
			e1.visited = false;
			vertices[v1].id = v1;
			vertices[v1].visited = false;
			vertices[v1].adj.push_back(e1);

			// Since graph is undirected, add an edge from v2 to v1 also
			e1.vertexStart = v2;
			e1.vertexEnd = v1;
			e1.cost = cost;
			e1.visited = false;
			vertices[v2].id = v2;
			vertices[v2].visited = false;
			vertices[v2].adj.push_back(e1);
		}
		itemSS.flush();
		lineSS.flush();
		file.close();
		return EXIT_SUCCESS;
	}
	else {
		cout << "Unable to open file \"" << fileName <<"\"" << endl;
		return EXIT_FAILURE;
	}
}

/* Algorithm :
		1.	Maintain an array on the vertices V (G).
		2. 	Put s in the queue, where s is the start vertex. Give s a key of 0.
			Add all other vertices and set their key to infinity.
		3. 	Repeatedly get the minimum-key vertex v from the array and mark it scanned.
			For each neighbor w of v do:
				If w is not scanned (so far), decrease its key to the min[cost(v,w) , w’s currentkey]
*/
bool generateMSTSimpleScheme(sVertex* vertices, const uint numOfNodes) {
	struct timeval start, end;
	long mtime, seconds, useconds;
	// the final MST will be stored in vMstOutput
	vector<sEdge> vMstOutput(numOfNodes);
	uint curMstIdx = 0;
	// Array to hold costs. Initially every vertex has infinity cost
	int curMstNodes[numOfNodes];
	// For recording parent from where the node can be accessed with the least cost
	uint vParentIds[numOfNodes];

	uint extractedVertexIdx = 0, totalCost = 0;
	int currentCost = MAX_COST, extractedCost = MAX_COST;


	// Adding root to the spanning tree initially and setting other node cost to infinity
	curMstNodes[0]= 0;
	vertices[0].visited = false;
	for(uint i=1; i < numOfNodes ;i++) {
		vertices[i].visited = false;
		curMstNodes[i] = MAX_COST;
	}

	// Start recording the time
	gettimeofday(&start, NULL);

	while(curMstIdx < numOfNodes) {

		extractedCost = MAX_COST;

		//Find the min element
		for(uint i=0; i < numOfNodes; i++) {
			if(curMstNodes[i] < extractedCost && !vertices[i].visited) {
				extractedCost = curMstNodes[i];
				extractedVertexIdx = i;
			}
		}
		vertices[extractedVertexIdx].visited = true;
#ifdef LOG_ON
		cout << "nextMin = " << extractedVertexIdx << " cost = " << extractedCost << endl;
#endif

		vMstOutput[curMstIdx].vertexStart = vParentIds[extractedVertexIdx];
		vMstOutput[curMstIdx].vertexEnd = extractedVertexIdx;
		vMstOutput[curMstIdx].cost = extractedCost;
		totalCost +=  extractedCost;

		// Iterate through all the edges of the vertex and set the cost if cost < currentCost
		list<sEdge>::iterator it = vertices[extractedVertexIdx].adj.begin();
		for (; it != vertices[extractedVertexIdx].adj.end(); it++) {
#ifdef LOG_ON
			cout << vertices[extractedVertexIdx] << "->" << *it << endl;
#endif
			if(!vertices[it->vertexEnd].visited) {
				currentCost = curMstNodes[it->vertexEnd];
				if(it->cost < currentCost) {
					curMstNodes[it->vertexEnd] = it->cost;
					vParentIds[it->vertexEnd] = extractedVertexIdx;
				}
			}
		}
#ifdef LOG_ON
		cout << endl;
#endif
		curMstIdx++;
	}
	gettimeofday(&end, NULL);
	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;
	mtime = ((seconds) * 1000000 + useconds) ;

	// Output the total cost
	cout << "TotalCost = " << totalCost << endl;
	// Output MST with n-1 items
	for (uint i=1; i < numOfNodes;i++)
		cout << setw(6) << left << vMstOutput[i].vertexStart
			 << setw(6) << left << vMstOutput[i].vertexEnd << endl;
	cout << "Time Taken = " << mtime << " microseconds"<< endl;
	cout << "==============================" << endl;

	return EXIT_SUCCESS;
}

/* Algorithm :
		1.	Maintain a priority queue on the vertices V (G).
		2. 	Put s in the queue, where s is the start vertex. Give s a key of 0.
			Add all other vertices and set their key to infinity.
		3. 	Repeatedly delete the minimum-key vertex v from the queue and mark it scanned.
			For each neighbor w of v do:
				If w is not scanned (so far), decrease its key to the min[cost(v,w) , w’s currentkey]
*/
bool generateMSTFibonacciScheme(sVertex* vertices, const uint numOfNodes) {
	// the final MST will be stored in vMstOutput
	vector<sEdge> vMstOutput(numOfNodes);
	uint totalCost = 0,curMstIdx = 0, extractedVertexIdx;
	// For recording parent from where the node can be accessed with the least cost
	uint vParentIds[numOfNodes];
 	int currentCost = 0;


	FHeap vertexHeap;
	//Saving pointer to fHeapNode (returned by insert) in fNodes vector for decreaseKey operation
	vector<FHeapNode*> fNodes(numOfNodes);

	// Setting root node's key to 0 and others to infinity
	fNodes[0] = vertexHeap.insert(0,0);
	for(uint i=1; i < numOfNodes ;i++) {
		vertices[i].visited = false;
		fNodes[i] = vertexHeap.insert(i,MAX_COST);
	}

	struct timeval start, end;
	long mtime, seconds, useconds;
	gettimeofday(&start, NULL);

	while(curMstIdx < numOfNodes) {

		//Find the min element by using removeMin
		FHeapNode temp = *vertexHeap.minimum();
		vertexHeap.removeMinimum();
		extractedVertexIdx = temp.data();
		vertices[extractedVertexIdx].visited  = true;

		//Store in MST for later use
		vMstOutput[curMstIdx].vertexStart = vParentIds[vertices[extractedVertexIdx].id];
		vMstOutput[curMstIdx].vertexEnd = extractedVertexIdx;
		vMstOutput[curMstIdx].cost = temp.key();
		totalCost +=  temp.key();

		// Iterate through all the edges of the vertex and do the decreaseKey if cost < currentCost
		list<sEdge>::iterator it = vertices[extractedVertexIdx].adj.begin();
		for(;it != vertices[extractedVertexIdx].adj.end();it++) {
			if(!vertices[it->vertexEnd].visited) {
				currentCost = fNodes[it->vertexEnd]->key();
				if(it->cost < currentCost) {
					vertexHeap.decreaseKey(fNodes[it->vertexEnd],it->cost);
					vParentIds[it->vertexEnd] = vertices[extractedVertexIdx].id;
				}
			}
		}
		curMstIdx++;
	}
	gettimeofday(&end, NULL);
	seconds  = end.tv_sec  - start.tv_sec;
	useconds = end.tv_usec - start.tv_usec;
	mtime = ((seconds) * 1000000 + useconds) ;

	// Total cost
	cout << "TotalCost = " << totalCost << endl;
	// Output MST with n-1 items
	for (uint i=1; i < numOfNodes;i++)
			cout << setw(6) << left << vMstOutput[i].vertexStart
				 << setw(6) << left << vMstOutput[i].vertexEnd << endl;
	cout << "Time Taken = " << mtime << " microseconds"<< endl;
	cout << "==============================" << endl;

	return EXIT_SUCCESS;
}

