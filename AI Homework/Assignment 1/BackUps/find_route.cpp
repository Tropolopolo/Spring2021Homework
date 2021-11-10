#include <string>
#include <vector>
#include <iostream>
#include <string.h>
#include <fstream>
#include <deque>
#include <utility>
#include <limits>

//defines the relationships between nodes
typedef struct Arcs{
	std::string Name;
	std::string Dest;
	int cost;
} Arcs;

//defines the node information
typedef struct Node {
	std::string Name;
	Node* parent;
	int depth;
	int pathCost;
	std::vector<std::string> states;
	std::string route;
	int estimate;
} Node;

//When a solution node is found this is generated for easy information output.
//if path cost is -1 then there is no solution.
typedef struct Solution {
	std::string dest;
	std::string route;
	int pathCost;
	int generatedNodes;
	int expandedNodes;
} Solution;

//Defines the problem that the algorithm needs to solve.
typedef struct Problem{
	std::vector<Arcs> relations;
	std::string origin;
	std::string dest;
}Problem;

//Defines the fringe information and functions.
typedef struct Fringe {
	//fringe queue ordered by path cost, lowest first
	std::deque<Node> nodes;
	int generated;
	int expanded;

	//used to prevent looping
	std::vector<Node> Visited;
	std::vector<Node> Failed;

	//check if empty function
	bool empty(Fringe f)
	{
		if (f.nodes.empty())
			return true;
		else
			return false;
	}

	//pop/push functions for nodes
	Node pop(Fringe** f)
	{
		Node ret = (**f).nodes.at(0);
		(**f).nodes.pop_front();
		//(**f).visited.push_back(ret);
		return ret;
	}

	void push(Fringe** f, Node** n)
	{
		(*f)->nodes.push_back(**n);
		return;
	}

	//Sort method
	void sort(Fringe** f)
	{
		Fringe* temp = *f;
		int n = temp->nodes.size();
		int i, j;
		Node key;

		for (i = 1; i < n; i++)
		{
			key = temp->nodes.at(i);
			j = i - 1;
			//if the node pathcost plus the estimate is greater than the key pathcost plus the estimate and j is greater or equal to 0...
			while (j >= 0 && ((temp->nodes.at(j).pathCost + temp->nodes.at(j).estimate) > (key.pathCost + key.estimate)))
			{
				temp->nodes.at(j + 1) = temp->nodes.at(j);
				j = j - 1;
			}

			temp->nodes.at(j + 1) = key;
		}
	}

	bool isInFailed(Node n)
	{
		bool present = false;
		for (Node f : Failed)
		{
			if (f.Name == n.Name)
			{
				present = true;
			}
		}

		return present;
	}
} Fringe;

//Defines the Heuristic used for estimation.
typedef struct Heuristic {
	std::string name;
	int estimate;
} Heuristic;

//File manipulation/ Node relationship building functions.

//extracts data from input file
//input: a string with the name of the data file
//output: string with the data
std::string readfile(std::string filename)
{
	std::fstream fp(filename, std::fstream::in);
	char* buffer = nullptr;
	if (fp.is_open())
	{
		fp.seekg(0, fp.end);
		int length = fp.tellg();
		fp.seekg(0, fp.beg);

		buffer = new char[length];
		fp.read(buffer, length);
		fp.close();
	}
	else
	{
		std::cout << "Failed to open file!" << std::endl;
	}

	std::string ret(buffer);
	delete(buffer);
	return ret;
}

int inputFilenameSize(char** input)
{
	char* ptr = *input;
	int count = 0;
	while (true)
	{
		if (strncmp(ptr, ".", 1) == 0)
		{
			break;
		}

		count++;
		ptr++;
	}

	return count + 4;
}

Arcs interpretDataLine(std::string dataline)
{
	//format is originCity destCity cost

	Arcs cp;
	int end = dataline.find(" ");
	//std::cout << "End pos: " << end << std::endl;
	cp.Name = dataline.substr(0, end);
	dataline.erase(0, end+1);
	end = dataline.find(" ");
	//std::cout << "End pos: " << end << std::endl;
	cp.Dest = dataline.substr(0, end);
	dataline.erase(0, end+1);
	std::string strcost = dataline.substr(0);
	cp.cost = stoi(strcost);
	//std::cout << "Origin: " << cp.Origin << std::endl << "Dest: " << cp.Dest << std::endl << "Cost: " << cost << std::endl;

	return cp;
}

