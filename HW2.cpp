// HW2.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <istream>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

struct ecoli
{
	int index;
	string name;
	double values[9];
	string species;
};

struct node
{
	struct ecoli spec;
	int axis;
	struct node* left;
	struct node* right;
	bool isleaf;
};

struct list
{
	struct ecoli spec;
	double distance;
};

void Input(vector<ecoli> &data, char* file)
{
	ifstream input(file, ifstream::in); ////change name to constant
	string strbuf;
	char charbuf;
	double doubuf;
	int intbuf;
	getline(input, strbuf, '\n');
	for (int i = 0; i < data.size(); i++)
	{
		input >> intbuf;
		data[i].index = intbuf;
		input >> charbuf;
		getline(input, strbuf, ',');
		data[i].name = strbuf;
		//input >> charbuf;
		for (int j = 0; j < 9; j++)
		{
			input >> doubuf;
			input >> charbuf;
			data[i].values[j] = doubuf;
		}
		//getline(input, strbuf, '\n');
		input >> strbuf;
		data[i].species = strbuf;
	}
	return;
}

int compunit;
bool Compare(ecoli &A, ecoli &B)
{
	return A.values[compunit] < B.values[compunit];
}

void ConstructTree(vector<ecoli> data, struct node &root, int level)
{
	//cout << level << " " << data.size() << endl;
	root.axis = level % 9;
	if (data.size() == 1)
	{
		root.isleaf = true;
		root.spec = data[0];
		return;
	}
	root.isleaf = false;
	compunit = root.axis;
	sort(data.begin(), data.end(), Compare);
	root.spec = data[data.size() / 2];
	vector<ecoli> left, right;
	left.assign(data.begin(), data.begin() + data.size() / 2);
	right.assign(data.begin() + data.size() / 2 + 1, data.end());
	if (left.size() == 0)
	{
		root.left = NULL;
	}
	else
	{
		root.left = new node;
		ConstructTree(left, *root.left, level + 1);
	}
	if (right.size() == 0)
	{
		root.right = NULL;
	}
	else
	{
		root.right = new node;
		ConstructTree(right, *root.right, level + 1);
	}
	return;
}

void ResetList(vector<list> &nearest_list)
{
	for (int i = 0; i < nearest_list.size(); i++)
	{
		nearest_list[i].distance = 100000000;
	}
	return;
}

double CalcDistance(struct ecoli A, struct ecoli B)
{
	double ans = 0;
	for (int i = 0; i < 9; i++)
	{
		ans += (A.values[i] - B.values[i]) * (A.values[i] - B.values[i]);
	}
	return sqrt(ans);
}

bool Compare2(struct list &A, struct list &B)
{
	return(A.distance < B.distance);
}

void FindNearest(struct ecoli spec, struct node root, vector<list> &nearest_list)
{
	struct list newnode;
	newnode.spec = root.spec;
	newnode.distance = CalcDistance(spec, root.spec);
	nearest_list.push_back(newnode);
	sort(nearest_list.begin(), nearest_list.end(), Compare2);
	nearest_list.pop_back();
	if (root.isleaf == true)
	{
		return;
	}
	if (spec.values[root.axis] < root.spec.values[root.axis])
	{
		if (root.left != NULL)
		{
			FindNearest(spec, *root.left, nearest_list);
		}
		if (nearest_list[nearest_list.size() - 1].distance > abs(spec.values[root.axis] - root.spec.values[root.axis]) && root.right != NULL)
		{
			FindNearest(spec, *root.right, nearest_list);
		}
	}
	else
	{
		if (root.right != NULL)
		{
			FindNearest(spec, *(root.right), nearest_list);
		}
		if (nearest_list[nearest_list.size() - 1].distance > abs(spec.values[root.axis] - root.spec.values[root.axis]) && root.left != NULL)
		{
			FindNearest(spec, *root.left, nearest_list);
		}
	}
	return;
}

void TestTree(vector<ecoli> data, struct node root, int K, vector<vector<int>> &tinylist)
{
	vector<list> nearest_list;
	nearest_list.resize(K);
	for (int i = 0; i < 36; i++)
	{
		ResetList(nearest_list);
		FindNearest(data[i], root, nearest_list);
		for (int j = 0; j < K; j++)
		{
			tinylist[i][j] = nearest_list[j].spec.index;
		}
	}
}

