#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <bits/stdc++.h>

// hello
// Format checker just assumes you have Alarm.bif and Solved_Alarm.bif (your file) in current directory
using namespace std;

// Our graph consists of a list of nodes where each node is represented as follows:
class Graph_Node
{

private:
	string Node_Name;		// Variable name
    int Node_Index; // Index of the node in the network
	vector<int> Children;	// Children of a particular node - these are index of nodes in graph.
	vector<string> Parents; // Parents of a particular node- note these are names of parents
	int nvalues;			// Number of categories a variable represented by this node can take
	vector<string> values;	// Categories of possible values
	vector<float> CPT;		// conditional probability table as a 1-d array . Look for BIF format to understand its meaning

public:
	// Constructor- a node is initialised with its name and its categories
	// Graph_Node(string name, int n, vector<string> vals)
	Graph_Node(string name, int n, vector<string> vals, int index)
	{
		Node_Name = name;
		nvalues = n;
		values = vals;
		Node_Index = index;
	}

	string get_name()
	{
		return Node_Name;
	}
	vector<int> get_children()
	{
		return Children;
	}
	vector<string> get_Parents()
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
	void set_Parents(vector<string> Parent_Nodes)
	{
		Parents.clear();
		Parents = Parent_Nodes;
	}
	// add another node in a graph as a child of this node
	int add_child(int new_child_index)
	{
		for (int i = 0; i < Children.size(); i++)
		{
			if (Children[i] == new_child_index)
				return 0;
		}
		Children.push_back(new_child_index);
		return 1;
	}
};

// The whole network represted as a list of nodes
class network
{

	list<Graph_Node> Pres_Graph;
    map<string, int> node_id; // Maps node names to their indices

public:
	// int addNode(Graph_Node node)
	// {
	// 	Pres_Graph.push_back(node);
	// 	return 0;
	// }
    int addNode(Graph_Node node)
    {
        int index = Pres_Graph.size(); // Get the index of the added node
        node_id[node.get_name()] = index; // Add the mapping to the map
        Pres_Graph.push_back(node);
        return 0;
    }

	int netSize()
	{
		return Pres_Graph.size();
	}
	// get the index of node with a given name
	int get_index(string val_name){
		if (node_id.find(val_name) != node_id.end()) {
			return node_id[val_name];
		}
		return -1; // Node not found
	}

	/*
	int get_index(string val_name)
	{
		list<Graph_Node>::iterator listIt;
		int count = 0;
		for (listIt = Pres_Graph.begin(); listIt != Pres_Graph.end(); listIt++)
		{
			if (listIt->get_name().compare(val_name) == 0)
				return count;
			count++;
		}
		return -1;
	}*/
	
	// get the node at nth index
	list<Graph_Node>::iterator get_nth_node(int n)
	{
		list<Graph_Node>::iterator listIt;
		int count = 0;
		for (listIt = Pres_Graph.begin(); listIt != Pres_Graph.end(); listIt++)
		{
			if (count == n)
				return listIt;
			count++;
		}
		return listIt;
	} 

	// get the iterator of a node with a given name
	list<Graph_Node>::iterator search_node(string val_name)
	{
		list<Graph_Node>::iterator listIt;
		for (listIt = Pres_Graph.begin(); listIt != Pres_Graph.end(); listIt++)
		{
			if (listIt->get_name().compare(val_name) == 0)
				return listIt;
		}

		cout << "node not found\n";
		return listIt;
	}
};


network read_network()
{
	network Alarm;
	string line;
	int find = 0;
	ifstream myfile("alarm.bif");
	string temp;
	string name;
	vector<string> values;

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
				Graph_Node new_node(name, values.size(), values);
				int pos = Alarm.addNode(new_node);
			}
			else if (temp.compare("probability") == 0)
			{

				ss >> temp;
				ss >> temp;

				list<Graph_Node>::iterator listIt;
				list<Graph_Node>::iterator listIt1;
				listIt = Alarm.search_node(temp);
				int index = Alarm.get_index(temp);
				ss >> temp;
				values.clear();
				while (temp.compare(")") != 0)
				{
					listIt1 = Alarm.search_node(temp);
					listIt1->add_child(index);
					values.push_back(temp);

					ss >> temp;
				}
				listIt->set_Parents(values);
				getline(myfile, line);
				stringstream ss2;

				ss2.str(line);
				ss2 >> temp;

				ss2 >> temp;

				vector<float> curr_CPT;
				string::size_type sz;
				while (temp.compare(";") != 0)
				{

					curr_CPT.push_back(atof(temp.c_str()));

					ss2 >> temp;
				}

				listIt->set_CPT(curr_CPT);
			}
			else
			{
			}
		}

		if (find == 1)
			myfile.close();
	}

	return Alarm;
}

class data{
    network Pres_Graph;
    int total_variables; // Total number of variables in the data
	vector<vector<int>> data; // Data is stored as integer format. Suppose a node can take 3 categories as values. Then first category is '0', then '1' and so on.
	vector<int> missing;	  // Stores index of question mark for data i
	vector<vector<int>> datapoints; // Stores the useful data, after replacing '?' with possible values
	vector<double> weights;			// Weights for the data that we will be using
public:

    void data::data_reader(){
        string line;
        ifstream myfile("filename1");
        string temp;
        string name;

        if (!myfile.is_open())
            return;
        else
        {
            while (!myfile.eof())
            {
                stringstream ss;
                getline(myfile, line);
                vector<int> values_dataset; // Stores the values of the data
                ss.str(line);
                int missing_idx = -1; // Stores the index of the unknown value
                for (int i = 0; i < total_variables; i++)
                {
                    ss >> temp;
                    if (temp.compare("?") == 0)
                    {
                        missing_idx = i;
                        values_dataset.push_back(-1);
                    }
                    else
                    {
                        int data_value = 0;
                        // go through the list of values of the node
                        int k = Pres_Graph.get_nth_node(i)->get_nvalues();
                        for (int j = 0; j < k; j++)
                        {
                            if (temp.compare(Pres_Graph.get_nth_node(i)->get_values()[j]) == 0)
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

                if(missing_idx == -1){
                    int node_idx = missing_idx;
                    vector<int> probable_values;
                    int k = Pres_Graph.get_nth_node(node_idx)->get_nvalues();
                    for(int i = 0; i < k; i++){
                        vector<int> probable_values(values_dataset.begin(), values_dataset.end());
                        probable_values[missing_idx] = i ;
                        data.push_back(probable_values);
                        weights.push_back(1 / k);
                    }
                }else{
                    data.push_back(values_dataset);
                    weights.push_back(1);
                }
            }
        }   
    }
};

void CPT_initializzer()