Arcs interpretDataLineReverse(std::string dataline)
{
	//format is originCity destCity cost

	Arcs cp;
	int end = dataline.find(" ");
	//std::cout << "End pos: " << end << std::endl;
	cp.Dest = dataline.substr(0, end);
	dataline.erase(0, end + 1);
	end = dataline.find(" ");
	//std::cout << "End pos: " << end << std::endl;
	cp.Name = dataline.substr(0, end);
	dataline.erase(0, end + 1);
	std::string strcost = dataline.substr(0);
	cp.cost = stoi(strcost);
	//std::cout << "Origin: " << cp.Origin << std::endl << "Dest: " << cp.Dest << std::endl << "Cost: " << cost << std::endl;

	return cp;
}

std::pair<std::string,std::string> extractCityStates(std::string dataline)
{
	std::pair<std::string, std::string> states;

	int end = dataline.find(" ");
	//std::cout << "End pos: " << end << std::endl;
	states.first = dataline.substr(0, end);
	dataline.erase(0, end + 1);
	end = dataline.find(" ");
	//std::cout << "End pos: " << end << std::endl;
	states.second = dataline.substr(0, end);
	dataline.erase(0, dataline.length());
	return states;
}

std::vector<Arcs> buildListOfArcs(std::string data)
{
	std::vector<Arcs> ret;
	std::string delim = "\n";
	std::string token;
	int end = 0;
	do
	{
		//std::cout << "What is left:\n" << data << std::endl;
		end = data.find(delim);
		//std::cout << "End position: " << end << std::endl;
		token = data.substr(0, end);
		if (token == "END OF INPUT")
			break;
		//do something...
		ret.push_back(interpretDataLine(token));
		ret.push_back(interpretDataLineReverse(token));
		//end of do something...
		data.erase(0, end+1);
		//std::cout << std::endl;
		//std::this_thread::sleep_for(std::chrono::seconds(1));

	} while (end != -1);

	return ret;
}

std::vector<Arcs> findNodeArcs(std::string origin, std::vector<Arcs> relations)
{
	std::vector<Arcs> ret;
	for (Arcs cp : relations)
	{
		if (cp.Name == origin)
			ret.push_back(cp);
	}

	return ret;
}

//Initialization functions-------------------------------------------------------------------------------

void NodeInit(std::string N, Node* par, int d, int pc, std::string s, std::string r, Node* currNode)
{
	currNode->Name = N;

	//causes SIGSEGV, segmentation fault  - Solved
	currNode->parent = par;
	//----------------------------------
	currNode->pathCost = pc;
	currNode->depth = d;
	if (par == nullptr)
		currNode->route = r;
	else
		currNode->route =	par->route + r;
	currNode->states.push_back(s);
	currNode->estimate = 0;
	//if(par != nullptr)
		//std::cout << "Node Name: " << currNode->Name << "\nParent: " << currNode->parent << "\nCost: " << currNode->pathCost << "\nroute: " << currNode->route << std::endl;
	//else
		//std::cout << "Node Name: " << currNode->Name << "\nCost: " << currNode->pathCost << "\nroute: " << currNode->route << std::endl;
		
	return;
}

void NodeInitAStar(std::string N, Node* par, int d, int pc, std::string s, std::string r, int e, Node* currNode)
{
	currNode->Name = N;

	//causes SIGSEGV, segmentation fault  - Solved
	currNode->parent = par;
	//----------------------------------
	currNode->pathCost = pc;
	currNode->depth = d;
	if (par == nullptr)
		currNode->route = r;
	else
		currNode->route = par->route + r;
	currNode->states.push_back(s);
	currNode->estimate = e;

	//if(par != nullptr)
		//std::cout << "Node Name: " << currNode->Name << "\nParent: " << currNode->parent << "\nCost: " << currNode->pathCost << "\nroute: " << currNode->route << std::endl;
	//else
		//std::cout << "Node Name: " << currNode->Name << "\nCost: " << currNode->pathCost << "\nroute: " << currNode->route << std::endl;

	return;
}

void ProblemInit(std::vector<Arcs> rel, std::string o, std::string d, Problem* p)
{
	p->relations = rel;
	p->origin = o;
	p->dest = d;
	return;
}

void SolutionInit(std::string d, std::string r, int pc, int g, int e, Solution* s)
{
	s->dest = d;
	s->route = r;
	s->pathCost = pc;
	s->generatedNodes = g;
	s->expandedNodes = e;
	return;
}


//fringe insert functions-------------------------------------------------------------------------------
void fringeInsert(Fringe* f, Node* n)
{
	f->push(&f, &n);
	f->sort(&f);
	//f->generated++;
}

