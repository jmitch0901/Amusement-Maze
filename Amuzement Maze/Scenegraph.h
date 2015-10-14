#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

#include <map>
#include <stack>
#include <vector>
using namespace std;
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/gl.h>
#include <utils/Object.h>
#include "Node.h"
#include "TransformNode.h"

class Scenegraph
{    
    friend class LeafNode;
	friend class SceneXMLReader;
public:
    Scenegraph();
    ~Scenegraph();
    void makeScenegraph(Node *root);
    void initShaderProgram(GLint shaderProgram);
	void initAnimate();
    void draw(stack<glm::mat4>& modelView);

	void addInstance(graphics::Object *in)
	{
		if (instances.count(in->getName())<1)
			instances[in->getName()] = in;
	}

	void animate2(float t);

	graphics::Object *getInstance(string name)
	{
		/*
		int i;

		i = 0;
		while ((i<instances.size()) && (instances[i]->getName()!=name))
			i++;

		if (i<instances.size())
		{
			return instances[i];
		}
		else
		{
			return NULL;
		}
		*/

		if (instances.count(name)<1)
			return NULL;

		return instances[name];
	}

	Node * getRoot()
	{
		return root;
	}

private:
    Node *root;
	float time;
	TransformNode *central_spin, *ra, *ba, *la, *fa;
	map<string,graphics::Object *> instances;
	//vector<TransformNode*> animationNodes;
    GLint objectColorLocation,modelviewLocation;
};

#endif // SCENEGRAPH_H
