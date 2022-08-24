#include "parse.h"

queue<tree_t*> split_command(string command)
{
	for(int i=0;i<command.length();i++)
	{
		if(command[i]=='<')
		{
			if(command[i+1]=='<')
			{
				if(command[i+2]!=' ')
				{
					command=command.substr(0,i+2)+" "+command.substr(i+2);
				}
			}
			else if(command[i+1]!=' ')
			{
				command=command.substr(0,i+1)+" "+command.substr(i+1);
			}
		}
		else if(command[i]=='>')
		{
			if(command[i+1]=='>')
			{
				if(command[i+2]!=' ')
				{
					command=command.substr(0,i+2)+" "+command.substr(i+2);
				}
			}
			else if(command[i+1]!=' ')
			{
				command=command.substr(0,i+1)+" "+command.substr(i+1);
			}
		}
	}

	queue<tree_t*> ret;
	string sfield;
	stringstream ss;
	ss<<command;
	while(getline(ss,sfield,' '))
	{
		tree_t* node_p=new tree_t();
		node_p->type=TREE_TOKEN;
		node_p->token=sfield;
		node_p->childc=0;
		ret.push(node_p);
	}
	return ret;
}

tree_t* parse_command(queue<tree_t*>& q)
{
	queue<tree_t*> tree_q;
	tree_t* root=new tree_t();
	bool async=false;

	while(!q.empty())
	{
		auto e=q.front();
		q.pop();
		if(e->token=="|")
		{
			tree_q.push(root);
			tree_q.push(e);
			root=new tree_t();
			root->type=TREE_BASIC;
		}
		else if(e->token==">"||e->token=="<"||e->token==">>"||e->token=="<<")
		{
			tree_q.push(root);
			tree_q.push(e);
			tree_q.push(q.front());
			q.pop();
			root=new tree_t();
			root->type=TREE_BASIC;
		}
		else if(e->token=="&")
		{
			tree_q.push(root);
			root=new tree_t();
			root->type=TREE_BASIC;
			async=true;
		}
		else
		{
			root->childc++;
			root->childv.push_back(e);
		}
	}

	if(root->childc!=1)
	{
		tree_q.push(root);
		root=new tree_t();
	}

	while(!tree_q.empty())
	{
		auto e=tree_q.front();
		tree_q.pop();
		if(e->type==TREE_BASIC)
		{
			if(!e->childv.size())continue;
			root->childc++;
			root->childv.push_back(e);
		}
		else
		{
			if(e->token=="|")
			{
				assert(!tree_q.empty());
				root->type=TREE_PIPE;
				root->childc++;
				root->childv.push_back(tree_q.front());
				tree_q.pop();
				auto tmp=root;
				root=new tree_t();
				root->childc++;
				root->childv.push_back(tmp);
			}
			else
			{
				tree_t *redir=new tree_t();
				redir->type=TREE_REDIRECT;
				redir->childc+=3;
				auto basic=root->childv[root->childv.size()-1];
				root->childv.pop_back();
				redir->childv.push_back(basic);
				redir->childv.push_back(e);
				redir->childv.push_back(tree_q.front());
				tree_q.pop();
				root->childv.push_back(redir);
			}
		}
	}

	if(async)
	{
		root->type=TREE_ASYNC;
	}
	else
	{
		if(root->childv[0]->type!=TREE_TOKEN)
		{
			auto tmp=root;
			root=root->childv[0];
			delete tmp;
			tmp=nullptr;
		}
	}
	return root;
}

void traverse(tree_t* root,int depth)
{
	if(!root) return;
	for(int i=0;i<depth;i++) cout<<" ";
	if(root->type==TREE_TOKEN)
	{
		cout<<root->token<<endl;
	}
	else if(root->type==TREE_REDIRECT)
	{
		cout<<"redirect"<<endl;
	}
	else if(root->type==TREE_ASYNC)
	{
		cout<<"back"<<endl;
	}
	else if(root->type==TREE_PIPE)
	{
		cout<<"pipe"<<endl;
	}
	else if(root->type==TREE_BASIC)
	{
		cout<<"basic"<<endl;
	}
	for(auto& c : root->childv)
	{
		traverse(c,depth+1);
	}
}