void fringeInsertAll(Fringe* f, std::vector<Node> ns)
{
	for (Node n : ns)
	{
		if (!f->isInFailed(n))
		{
			//std::cout << n.Name << std::endl;
			fringeInsert(f, &n);
			//f->Failed.push_back(n);
			//f->expanded++;

			//std::cout << "------------------------------" << std::endl;
		}
		else
		{
			std::cout << "Is in failed..." << std::endl;
		}
	}
}

//Testing functions

bool GoalTest(Problem p, Node n)
{
	//this may need adjustment
	if (n.Name == p.dest)
		return true;
}

bool inVisited(Fringe* f, std::string dest)
{
	for (Node n : f->Visited)
	{
		if (n.Name == dest)
			return true;
	}

	return false;
}

bool findInList(std::vector<std::string> list, std::string str)
{
	//int count = 0;
	for (std::string s : list)
	{
		//std::cout << "Searching: " << s << " At: " << list.at(count) << " count: " << count << std::endl;

		if (str == s)
			return true;

		//count++;
	}

	return false;
}

int isInList(std::vector<std::string> list, std::string s1, std::string s2)
{
	int ret = -1;
	bool f = findInList(list, s1);
	bool s = findInList(list, s2);
	//std::cout << "First: " << f << " Second: " << s << std::endl;
	if (f == true && s == true)
	{
		ret = 2;
		return ret;
	}
	else if (f == true)
	{
		ret = 0;
		return ret;
	}
	else if (s == true)
	{
		ret = 1;
		return ret;
	}
	else
		return ret;


}

//Conversions -----------------------------------------------------------------------------------------
std::string nodeToString(Node n)
{
	return "node: " + n.Name + " parent: " + n.parent->Name;
}

std::string nodeListToString(std::vector<Node> v)
{
	std::string s = "";
	for (Node n : v)
	{
		s += nodeToString(n) + " ";
	}

	return s;
}

Node arcsToNodes(Arcs a, Node* par)
{

	std::string st = "";

	if (par->parent != nullptr)
	{
		for (std::string sta : par->states)
		{
			st += sta;
		}
	}
	
	st += par->Name;

	Node s;
	NodeInit(a.Dest, par, par->depth + 1, par->pathCost + a.cost, st , par->Name + " -> " + a.Dest + " " + std::to_string(a.cost) + ".0 km\n", &s);

	return s;
}

//Uniform Cost functions-------------------------------------------------------------------------------
//Takes the node and generates the successors based on the relations in the problem.
bool expand(Node* n, Problem* p, Fringe* f, std::vector<Node> * successors)
{
	std::vector<Arcs> potential = findNodeArcs(n->Name, p->relations);

	//if the node has no potential children then this particular route has failed.
	if (potential.size() == 0)
	{
		return false;
	}
	else
	{
		f->expanded++;

		for (auto p : potential)
		{
			if (n->parent == nullptr)
			{
				Node s = arcsToNodes(p, n);
				f->generated++;
				successors->push_back(s);
				f->Visited.push_back(s);
			}
			else 
			{
				Node s = arcsToNodes(p, n);

				if (n->parent->Name == p.Dest)
				{
					f->expanded--;
					continue;
				}
				else if (!f->isInFailed(s) && !inVisited(f, p.Dest))
				{
					f->generated++;
					successors->push_back(s);
					f->Visited.push_back(s);
				}
			}
		}
	}

	return true;
}

void updateFailed(Fringe* f, Node n)
{
	if (!f->isInFailed(n))
	{
		f->Failed.push_back(n);
	}
}

Solution uniformCostSearch(Problem* p, Fringe* f)
{
	Solution ret;
	SolutionInit(p->dest, "", -1, 0, 0, &ret);
	Node start;
	NodeInit(p->origin, nullptr, 0, 0, p->origin , "",&start);
	f->Visited.push_back(start);
	f->generated++;
	fringeInsert(f, &start);

	std::deque<Node> goalNodes;

	while (!f->empty(*f))
	{
		f->sort(&f);
		Node n = f->pop(&f);
		if (GoalTest(*p, n))
		{
			goalNodes.push_back(n);
			break;
		}
		else
		{
			std::vector<Node> successors;
			if (expand(&n, p, f, &successors))
			{
				fringeInsertAll(f, successors);
			}
			else
			{
				updateFailed(f, n);
			}
		}
	}

	if (goalNodes.size() != 0)
	{
		int pos = 0;
		int minpos = 0;
		int max = std::numeric_limits<int>::max();
		for (Node n : goalNodes)
		{
			if (n.pathCost < max)
			{
				max = n.pathCost;
				minpos = pos;
			}
			pos++;
		}

		Node temp = goalNodes.at(minpos);
		ret.pathCost = temp.pathCost;
		ret.route = temp.route;
		ret.expandedNodes = f->expanded;
		ret.generatedNodes = f->generated;
	}
	
	return ret;
}

