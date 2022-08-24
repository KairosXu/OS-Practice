#include<stdlib.h>
#include<stdio.h>
#include "parse.h"
#include "exec.h"

void read_and_execute()
{
	string command;
	getline(cin,command);
	auto q=split_command(command);
	auto root=parse_command(q);
	traverse(root,0);
	tree_execute_wrapper(root);
}

int main(int argc,char *argv[])
{
	while(true)
		read_and_execute();
	return 0;
}
