/*
 * Written by ChiLaX - ChiLaX1@live.de
 * Revision 1.0
 */

#include <string>
#include <stack>
#include <queue>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

namespace XML
{
	enum NodeType
	{
		xINT,
		xSTRING,
		xFLOAT,
		IMGDIR,
		CANVAS
	};

	struct Node
	{
		NodeType type;
		string name;
		string sValue;
		int iValue, iValue2;
		float fValue;
		vector<Node*> children;
		Node* parent;
	};

	class Parser
	{
	public:
		void ParseFile(string path);
		Node* GetFirstNode();

	private:
		stack<Node*> stack;
		queue<Node*> queue;
		Node* first_node;
	};

}