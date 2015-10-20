#include "Scenegraph.h"
#include <stack>
#include "TransformNode.h"
#include <iostream>
using namespace std;
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

Scenegraph::Scenegraph()
{
    root = NULL;
	//time=0;
}

void Scenegraph::makeScenegraph(Node *r)
{
    if (root!=NULL)
    {
        delete root;
        root = NULL;
    }
    this->root = r;
	if (this->root!=NULL)
		this->root->setScenegraph(this);

}



Scenegraph::~Scenegraph()
{
    if (root!=NULL)
    {
        delete root;
        root = NULL;
    }
}

void Scenegraph::initShaderProgram(GLint shaderProgram)
{
   modelviewLocation = glGetUniformLocation(shaderProgram,"modelview");
   objectColorLocation = glGetUniformLocation(shaderProgram,"vColor");
}

void Scenegraph::draw(stack<glm::mat4>& modelView)
{
    if (root!=NULL)
    {
		root->draw(modelView);
    }
}

void Scenegraph::animate(float time)
{
	TransformNode *tn2 = dynamic_cast<TransformNode *>(root->getNode("transform-movable-stem"));

	if(tn2!=NULL){
		
		tn2->setAnimationTransform(glm::translate(glm::mat4(1.0f),glm::vec3(0,glm::sin(time)*30,0)));
	}

	tn2 = NULL;
	TransformNode *tn = dynamic_cast<TransformNode*>(root->getNode("transform-top-disk"));
	if(tn!=NULL){
		
		tn->setAnimationTransform(glm::rotate(glm::mat4(1.0f),-1.0f * time,glm::vec3(0,1,0)));
	}

	tn = NULL;
}

