/*
 * Written by ChiLaX - ChiLaX1@live.de
 * Revision 1.0
 */

#include "Map XML Parser.h"

/*
 * Traverses the tree produced by XML::Parser::ParseFile() and calls
 * helper functions when arriving at a life/foothold/ladderRope/portal node.
 * Receives pointer to a node of the tree and a pointer to the map object
 * that will be filled with the information of the tree.
 */
void MapXMLParser::ParseTree(Node* node, Map* map)
{
	if (node->name == "mapMark")
		map->mapMark = node->sValue;
	if (node->name == "returnMap")
		map->returnMapId = node->iValue;
	if (node->name == "town")
		map->isTown = (bool)node->iValue;
	if (node->type == IMGDIR)
	{
		if (node->name == "life")
			for (std::vector<Node*>::iterator iter = node->children.begin(); iter != node->children.end(); iter++)
				ParseLife(*iter, map);
		else if (node->name == "foothold")
			for (std::vector<Node*>::iterator iter = node->children.begin(); iter != node->children.end(); iter++)
				ParseFoothold(*iter, map);
		else if (node->name == "ladderRope")
			for (std::vector<Node*>::iterator iter = node->children.begin(); iter != node->children.end(); iter++)
				ParseRope(*iter, map);
		else if (node->name == "portal")
			for (std::vector<Node*>::iterator iter = node->children.begin(); iter != node->children.end(); iter++)
				ParsePortal(*iter, map);
		else
			for (std::vector<Node*>::iterator iter = node->children.begin(); iter != node->children.end(); iter++)
				ParseTree(*iter, map);
	}
	else
		for (std::vector<Node*>::iterator iter = node->children.begin(); iter != node->children.end(); iter++)
			ParseTree(*iter, map);
}

/*
 * Helper function for ParseTree. Distinguishes between different
 * attributes and passes the information to the Map object pointed to by map.
 */
void MapXMLParser::ParseLife(Node* node, Map* map)
{
	if (node->type == IMGDIR)
		for (std::vector<Node*>::iterator iter = node->children.begin(); iter != node->children.end(); iter++)
			ParseLife(*iter, map);
	if (node->name == "type")
	{
		currentLife = new Life;
		currentLife->type = node->sValue;
	}
	if (node->name == "id")
		currentLife->id = atoi(node->sValue.c_str());
	if (node->name == "x")
		currentLife->x = node->iValue;
	if (node->name == "y")
		currentLife->y = node->iValue;
	if (node->name == "fh")
		currentLife->fh = node->iValue;
	if (node->name == "f")
		map->lifes.push_back(*currentLife);
}

/*
 * Helper function for ParseTree. Distinguishes between different
 * attributes and passes the information to the Map object pointed to by map.
 */
void MapXMLParser::ParseFoothold(Node* node, Map* map)
{
	if (node->type == IMGDIR)
		for (std::vector<Node*>::iterator iter = node->children.begin(); iter != node->children.end(); iter++)
			ParseFoothold(*iter, map);
	if (node->name == "x1")
	{
		currentFoothold = new Foothold;
		currentFoothold->id = atoi(node->parent->name.c_str());
		currentFoothold->x1 = node->iValue;
	}
	if (node->name == "x2")
		currentFoothold->x2 = node->iValue;
	if (node->name == "y1")
		currentFoothold->y1 = node->iValue;
	if (node->name == "y2")
		currentFoothold->y2 = node->iValue;
	if (node->name == "prev")
		currentFoothold->prev = node->iValue;
	if (node->name == "next")
		currentFoothold->next = node->iValue;
	if (node->name == "piece")
		map->footholds.push_back(*currentFoothold);
}

/*
 * Helper function for ParseTree. Distinguishes between different
 * attributes and passes the information to the Map object pointed to by map.
 */
void MapXMLParser::ParseRope(Node* node, Map* map)
{
	if (node->type == IMGDIR)
		for (std::vector<Node*>::iterator iter = node->children.begin(); iter != node->children.end(); iter++)
			ParseRope(*iter, map);
	if (node->name == "l")
	{
		currentRope = new Rope;
		currentRope->id = atoi(node->parent->name.c_str());
	}
	if (node->name == "x")
		currentRope->x = node->iValue;
	if (node->name == "y1")
		currentRope->y1 = node->iValue;
	if (node->name == "y2")
		currentRope->y2 = node->iValue;
	if (node->name == "page")
		map->ropes.push_back(*currentRope);
}

/*
 * Helper function for ParseTree. Distinguishes between different
 * attributes and passes the information to the Map object pointed to by map.
 */
void MapXMLParser::ParsePortal(Node* node, Map* map)
{
	if (node->type == IMGDIR)
		for (std::vector<Node*>::iterator iter = node->children.begin(); iter != node->children.end(); iter++)
			ParsePortal(*iter, map);
	if (node->name == "pn")
	{
		currentPortal = new Portal;
		currentPortal->pn = node->sValue;
		currentPortal->id = atoi(node->parent->name.c_str());
	}
	if (node->name == "x")
		currentPortal->x = node->iValue;
	if (node->name == "y")
		currentPortal->y = node->iValue;
	if (node->name == "tm")
		currentPortal->tm = node->iValue;
	if (node->name == "tn")
	{
		currentPortal->tn = node->sValue;
		map->portals.push_back(*currentPortal);
	}
}

/*
 * Parses the XML files produced by Harepacker when dumping a MapleStory Map.wz file.
 * Creates a map object for each file, fills it with data and then passes it to the
 * corresponding Street/Region object.
 */
