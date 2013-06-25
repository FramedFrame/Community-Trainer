/*
 * Written by ChiLaX - ChiLaX1@live.de
 * Revision 1.0
 */

#include "XML Parser.h"

XML::Node* XML::Parser::GetFirstNode() {
	return first_node;
}

/*
 * Receives the path of a XML (Harepacker dump) file as a string parameter 
 * and constructs a tree of XML-nodes out of the data.
 * Afterwards it is accessible via XML::Parser::GetFirstNode() and navigation via
 * XML::Node::parent and XML::Node::children.
 */
void XML::Parser::ParseFile(string path)
{
	first_node = nullptr;
	string data = "";
	char* temp = new char[255];
	ifstream ifs;
	ifs.open(path);
	while (!ifs.eof())
	{
		ifs >> temp;
		data.append(temp);
		data.append(" ");
	}
	stringstream ss(data);
	string item;
	while (getline(ss, item, '<')) 
	{
		if (item[0] != '?')
		{
			if (item == "/imgdir>")
			{
				if (!stack.empty())
				{
					Node* parent = (Node*)stack.top();
					stack.pop();
					while(!queue.empty())
					{
						queue.front()->parent = parent;
						parent->children.push_back(queue.front());
						queue.pop();
					}
					if (!stack.empty())
						stack.top()->children.push_back(parent);
				}
			}
			else
			{
				Node* n = new Node;
				n->parent = nullptr;
				stringstream ss2(item);
				string item2;
				getline(ss2, item2, ' ');
				if (item2 == "int")
					n->type = xINT;
				else if (item2 == "imgdir")
					n->type = IMGDIR;
				else if (item2 == "string")
					n->type = xSTRING;
				else if (item2 == "float")
					n->type = xFLOAT;
				else if (item2 == "canvas")
					n->type = CANVAS;
				if (n->type == IMGDIR)
				{
					getline(ss2, item2, '"');
					getline(ss2, item2, '"');
					n->name = item2;
					stack.push(n);
					if (first_node == nullptr)
						first_node = n;
				}
				else if (n->type == CANVAS)
				{
					getline(ss2, item2, '"');
					getline(ss2, item2, '"');
					n->name = item2;
					getline(ss2, item2, '"');
					getline(ss2, item2, '"');
					n->iValue = atoi(item2.c_str());
					getline(ss2, item2, '"');
					getline(ss2, item2, '"');
					n->iValue2 = atoi(item2.c_str());
				}
				else if (n->type == xINT || n->type == xSTRING || n->type == xFLOAT)
				{
					getline(ss2, item2, '"');
					getline(ss2, item2, '"');
					n->name = item2;
					getline(ss2, item2, '"');
					getline(ss2, item2, '"');
					if (n->type == xINT)
						n->iValue = atoi(item2.c_str());
					else if (n->type == xSTRING)
						n->sValue = item2;
					else if (n->type == xFLOAT)
						n->fValue = atof(item2.c_str());
					queue.push(n);
				}
			}
		}
	}
}