void TinylistInit(vector<vector<int>> &tinylist, int K, int size)
{
	tinylist.resize(size);
	for (int i = 0; i < size; i++)
	{
		tinylist[i].resize(K);
	}
	return;
}

void EvalAccuracy(vector<ecoli> train_data, vector<ecoli> test_data, vector<vector<int>> tinylist)
{
	//8 classes: cp=0, im=1, pp=2, imU=3, om=4, omL=5, inL=6, imS=7
	int record[8];
	int correct = 0;
	for (int i = 0; i < tinylist.size(); i++)
	{
		for (int j = 0; j < 8; j++)
		{
			record[j] = 0;
		}
		for (int j = 0; j < tinylist[0].size(); j++)
		{
			if (train_data[tinylist[i][j]].species == "cp")
			{
				record[0]++;
			}
			else if (train_data[tinylist[i][j]].species == "im")
			{
				record[1]++;
			}
			else if (train_data[tinylist[i][j]].species == "pp")
			{
				record[2]++;
			}
			else if (train_data[tinylist[i][j]].species == "imU")
			{
				record[3]++;
			}
			else if (train_data[tinylist[i][j]].species == "om")
			{
				record[4]++;
			}
			else if (train_data[tinylist[i][j]].species == "omL")
			{
				record[5]++;
			}
			else if (train_data[tinylist[i][j]].species == "imL")
			{
				record[6]++;
			}
			else if (train_data[tinylist[i][j]].species == "imS")
			{
				record[7]++;
			}
			else
			{
				cout << "error" << " " << train_data[tinylist[i][j]].species << endl;
			}
		}
		int most_species = 0, most_num = 0;
		for (int j = 0; j < 8; j++)
		{
			if (record[j] > most_num)
			{
				most_species = j;
				most_num = record[j];
			}
		}
		if ((most_species == 0 && test_data[i].species == "cp") || (most_species == 1 && test_data[i].species == "im") || (most_species == 2 && test_data[i].species == "pp") || (most_species == 3 && test_data[i].species == "imU") || (most_species == 4 && test_data[i].species == "om") || (most_species == 5 && test_data[i].species == "omL") || (most_species == 6 && test_data[i].species == "imL") || (most_species == 7 && test_data[i].species == "omS"))
		{
			correct++;
		}
	}
	cout << "KNN accuracy: " << correct / (double)tinylist.size() << endl;
	for (int i = 0; i < tinylist.size(); i++)
	{
		for (int j = 0; j < tinylist[0].size(); j++)
		{
			cout << tinylist[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
	return;
}

int main(int argc, char* argv[])
{
	vector<ecoli> train_data, test_data;
	train_data.resize(300);
	test_data.resize(36);
	/**********change to constant filename**********/
	argv[1] = "train.csv";
	argv[2] = "test.csv";
	/***********************************************/
	Input(train_data, argv[1]);
	Input(test_data, argv[2]);
	struct node root;
	ConstructTree(train_data, root, 0);
	//cout << root.axis << " " << root.left->spec.species << " " << root.left->left->spec.species << endl;
	vector<vector<int>> K1_tinylist, K5_tinylist, K10_tinylist, K100_tinylist;
	TinylistInit(K1_tinylist, 1, 36);
	TinylistInit(K5_tinylist, 5, 36);
	TinylistInit(K10_tinylist, 10, 36);
	TinylistInit(K100_tinylist, 100, 36);
	TestTree(test_data, root, 1, K1_tinylist);
	TestTree(test_data, root, 5, K5_tinylist);
	TestTree(test_data, root, 10, K10_tinylist);
	TestTree(test_data, root, 100, K100_tinylist);
	EvalAccuracy(train_data, test_data, K1_tinylist);
	EvalAccuracy(train_data, test_data, K5_tinylist);
	EvalAccuracy(train_data, test_data, K10_tinylist);
	EvalAccuracy(train_data, test_data, K100_tinylist);
	system("pause");
	return 0;
}

