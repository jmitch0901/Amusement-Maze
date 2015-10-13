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
	Node *n = root->getNode("transform-top-disk");
	if(n!=NULL){

		TransformNode *tn = dynamic_cast<TransformNode *> (n);

		if(tn!=NULL){

			glm::mat4 transform =  glm::rotate(glm::mat4(1.0f),-time,glm::vec3(0,1,0));
			tn->setAnimationTransform(transform);

		} else {
			cout<<"Couldn't do dynamic cast on top-disk."<<endl;
		}
	} else {
		cout<<"Couldn't find top-disk transform node"<<endl;
	}

	/*Node *n2 = root->getNode("transform-movable-stem");

	if(n2!=NULL){

		TransformNode *tn = dynamic_cast<TransformNode *> (n2);

		if(tn!=NULL){

			glm::mat4 transform = glm::mat4(1.0f) * glm::translate(glm::mat4(1.0f),glm::vec3(0,glm::cos(time),0));
			tn->setAnimationTransform(transform);

		} else {

			cout<<"Couldn't do dynamic cast on movable-stem."<<endl;

		}



	} else {
		cout<<"Couldn't find movable-stem transform node"<<endl;
	}*/

}