void MapXMLParser::ParseMapXMLFile(string path) 
{
	ParseFile(path);
	Map map;
	mapTree = GetFirstNode();
	map.mapId = atoi(mapTree->name.erase(mapTree->name.length()-4, string::npos).c_str()); //get this manually, therefore ignore any "IMGDIR" afterwards
	ParseTree(mapTree, &map);
	for (std::vector<Rope>::iterator iter = map.ropes.begin(); iter != map.ropes.end(); iter++)
		UpdateRope(&(*iter), &map);
	LookUpMapName(mapNameTree, &map);
}

/*
 * Receives a pointer to the tree produced by MapXMLParser::ParseStringMapXMLFile
 * and a pointer to the Map object that requires name information.
 * Traverses the tree and looks for the street/region name. Afterwards checks
 * wether a street with the same name exists, if not then create a new Street object with
 * this name and add it to the corresponding region. If there is one, add it to the vector<Map> Street::maps.
 */
void MapXMLParser::LookUpMapName(Node* node, Map* map) {
	if (node->parent != nullptr)
	{
		if (atoi(node->parent->name.c_str()) == map->mapId)
		{
			if (node->name == "streetName")
				map->streetName = node->sValue;
			else if (node->name == "mapName")
			{
				map->mapName = node->sValue;
				map->regionName = node->parent->parent->name;
				for (std::vector<Region>::iterator iter = regions.begin(); iter != regions.end(); iter++)
				{
					if (map->regionName == (*iter).name)
					{
						bool found = false;
						for (std::vector<Street>::iterator iterr = (*iter).streets.begin(); iterr != (*iter).streets.end(); iterr++)
						{
							if (map->streetName == (*iterr).name)
							{
								(*iterr).maps.push_back(*map);
								found = true;
								break;
							}
						}
						if (!found)
						{
							Street s;
							s.name = map->streetName;
							s.maps.push_back(*map);
							(*iter).streets.push_back(s);
						}
						break;
					}
				}
			}
		}
		else
			for (std::vector<Node*>::iterator iter = node->children.begin(); iter != node->children.end(); iter++)
			{
				(*iter)->parent = node;
				LookUpMapName(*iter, map);
			}
	}
	else
		for (std::vector<Node*>::iterator iter = node->children.begin(); iter != node->children.end(); iter++)
		{
			(*iter)->parent = node;
			LookUpMapName(*iter, map);
		}
}

/*
 * Calculate the distance between 2 points.
 */
int MapXMLParser::calculateDistance(int x1, int y1, int x2, int y2)
{
	return (int)sqrt(pow((x2-x1),2) + pow((y2-y1),2));
}

/*
 * Given a pointer to a rope and a pointer to the map that it is in, calculates
 * the minimum distance from any foothold to the rope's top coordinate and bottom coordinate.
 */
void MapXMLParser::UpdateRope(Rope* rope, Map* map)
{
	int nearestFootholdTopId, nearestFootholdBottomId, minimumDistanceTop = -1, minimumDistanceBottom = -1, distanceTop, distanceBottom;
	for (std::vector<Foothold>::iterator iter = map->footholds.begin(); iter != map->footholds.end(); iter++)
	{
		distanceTop = calculateDistance(rope->x, rope->y1, (*iter).x1, (*iter).y1);
		distanceTop = min(distanceTop, calculateDistance(rope->x, rope->y1, (*iter).x1, (*iter).y2));
		distanceTop = min(distanceTop, calculateDistance(rope->x, rope->y1, (*iter).x2, (*iter).y1));
		distanceTop = min(distanceTop, calculateDistance(rope->x, rope->y1, (*iter).x2, (*iter).y2));
		distanceBottom = calculateDistance(rope->x, rope->y2, (*iter).x1, (*iter).y1);
		distanceBottom = min(distanceTop, calculateDistance(rope->x, rope->y2, (*iter).x1, (*iter).y2));
		distanceBottom = min(distanceTop, calculateDistance(rope->x, rope->y2, (*iter).x2, (*iter).y1));
		distanceBottom = min(distanceTop, calculateDistance(rope->x, rope->y2, (*iter).x2, (*iter).y2));

		if (minimumDistanceTop == -1)
		{
			minimumDistanceTop = distanceTop;
			nearestFootholdTopId = (*iter).id;
		}
		else if (minimumDistanceTop > distanceTop)
		{
			minimumDistanceTop = distanceTop;
			nearestFootholdTopId = (*iter).id;
		}
		if (minimumDistanceBottom == -1)
			minimumDistanceBottom = distanceBottom;
		else if (minimumDistanceBottom > distanceBottom)
		{
			minimumDistanceBottom = distanceBottom;
			nearestFootholdBottomId = (*iter).id;
		}
	}
	rope->nearestFootholdBottom = nearestFootholdBottomId;
	rope->nearestFootholdTop = nearestFootholdTopId;
}

/*
 * Parse the Map.img.xml file produced by Harepacker when dumping a MapleStory String.wz file.
 * Also creates a Region object for any top-level element in the file and adds it to the
 * vector<Region> MapXMLParser::regions.
 */
void MapXMLParser::ParseStringMapXMLFile(string path) 
{
	ParseFile(path);
	mapNameTree = GetFirstNode();
	for (std::vector<Node*>::iterator iter = mapNameTree->children.begin(); iter != mapNameTree->children.end(); iter++)
	{
		Region r;
		r.name = (*iter)->name;
		regions.push_back(r);
	}
}

vector<Region> MapXMLParser::GetRegions()
{
	return this->regions;
}

/*
 * Calls Parser::GetFirstNode().
 */
Node* MapXMLParser::GetFirstNodeA()
{
	return Parser::GetFirstNode();
}