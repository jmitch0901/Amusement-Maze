#include "Scenegraph.h"
#include <stack>
#include "TransformNode.h"
#include "GroupNode.h"
using namespace std;
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

Scenegraph::Scenegraph()
{
    root = NULL;
	//time=0;
	central_spin = ra = ba = la = fa = NULL;
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

	//central_spin = ra = ba = la = fa = NULL;
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
}

void Scenegraph::animate2(float time)
{

	float PI = 3.14159f;

	//TransformNode* central_spin = dynamic_cast<TransformNode *>(root->getNode("animate-me-m8"));

	if(central_spin!=NULL){
		central_spin->setAnimationTransform(glm::rotate(glm::mat4(1.0),time/75.0f, glm::vec3(0,1,0)));
	}

	/*TransformNode* ra = dynamic_cast<TransformNode *>(root->getNode("right-amusement-arm"));
	TransformNode* ba = dynamic_cast<TransformNode *>(root->getNode("back-amusement-arm"));
	TransformNode* la = dynamic_cast<TransformNode *>(root->getNode("left-amusement-arm"));
	TransformNode* fa = dynamic_cast<TransformNode *>(root->getNode("front-amusement-arm"));*/

	if(ra!=NULL){
		ra->setAnimationTransform(glm::rotate(glm::mat4(1.0),-cos(time/50.0f)*(PI / 12.0f) + (PI / 12.0f), glm::vec3(0,0,1)));
	}
	if(ba!=NULL){
		ba->setAnimationTransform(glm::rotate(glm::mat4(1.0),cos(time/50.0f)*(PI / 12.0f) - (PI / 12.0f), glm::vec3(1,0,0)));
	}
	if(la!=NULL){
		la->setAnimationTransform(glm::rotate(glm::mat4(1.0),-cos(time/50.0f)*(PI / 12.0f) + (PI / 12.0f), glm::vec3(0,0,-1)));
	}
	if(fa!=NULL){
		fa->setAnimationTransform(glm::rotate(glm::mat4(1.0),cos(time/50.0f)*(PI / 12.0f) - (PI / 12.0f), glm::vec3(-1,0,0)));
	}

}

