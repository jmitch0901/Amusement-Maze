#include "Scenegraph.h"
#include <stack>
#include "TransformNode.h"
#include <iostream>
#include "GroupNode.h"
using namespace std;
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

Scenegraph::Scenegraph()
{
    root = NULL;
	central_spin = ra = ba = la = fa = tn = tn2 = NULL;
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

	initAnimate();
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

void Scenegraph::initAnimate(){

	central_spin = dynamic_cast<TransformNode *>(root->getNode("animate-me-m8"));

	ra = dynamic_cast<TransformNode *>(root->getNode("right-amusement-arm"));
	ba = dynamic_cast<TransformNode *>(root->getNode("back-amusement-arm"));
	la = dynamic_cast<TransformNode *>(root->getNode("left-amusement-arm"));
	fa = dynamic_cast<TransformNode *>(root->getNode("front-amusement-arm"));

	tn2 = dynamic_cast<TransformNode *>(root->getNode("transform-movable-stem"));

	tn = dynamic_cast<TransformNode*>(root->getNode("transform-top-disk"));
}

void Scenegraph::animate(float time)
{
	
	if(tn2!=NULL){
		
		tn2->setAnimationTransform(glm::translate(glm::mat4(1.0f),glm::vec3(0,glm::sin(time)*30,0)));
	}
	
	if(tn!=NULL){

		tn->setAnimationTransform(glm::rotate(glm::mat4(1.0f),-1.0f * time,glm::vec3(0,1,0)));
	}


	float PI = 3.14159f;

	if(central_spin!=NULL){
		central_spin->setAnimationTransform(glm::rotate(glm::mat4(1.0),time, glm::vec3(0,1,0)));
	}

	if(ra!=NULL){
		ra->setAnimationTransform(glm::rotate(glm::mat4(1.0),-cos(time)*(PI / 12.0f) + (PI / 12.0f), glm::vec3(0,0,1)));
	}
	if(ba!=NULL){
		ba->setAnimationTransform(glm::rotate(glm::mat4(1.0),cos(time)*(PI / 12.0f) - (PI / 12.0f), glm::vec3(1,0,0)));
	}
	if(la!=NULL){
		la->setAnimationTransform(glm::rotate(glm::mat4(1.0),-cos(time)*(PI / 12.0f) + (PI / 12.0f), glm::vec3(0,0,-1)));
	}
	if(fa!=NULL){
		fa->setAnimationTransform(glm::rotate(glm::mat4(1.0),cos(time)*(PI / 12.0f) - (PI / 12.0f), glm::vec3(-1,0,0)));
	}
	
}