std::vector<std::string> findAllStates(std::string data)
{
	std::vector<std::string> ret;

	//input has \r\n may need to see about adjusting this if it causes issues.
	std::string delim = "\n";
	std::string token;
	int end = 0;
	do
	{
		end = data.find(delim);
		token = data.substr(0, end);
		if (token == "END OF INPUT")
			break;
		std::pair<std::string, std::string> states = extractCityStates(token);
		int pos = isInList(ret, states.first, states.second);
		if (pos == -1)
		{
			ret.push_back(states.first);
			ret.push_back(states.second);
		}
		else if (pos == 0)
		{
			ret.push_back(states.second);
		}
		else if (pos == 1)
		{
			ret.push_back(states.first);
		}
		data.erase(0, end + 1);
	} while (end != -1);

	return ret;
}

//heuristics portion-----------------------------------------------------------------------------------

void InitHeuristics(std::string n, int e, Heuristic* h)
{
	h->name = n;
	h->estimate = e;
}

Heuristic interpretHeuristic(std::string dataline)
{
	Heuristic h;

	int end = dataline.find(" ");
	std::string Name = dataline.substr(0, end);
	dataline.erase(0, end + 1);
	end = dataline.find("\n");
	std::string est = dataline.substr(0, end);
	int estimate = stoi(est);

	InitHeuristics(Name, estimate, &h);

	return h;
}

std::vector<Heuristic> buildListOfHeuristics(std::string data)
{
	std::vector<Heuristic> ret;
	std::string delim = "\n";
	std::string token;
	int end = 0;
	do
	{
		end = data.find(delim);
		token = data.substr(0, end);
		if (token == "END OF INPUT")
			break;
		ret.push_back(interpretHeuristic(token));
		data.erase(0, end + 1);
	} while (end != -1);

	return ret;
}

int findHeuristicFromName(std::string n, std::vector<Heuristic> h)
{
	int counter = 0;
	for (auto hue : h)
	{
		if (hue.name == n)
		{
			return counter;
		}
		counter++;
	}

	return 0;
}

Node aStarArcsToNodes(Arcs a, Node* par, Heuristic h)
{
	//std::cout << "Here I am (aStarAToN)!" << std::endl;

	std::string st = "";

	if (par->parent != nullptr)
	{
		for (std::string sta : par->states)
		{
			st += sta;
		}
	}
	
	st += par->Name;

	Node s;
	NodeInitAStar(a.Dest, par, par->depth + 1, par->pathCost + a.cost, st, par->Name + " -> " + a.Dest + " " + std::to_string(a.cost) + ".0 km\n", h.estimate, &s);

	//std::cout << "Leaving (aStarAToN)!" << std::endl;
	return s;
}

bool aStarExpand(Node* n, Problem* p, Fringe* f, std::vector<Node>* successors, std::vector<Heuristic> h)
{
	std::vector<Arcs> potential = findNodeArcs(n->Name, p->relations);

	//if the node has no potential children then this particular route has failed.
	if (potential.size() == 0)
	{
		return false;
	}
	else
	{
		f->expanded++;

		for (auto p : potential)
		{
			if (n->parent == nullptr)
			{
				//the destination is what is used to generate the new node.
				Heuristic tmp = h.at(findHeuristicFromName(p.Dest, h));
				Node s = aStarArcsToNodes(p, n, tmp);
				f->generated++;
				successors->push_back(s);
				f->Visited.push_back(s);
			}
			else
			{
				Heuristic tmp = h.at(findHeuristicFromName(p.Dest, h));
				Node s = aStarArcsToNodes(p, n, tmp);

				if (n->parent->Name == p.Dest)
				{
					f->expanded--;
					continue;
				}
				else if (!f->isInFailed(s) && !inVisited(f, p.Dest))
				{
					f->generated++;
					successors->push_back(s);
					f->Visited.push_back(s);
				}
			}
		}
	}
	return true;
}

std::deque<Node> aStarSearch(Problem* p, Fringe* f, std::vector<Heuristic> h)
{
	std::deque<Node> goalNodes;

	while (!f->empty(*f))
	{
		f->sort(&f);
		Node n = f->pop(&f);


		if (GoalTest(*p, n))
		{
			goalNodes.push_back(n);
			break;
		}
		else
		{
			std::vector<Node> successors;
			//if there are any successors to the current node...
			if (aStarExpand(&n, p, f, &successors, h))
			{
				//insert them into the fringe.
				fringeInsertAll(f, successors);
			}
			else
			{
				updateFailed(f, n);
			}
		}
	}
	return goalNodes;
}

