#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <bits/stdc++.h>

// hello
// TODO
// check parent string and children int

// Format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;

// Our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node
{

private:
	string Node_Name;	   // Variable name
	int Node_Index;		   // Index of the node in the network
	vector<int> Children;  // Children of a particular node - these are index of nodes in the graph.
	vector<int> Parents;   // Parents of a particular node - note these are names of parents
	int nvalues;		   // Number of categories a variable represented by this node can take
	vector<string> values; // Categories of possible values
	vector<float> CPT;	   // Conditional probability table as a 1-d array. Look for BIF format to understand its meaning

public:
	// Constructor- a node is initialized with its name and its categories
	Graph_Node(string name, int n, vector<string> vals, int index)
		: Node_Name(name), Node_Index(index), Children(), Parents(), nvalues(n), values(vals), CPT()
	{
	}

	// Graph_Node(string name, int n, vector<string> vals, int index)
	// {
	// 	Node_Name = name;
	// 	nvalues = n;
	// 	values = vals;
	// 	Node_Index = index;
	// }

	string get_name()
	{
		return Node_Name;
	}
	vector<int> get_children()
	{
		return Children;
	}
	vector<int> get_Parents()
	{
		return Parents;
	}
	vector<float> get_CPT()
	{
		return CPT;
	}
	int get_nvalues()
	{
		return nvalues;
	}
	vector<string> get_values()
	{
		return values;
	}
	void set_CPT(vector<float> new_CPT)
	{
		CPT.clear();
		CPT = new_CPT;
	}
	void set_Parents(vector<int> Parent_Nodes)
	{
		Parents.clear();
		Parents = Parent_Nodes;
	}
	// add another node in a graph as a child of this node
	void add_child(int new_child_index)
	{
		// Removed the looping below and assume added child is not already present
		// for (int i = 0; i < Children.size(); i++)
		// {
		// 	if (Children[i] == new_child_index)
		// 		return 0;
		// }
		Children.push_back(new_child_index);
		// return 1;
	}
};

// The whole network represted as a list of nodes
class network
{

private:
	vector<Graph_Node *> Pres_Graph;
	unordered_map<string, int> index_map; // Maps node names to their indices

public:
	// Constructor for the network class
	network()
		: Pres_Graph(), index_map()
	{
	}
	void CPT_initializer();

	int addNode(Graph_Node *node)
	{
		int index = static_cast<int>(Pres_Graph.size()); // Get the index of the added node
		index_map[node->get_name()] = index;			 // Add the mapping to the map
		Pres_Graph.push_back(node);
		return 0;
	}

	int netSize()
	{
		return static_cast<int>(Pres_Graph.size());
	}
	// Gets the index of node with a given name
	int get_index(string val_name)
	{
		// TODO: can get rid of the map.find() part if it makes it faster
		if (index_map.find(val_name) != index_map.end())
		{
			return index_map[val_name];
		}
		return -1; // Node not found
	}

	// Gets the pointer of node at nth index
	Graph_Node *get_nth_node(int n)
	{
		return Pres_Graph[n];
	}

	// Gets the pointer of a node with a given name
	Graph_Node *search_node(string val_name)
	{
		return Pres_Graph[index_map[val_name]];
		// return get_nth_node(get_index(val_name));
	}
};

