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
	time=0;
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

void Scenegraph::animate2(float time)
{

	/*GroupNode * t = (dynamic_cast<GroupNode *>(root));
	TransformNode * t2 = dynamic_cast<TransformNode *>(t->getNode("leftleg"));
	t2->setAnimationTransform(glm::rotate(glm::mat4(1.0),1.10f*time, glm::vec3(0,1,0)));*/
	
	//TransformNode* central_spin = dynamic_cast<TransformNode *>(root->getNode("animate-me-m8"));

	//TransformNode* central_spin = dynamic_cast<TransformNode *>(root->getNode("leftleg"));

	/*GroupNode * unit = dynamic_cast<GroupNode *>(root->getNode("unit-height-humanoid"));

	GroupNode * lowerbody = dynamic_cast<GroupNode *>(unit->getNode("lowerbody"));

	TransformNode * leftleg = dynamic_cast<TransformNode *>(lowerbody->getNode("leftleg"));

	leftleg->setAnimationTransform(glm::rotate(glm::mat4(1.0),1.10f*time, glm::vec3(0,1,0)));*/





	// vvvvvvvvvvvvvvvvvv---- code should work ----vvvvvvvvvvvvvvvvvvvvvvvvv






	float PI = 3.14159f;

	TransformNode* central_spin = dynamic_cast<TransformNode *>(root->getNode("animate-me-m8"));

	if(central_spin!=NULL){
		central_spin->setAnimationTransform(glm::rotate(glm::mat4(1.0),time/75.0f, glm::vec3(0,1,0)));
	}

	TransformNode* arms = dynamic_cast<TransformNode *>(root->getNode("animate-me-2"));

	if(arms!=NULL){
		arms->setAnimationTransform(glm::rotate(glm::mat4(1.0),-cos(time/50.0f)*(PI / 12.0f) + (PI / 12.0f), glm::vec3(0,0,1)));
	}

}