Solution aStarSearchSetup(Problem* p, Fringe* f, std::vector<Heuristic> h)
{
	Solution ret;
	SolutionInit(p->dest, "", -1, 0, 0, &ret);
	Node start;
	NodeInitAStar(p->origin, nullptr, 0, 0, p->origin, "", h.at(findHeuristicFromName(p->origin, h)).estimate, &start);
	f->Visited.push_back(start);
	f->generated++;
	fringeInsert(f, &start);


	std::deque<Node> goalNodes = aStarSearch(p, f, h);

	if (goalNodes.size() != 0)
	{
		int pos = 0;
		int minpos = 0;
		int max = std::numeric_limits<int>::max();
		for (Node n : goalNodes)
		{
			if (n.pathCost < max)
			{
				max = n.pathCost;
				minpos = pos;
			}
			pos++;
		}

		Node temp = goalNodes.at(minpos);
		ret.pathCost = temp.pathCost;
		ret.route = temp.route;
		ret.expandedNodes = f->expanded;
		ret.generatedNodes = f->generated;
	}

	return ret;
}

//Main function-----------------------------------------------------------------------------------------
int main(int args, char * argv[])
{
	if(args == 1)
	{
		std::cout << "Please use command line arguments." << std::endl;
	}
	else if (args == 2)
	{
		std::cout << "Please input an origin city and a destination city and/or a heuristic file into the command line." << std::endl;
	}
	else if (args == 3)
	{
		//and/or a heuristic file
		std::cout << "Please input a destination city and/or a heuristic file into the command line." << std::endl;
	}
	else if (args == 4)
	{
		//command line arguments format
		//find_route input_filename origin_city destination_city heuristic_filename
		std::string filename(argv[1]);
		std::string originCity(argv[2]);
		std::string destCity(argv[3]);
		
		std::string data = readfile(filename);
		std::vector<Arcs> relations = buildListOfArcs(data);
		std::vector<std::string> allStates = findAllStates(data);
		//from here out use uninformed search to find solution.
		Problem problem;
		
		ProblemInit(relations, originCity, destCity, &problem);
		Fringe fringe;
		fringe.generated = 0;
		fringe.expanded = 0;

		Solution s = uniformCostSearch(&problem, &fringe);
		
		std::cout << "Nodes Expanded: " << s.expandedNodes << std::endl;
		std::cout << "Nodes Generated: " << s.generatedNodes << std::endl;
		if (s.pathCost != -1)
		{
			std::cout << "Distance: " << s.pathCost << ".0 km" << std::endl;
			std::cout << "Route: \n" << s.route << std::endl;
		}
		else
		{
			std::cout << "Distance: " << "Infinite" << std::endl;
			std::cout << "Route: \n" << "None" << std::endl;
		}
		
	}
	else if(args == 5)
	{
		//command line arguments format
		//find_route input_filename origin_city destination_city heuristic_filename
		std::string filename(argv[1]);
		std::string originCity(argv[2]);
		std::string destCity(argv[3]);
		std::string heuristic(argv[4]);
		std::string relationdata = readfile(filename);
		std::string heuristicdata = readfile(heuristic);
		std::vector<Arcs> relations = buildListOfArcs(relationdata);
		std::vector<Heuristic> heuristics = buildListOfHeuristics(heuristicdata);
		//from here out use informed search to find solution.
		Problem problem;
		ProblemInit(relations, originCity, destCity, &problem);

		Fringe fringe;
		fringe.generated = 0;
		fringe.expanded = 0;

		
		Solution s = aStarSearchSetup(&problem, &fringe, heuristics);
		std::cout << "Nodes Expanded: " << s.expandedNodes << std::endl;
		std::cout << "Nodes Generated: " << s.generatedNodes << std::endl;
		if (s.pathCost != -1)
		{
			std::cout << "Distance: " << s.pathCost << ".0 km" << std::endl;
			std::cout << "Route: \n" << s.route << std::endl;
		}
		else
		{
			std::cout << "Distance: " << "Infinite" << std::endl;
			std::cout << "Route: \n" << "None" << std::endl;
		}
	}

	return 0;
}

//compilation:
//g++ find_route.cpp -o find_route
//./find_route input1.txt Luebeck Hamburg
// or 
//./find_route input1.txt Luebeck Kassel h_kassel.txt