network read_network()
{
	network Graph_network = network();
	string line;
	int find = 0;
	ifstream myfile("alarm.bif");
	string temp;
	string name;

	if (myfile.is_open())
	{
		while (!myfile.eof())
		{
			stringstream ss;
			getline(myfile, line);

			ss.str(line);
			ss >> temp;

			if (temp.compare("variable") == 0)
			{
				vector<string> values;

				ss >> name;
				getline(myfile, line);

				stringstream ss2;
				ss2.str(line);
				for (int i = 0; i < 4; i++)
				{

					ss2 >> temp;
				}
				values.clear();
				while (temp.compare("};") != 0)
				{
					values.push_back(temp);

					ss2 >> temp;
				}

				int index = Graph_network.netSize(); // Get the index of the added node
				Graph_Node *new_node = new Graph_Node(name, values.size(), values, index);
				Graph_network.addNode(new_node);
				// Graph_network.index_map[name] = pos;
			}
			else if (temp.compare("probability") == 0)
			{

				// temp represents name of variables.
				ss >> temp;
				ss >> temp;
				// first variable name is the name of the current node.
				Graph_Node *curr_node = Graph_network.search_node(temp);
				int index = Graph_network.get_index(temp);

				// remaining variable names are the name of the parents.
				ss >> temp;
				vector<int> parents;
				while (temp.compare(")") != 0)
				{
					// We find the parent node, modify its children list
					// and add the parent name to the parents list of the current node.
					Graph_Node *parent = Graph_network.search_node(temp);
					parent->add_child(index);
					parents.push_back(Graph_network.get_index(temp));

					ss >> temp;
				}
				curr_node->set_Parents(parents);
				getline(myfile, line);
				stringstream ss2;

				ss2.str(line);
				ss2 >> temp;

				// Here temp represents each value stored in the CPT.
				ss2 >> temp;

				vector<float> curr_CPT;
				// string::size_type sz;

				// iterate through and add each value to the CPT of the current node.
				while (temp.compare(";") != 0)
				{
					curr_CPT.push_back(atof(temp.c_str()));

					ss2 >> temp;
				}

				curr_node->set_CPT(curr_CPT);
			}
		}

		//! what is the purpose of 'find' here? it is not modified anywhere above.
		if (find == 1)
			myfile.close();
	}

	return Graph_network;
}

class Data
{

public:
	network Data_network;
	int total_variables;			// Total number of variables in the data
	vector<vector<int>> data;		// Data is stored as integer format. Suppose a node can take 3 categories as values. Then first category is '0', then '1' and so on.
	vector<int> missing;			// Stores index of question mark for data i. -1 if not there. Also assumes atmost one '?' per data point.
	vector<vector<int>> datapoints; // Stores the useful data, after replacing '?' with possible values
	vector<double> weights;			// Weights for the data that we will be using

	Data(network Alarm)
	{
		Data_network = Alarm;
		total_variables = Alarm.netSize();
		data_reader();
	};

	void cpt_updater();
	void dataUpdater();
	float probabilityCalc(int i, int var, int j);

	void data_reader()
	{
		string line;
		ifstream myfile("records.dat");
		string temp;
		string name;

		if (!myfile.is_open())
			return;
		else
		{
			while (!myfile.eof())
			// for each line in the file
			{
				stringstream ss;
				getline(myfile, line);
				vector<int> values_dataset; // Stores the values of the data
				ss.str(line);
				int missing_idx = -1; // Stores the index of the unknown value
				for (int i = 0; i < total_variables; i++)
				// for each variable value in the line
				{
					// temp represents the value of the ith variable
					ss >> temp;
					// if (temp.compare("?") == 0)
					// std::cout << temp << std::endl;
					if (temp == "\"?\"")
					{
						missing_idx = i;
						values_dataset.push_back(-1);
					}
					else
					{
						int data_value = 0;
						// go through the list of values of the node
						int k = Data_network.get_nth_node(i)->get_nvalues();
						for (int j = 0; j < k; j++)
						{
							if (temp.compare(Data_network.get_nth_node(i)->get_values()[j]) == 0)
							{
								data_value = j;
								break;
							}
						}
						values_dataset.push_back(data_value);
					}
				}
				datapoints.push_back(values_dataset);
				missing.push_back(missing_idx);

				if (missing_idx != -1)
				{
					int node_idx = missing_idx;
					vector<int> probable_values;
					int k = Data_network.get_nth_node(node_idx)->get_nvalues();
					for (int i = 0; i < k; i++)
					{
						// copying values_dataset into probable_values
						vector<int> probable_values(values_dataset.begin(), values_dataset.end());
						probable_values[missing_idx] = i;
						data.push_back(probable_values);
						weights.push_back(1 / static_cast<float>(k));
					}
				}
				else
				{
					data.push_back(values_dataset);
					weights.push_back(1.0);
				}
			}
		}
	}
};

