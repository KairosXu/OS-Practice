#ifndef _PARSE_H
#define _PARSE_H

#include<queue>
#include<string.h>
#include<vector>
#include<sstream>
#include<assert.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<string>

using namespace std;

enum
{
	TREE_PIPE,
	TREE_REDIRECT,
	TREE_BASIC,
	TREE_TOKEN,
	TREE_ASYNC
};

struct tree_t
{
	int type;
	string token;
	int childc;
	vector<tree_t*> childv;
	tree_t()
	{
		type=TREE_BASIC;
		childc=0;
	}
};

queue<tree_t*> split_command(string command);
tree_t* parse_command(queue<tree_t*>& q);
void traverse(tree_t* root,int depth);

#endif
