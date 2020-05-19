#pragma once
#include "include.h"
#include<Eigen/Dense>
#include"Cloth.h"
using std::vector;
class OBBTreeNode
{
public:
	vec3 center;
	vec3 dir[3];
	float dis[3];
	bool bound[4] = {true,false,false,false};
	vector<OBBTreeNode*>child;
	vector<Particle*>node;
	OBBTreeNode();
	OBBTreeNode(Cloth *cloth);
	OBBTreeNode(Cloth *cloth,int way,int h_start,int h_end,int w_start,int w_end);
	OBBTreeNode(Particle* p1,Particle* p2,Particle* p3,Particle* p4);
	OBBTreeNode(OBBTreeNode* OBB1,OBBTreeNode* OBB2);
	static vector<vec3> OBBxyz(vector<vec3> nodelist);
	static float thick ;
	void self_collision();
	void update();
	static void collition(OBBTreeNode* N1,OBBTreeNode* N2);
	static bool intersect(OBBTreeNode* N1, OBBTreeNode* N2);
	static float reflectW;
	bool isleaf = true;
};

