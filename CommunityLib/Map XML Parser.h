/*
 * Written by ChiLaX - ChiLaX1@live.de
 * Revision 1.0
 */

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <math.h> //for calculateDistance
#include "XML Parser.h"

using namespace std;
using namespace XML;

struct Life
	{
		string type;
		int id, x, y, fh;
	};
	struct Foothold
	{
		int id, x1, y1, x2, y2, prev, next;
	};
	struct Rope
    {
        int id, x, y1, y2, nearestFootholdTop, nearestFootholdBottom;
    };
	struct Portal
	{
		string pn, tn;
        int x, y, id, tm;
	};
	struct Map
	{
		vector<Life> lifes;
		vector<Foothold> footholds;
		vector<Rope> ropes;
		vector<Portal> portals;
		int mapId, returnMapId;
		bool isTown;
        string mapMark, mapName, streetName, regionName;
	};
	struct Street
	{
		string name;
		vector<Map> maps;
	};
	struct Region
	{
		string name;
		vector<Street> streets;
	};

class MapXMLParser : XML::Parser
{
public: 
	void ParseStringMapXMLFile(string path); //string.wz xml dumps -> Map.img.wz
	void ParseMapXMLFile(string path); //map.wz xml dumps
	vector<Region> GetRegions();
	Node* GetFirstNodeA();

private:
	void ParseTree(Node* node, Map* map); 
	void LookUpMapName(Node* node, Map* map);
	
	//Helper functions for ParseTree
	void ParseLife(Node* node, Map* map);
	void ParseFoothold(Node* node, Map* map);
	void ParseRope(Node* node, Map* map);
	void ParsePortal(Node* node, Map* map);
	//Helper variables for ParseTree
	Life* currentLife;
	Foothold* currentFoothold;
	Rope* currentRope;
	Portal* currentPortal;
	Map* currentMap;

	void UpdateRope(Rope* rope, Map* map); //calculate distance from rope to closest footholds (bottom/top)
	int calculateDistance(int x1, int y1, int x2, int y2); //helper for UpdateRope

	vector<Region> regions;

	Node* mapTree, *mapNameTree;
};