void network::CPT_initializer()
{
	for (Graph_Node *listIt : Pres_Graph)
	{
		// Graph_Node *ListIt = ;
		int nvalues = listIt->get_nvalues();
		vector<float> CPT;
		vector<int> Parents = listIt->get_Parents();
		int num_parents = Parents.size();
		int num_combinations = nvalues;
		for (int i = 0; i < num_parents; i++)
		{
			Graph_Node *parent = get_nth_node(Parents[i]);
			int n = parent->get_nvalues();
			num_combinations *= n;
		}

		for (int i = 0; i < num_combinations; i++)
		{
			CPT.push_back(1.0 / nvalues);
		}
		listIt->set_CPT(CPT);
	}
}

void Data::dataUpdater()
{
	weights.clear(); // Clear the 'weights' vector.

	for (int i = 0; i < datapoints.size(); i++)
	{
		int missing_idx = missing[i];
		if (missing_idx != -1)
		{
			weights.push_back(1);
			continue;
		}
		else
		{
			// Initialize a vector to store probabilities and a sum variable.
			vector<float> probs;

			int j = 0;

			int nvalues = Data_network.get_nth_node(missing_idx)->get_nvalues();

			while (j < nvalues)
			{
				// i is real data index, isse hum joint probability nikalenge
				float probab = probabilityCalc(i, missing_idx, j);
				for (int k = 0; k < Data_network.get_nth_node(missing_idx)->get_children().size(); k++)
				{

					int child_idx = Data_network.get_nth_node(missing_idx)->get_children()[k];
					// joint probability of child given parent(current node)
					float child_probab = probabilityCalc(i, child_idx, datapoints[i][child_idx]);
					probab *= child_probab;
				}
				probs.push_back(probab);
				j++;
			}
			int max_index = max_element(probs.begin(), probs.end()) - probs.begin();
			datapoints[i][missing_idx] = max_index;

			// sum of all probabilities
			float sum = accumulate(probs.begin(), probs.end(), 0.0);
			// Update the weights vector.
			for (int j = 0; j < probs.size(); j++)
			{
				weights.push_back(probs[j] / sum);
			}
			// for (int j = 0; j < probs.size(); j++)
			// {
			// 	weights.push_back(probs[j]);
			// }
		}
	}
}

// Finds P(var = j | parents of var), by finding position of that value in the CPT of var node.
// i is index of data point, var is index of variable, j is index of value of variable.
// Implied that var is the likely missing variable, and it's parents are known.
float Data::probabilityCalc(int datapt_idx, int var, int var_val)
{
	float probab = 1;
	// get parents of var
	//! copying is happening here, might affect performance
	vector<int> parents = Data_network.get_nth_node(var)->get_Parents();
	// if no parents
	if (parents.size() == 0)
	{
		probab = Data_network.get_nth_node(var)->get_CPT()[var_val];
	}
	else
	{
		// position of CPT val = p1val* 1 + p2val * (p1) + p3val * (p1*p2) ... + var_val* Prod(all p nvals).
		// Note: In the above, p is numbered from the back.
		// get index of parent
		int base = 1;
		int CPT_idx = 0;
		//? Possible to make this faster and without a loop?
		for (int k = static_cast<int>(parents.size()) - 1; k >= 0; k--)
		{
			int parent_idx = parents[k];
			// get value of parent.
			int parent_val = datapoints[datapt_idx][parent_idx];

			CPT_idx += parent_val * base;
			// base is multiplied with number of values of current parent.
			base *= Data_network.get_nth_node(parent_idx)->get_nvalues();
		}
		CPT_idx += var_val * base;
		probab = Data_network.get_nth_node(var)->get_CPT()[CPT_idx];
	}
	return probab;
}

void Data::cpt_updater(){

};

int main()
{
	network Alarm = read_network();
	Data data = Data(Alarm);
	return 0;
}
