#include "TwoDView.h"
#include <GL/glew.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <math.h>
#include <fstream>
#include <vector>
using namespace std;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

TwoDView::TwoDView(){
}

void TwoDView::resize(int w, int h)
{
    WINDOW_WIDTH = w;
    WINDOW_HEIGHT = h;

    if(w > h){
		SMALL_EDGE = h;
	} else{
		SMALL_EDGE = w;
	}

	double padding = SMALL_EDGE * .9;								//Add a padding to outside of smalled edge.

	proj = glm::ortho(-WINDOW_WIDTH/padding,WINDOW_WIDTH/padding,-WINDOW_HEIGHT/padding,WINDOW_HEIGHT/padding);		//Make projection half of each dimension in - and + to make (0,0) center.

}

void TwoDView::initialize(string fileName, string pathFile)
{

    ShaderInfo shaders[] =
    {
        {GL_VERTEX_SHADER,"default.vert"},
        {GL_FRAGMENT_SHADER,"default.frag"},
        {GL_NONE,""}
    };
	program = createShaders(shaders);


    glUseProgram(program);

    projectionLocation = glGetUniformLocation(program,"projection");
    modelViewLocation = glGetUniformLocation(program,"modelview");

    vPositionLocation = glGetAttribLocation(program,"vPosition");
    vColorLocation = glGetAttribLocation(program,"vColor");

	parseFile(fileName);

	parsePathFile(pathFile);

	rectDrawn = false;
    
	GLfloat colors[3] = {0, 0, 0};
	numOfVertices = (row + 1) * (col + 1);
	vdata.resize(numOfVertices);
	float xCoord, yCoord;

	for(int i = 0; i <= row; i++){										//Space vertices equally.
		for(int j = 0; j <= col; j++){
			int index = i * (col + 1) + j;
			//int divider = (row >= col) ? row : col;
			//xCoord = -1 * (1 - j / (divider / 2.0f));
			//yCoord = 1 - (i / (divider / 2.0f));
			xCoord = -1 * (1 - j / (col / 2.0f));
			yCoord = 1 - (i / (row / 2.0f));
			vdata[index].position[0] = xCoord;
			vdata[index].position[1] = yCoord;
			vdata[index].position[2] = 0;
			vdata[index].position[3] = 1.0f;

			for(int k = 0; k < 3; k++){
				vdata[index].color[k] = colors[k];
			}
		}
	}
    
	for(int i = 0; i < row; i++){										//Determine wall status from each file number 
		for(int j = 0; j < col; j++){									//	(only draws top and left walls for vertices to reduce number of draws).
			bool walls[] = {false, false, false, false};
			if(mazeArr[i][j] % 16 > 7) walls[0] = true;
			if(mazeArr[i][j] % 8 > 3) walls[1] = true;
			if(i == row - 1 || j == col - 1){
				if(mazeArr[i][j] % 4 > 1) walls[2] = true;
				if(mazeArr[i][j] % 2 == 1) walls[3] = true;
			}
			pushWalls(i, j, walls);
		}
	}

	cout << "row 34: " << path[34][25].row << ", col 25: " << path[34][25].col << endl;
	cout << "row 0: " << path[0][1].row << ", col 1: " << path[0][1].col << endl;
	pushPath(start[0], start[1]);

    
    glGenVertexArrays(1,&vao);
    glBindVertexArray(vao);

	
    glGenBuffers(NumBuffers,vbo);
    glBindBuffer(GL_ARRAY_BUFFER,vbo[ArrayBuffer]);

	
    glBufferData(GL_ARRAY_BUFFER,sizeof(VAttribs)*vdata.size(),&vdata[0],GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[IndexBuffer]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices.size(), &indices[0], GL_STATIC_DRAW);

	
	
    glBindBuffer(GL_ARRAY_BUFFER,vbo[ArrayBuffer]);

    
    glVertexAttribPointer(vPositionLocation,4,GL_FLOAT,GL_FALSE,sizeof(VAttribs),BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vPositionLocation);

    glVertexAttribPointer(vColorLocation,3,GL_FLOAT,GL_FALSE,sizeof(VAttribs),BUFFER_OFFSET(4*sizeof(GLfloat)));
    glEnableVertexAttribArray(vColorLocation);

	glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);

	glUseProgram(0);
}

//Draws rectangle as mouse is pressed and dragged.
void TwoDView::dragRect(int x1, int y1, int x2, int y2){
	double x1FromOrigin, y1FromOrigin, x2FromOrigin, y2FromOrigin;
	x1FromOrigin = x1 - (WINDOW_WIDTH / (double) 2.0);						//Calculate pixel coordinates from origin center of window.
	y1FromOrigin = (WINDOW_HEIGHT / (double) 2.0) - y1;
	x2FromOrigin = x2 - (WINDOW_WIDTH / (double) 2.0);
	y2FromOrigin = (WINDOW_HEIGHT / (double) 2.0) - y2;

	GLfloat x1AsFloat, y1AsFloat, x2AsFloat, y2AsFloat;						//Changes pixel coordinates to -1 to 1 scale.
	x1AsFloat = x1FromOrigin * 2 / SMALL_EDGE / .9f;
	y1AsFloat = y1FromOrigin * 2 / SMALL_EDGE / .9f;
	x2AsFloat = x2FromOrigin * 2 / SMALL_EDGE / .9f;
	y2AsFloat = y2FromOrigin * 2 / SMALL_EDGE / .9f;
	
	double xBoundary, yBoundary;
	xBoundary = ((col - 1) / (float) col) - 1 / (float) col;
	yBoundary = ((row - 1) / (float) row) - 1 / (float) row;

																			//Check if dragging area is allowsd to draw rectangle.
	if(abs(x1AsFloat) < xBoundary && abs(x2AsFloat) < xBoundary && abs(y1AsFloat) < yBoundary && abs(y2AsFloat) < yBoundary){

		GLfloat colors[3] = {0, 0, 1};

		GLfloat vertices[][4] =
		{
			{x1AsFloat, y1AsFloat, 0, 1.0f},
			{x1AsFloat, y2AsFloat, 0, 1.0f},
			{x2AsFloat, y2AsFloat, 0, 1.0f},
			{x2AsFloat, y1AsFloat, 0, 1.0f}
		};

		for (int i = 0; i < 4; i++){
			if(!rectDrawn){
				VAttribs v;
				vdata.push_back(v);
			}
			for (int j = 0; j < 3; j++){
				vdata[i + numOfVertices].color[j] = colors[j];
			}

			for (int j = 0; j < 4; j++){
				vdata[i + numOfVertices].position[j] = vertices[i][j];
			}

		}
		if(!rectDrawn){
			indices.push_back(0 + numOfVertices);							//Add to end of indices array first time.
			indices.push_back(1 + numOfVertices);
			indices.push_back(1 + numOfVertices);
			indices.push_back(2 + numOfVertices);
			indices.push_back(2 + numOfVertices);
			indices.push_back(3 + numOfVertices);
			indices.push_back(3 + numOfVertices);
			indices.push_back(0 + numOfVertices);
		}

		glBindBuffer(GL_ARRAY_BUFFER,vbo[ArrayBuffer]);

	
		glBufferData(GL_ARRAY_BUFFER,sizeof(VAttribs)*vdata.size(),&vdata[0],GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[IndexBuffer]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices.size(), &indices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER,vbo[ArrayBuffer]);

		rectDrawn = true;

	}
}

//Draws final maze created after mouse release and outputs a text file - i.e. "NewMaze-10x10.txt".
void TwoDView::finalRect(int x1, int y1, int x2, int y2){
	
	double x1FromOrigin, y1FromOrigin, x2FromOrigin, y2FromOrigin;
	x1FromOrigin = x1 - (WINDOW_WIDTH / 2.0);
	y1FromOrigin = (WINDOW_HEIGHT / 2.0) - y1;
	x2FromOrigin = x2 - (WINDOW_WIDTH / 2.0);
	y2FromOrigin = (WINDOW_HEIGHT / 2.0) - y2;

	double x1AsFloat, y1AsFloat, x2AsFloat, y2AsFloat;
	x1AsFloat = x1FromOrigin * 2 / SMALL_EDGE / .9f;
	y1AsFloat = y1FromOrigin * 2 / SMALL_EDGE / .9f;
	x2AsFloat = x2FromOrigin * 2 / SMALL_EDGE / .9f;
	y2AsFloat = y2FromOrigin * 2 / SMALL_EDGE / .9f;

	if(rectDrawn && x2AsFloat > x1AsFloat && y2AsFloat < y1AsFloat){

		int xCoordTopLeft, yCoordTopLeft, xCoordBottomRight, yCoordBottomRight;
																					//Find cells of maze where mouse pressed and released.
		for(int i = 1; i < col - 1; i++){
			if(x1AsFloat > vdata[i].position[0] && x1AsFloat < vdata[i+1].position[0]){
				xCoordTopLeft = i;
				break;
			}
		}
		for(int i = 1; i < col - 1; i++){
			if(x2AsFloat > vdata[i].position[0] && x2AsFloat < vdata[i+1].position[0]){
				xCoordBottomRight = i;
				break;
			} else if(i == col - 2){
				xCoordBottomRight = i;
			}
		}
		for(int i = 1; i < row - 1; i++){
			if(y1AsFloat < vdata[i*(col+1)].position[1] && y1AsFloat > vdata[i*(col+1)+col+1].position[1]){
				yCoordTopLeft = i;
				break;
			}
		}
		for(int i = 1; i < row - 1; i++){
			if(y2AsFloat < vdata[i*(col+1)].position[1] && y2AsFloat > vdata[i*(col+1)+col+1].position[1]){
				yCoordBottomRight = i;
				break;
			} else if(i == row - 2){
				yCoordBottomRight = i;
			}
		}

		vector< vector<int> > newMazeArr;
		newMazeArr.resize(row);
		for(int i = 0; i < row; i++){												//Copy numbers from original maze.
			newMazeArr[i].resize(col);
		}
		for(int i = 0; i < row; i++){
			for(int j = 0; j < col; j++){
				newMazeArr[i][j] = mazeArr[i][j];
			}
		}

		for(int k = 0; k <= yCoordBottomRight - yCoordTopLeft; k++){				//Change numbers based on rectangle drawn.
			for(int m = 0; m <= xCoordBottomRight - xCoordTopLeft; m++){
				newMazeArr[yCoordTopLeft + k][xCoordTopLeft + m] = 0;
			}
		}


		//Code for empty boarder for Asg2
		for(int k = 0; k <= xCoordBottomRight - xCoordTopLeft; k++){
			if(newMazeArr[yCoordTopLeft - 1][xCoordTopLeft + k] % 2 == 1) newMazeArr[yCoordTopLeft][xCoordTopLeft + k] += 4;
			if(newMazeArr[yCoordBottomRight + 1][xCoordTopLeft + k] % 8 > 3) newMazeArr[yCoordBottomRight][xCoordTopLeft + k] += 1;
		}
		for(int k = 0; k <= yCoordBottomRight - yCoordTopLeft; k++){
			if(newMazeArr[yCoordTopLeft + k][xCoordTopLeft - 1] % 4 > 1) newMazeArr[yCoordTopLeft + k][xCoordTopLeft] += 8;
			if(newMazeArr[yCoordTopLeft + k][xCoordBottomRight + 1] % 16 > 7)newMazeArr[yCoordTopLeft + k][xCoordBottomRight] += 2;
		}


		//Code for closed boarder with 2 openings
		/*for(int k = 0; k <= xCoordBottomRight - xCoordTopLeft; k++){
			newMazeArr[yCoordTopLeft][xCoordTopLeft + k] += 4;
			newMazeArr[yCoordBottomRight][xCoordTopLeft + k] += 1;
		}
		for(int k = 0; k <= yCoordBottomRight - yCoordTopLeft; k++){
			newMazeArr[yCoordTopLeft + k][xCoordTopLeft] += 8;
			newMazeArr[yCoordTopLeft + k][xCoordBottomRight] += 2;
		}

		for(int k = 0; k <= xCoordBottomRight - xCoordTopLeft; k++){
			if(newMazeArr[yCoordTopLeft - 1][xCoordTopLeft + k] % 2 != 1){
				newMazeArr[yCoordTopLeft - 1][xCoordTopLeft + k] += 1;
			}
			if(newMazeArr[yCoordBottomRight + 1][xCoordTopLeft + k] % 8 < 4){
				newMazeArr[yCoordBottomRight + 1][xCoordTopLeft + k] += 4;
			}
		}

		for(int k = 0; k <= yCoordBottomRight - yCoordTopLeft; k++){
			if(newMazeArr[yCoordTopLeft + k][xCoordTopLeft - 1] % 4 < 2){
				newMazeArr[yCoordTopLeft + k][xCoordTopLeft - 1] += 2;
			}
			if(newMazeArr[yCoordTopLeft + k][xCoordBottomRight + 1] % 16 < 8){
				newMazeArr[yCoordTopLeft + k][xCoordBottomRight + 1] += 8;
			}
		}

		int randNumberOne = rand() % 100;												//random between 0 - 99.
		int randNumberTwo;
		bool enterExitLR;																//Determine Left/Right exit/entrance or not.
		if(randNumberOne < 50){
			enterExitLR = true;
			randNumberOne = rand() % (yCoordBottomRight - yCoordTopLeft + 1);			//Break random walls.
			randNumberTwo = rand() % (yCoordBottomRight - yCoordTopLeft + 1);
			
		} else{
			enterExitLR = false;
			randNumberOne = rand() % (xCoordBottomRight - xCoordTopLeft + 1);
			randNumberTwo = rand() % (xCoordBottomRight - xCoordTopLeft + 1);
		}

		if(enterExitLR){
			newMazeArr[yCoordTopLeft + randNumberOne][xCoordTopLeft] -= 8;				//Break walls on left and right (change 2 numbers for each wall break).
			newMazeArr[yCoordTopLeft + randNumberOne][xCoordTopLeft - 1] -= 2;
			newMazeArr[yCoordTopLeft + randNumberTwo][xCoordBottomRight] -= 2;
			newMazeArr[yCoordTopLeft + randNumberTwo][xCoordBottomRight + 1] -= 8;
		} else{
			newMazeArr[yCoordTopLeft][xCoordTopLeft + randNumberOne] -= 4;				//Break walls on top and bottom.
			newMazeArr[yCoordTopLeft - 1][xCoordTopLeft + randNumberOne] -= 1;
			newMazeArr[yCoordBottomRight][xCoordTopLeft + randNumberTwo] -= 1;
			newMazeArr[yCoordBottomRight + 1][xCoordTopLeft + randNumberTwo] -= 4;
		}*/

		ofstream drawnFile;
		stringstream ss;
		ss << "NewMaze-" << row << "x" << col << ".txt";								//File output name.
		drawnFile.open(ss.str());
		drawnFile << row << "\n";
		drawnFile << col << "\n";
		drawnFile << start[0] << " " << start[1] << " " << end[0] << " " << end[1] << "\n";
		for(int i = 0; i < row; i++){
			for( int j = 0; j < col; j++){
				drawnFile << newMazeArr[i][j] << " ";
			}
			drawnFile << "\n";
		}
		drawnFile.close();

		//initialize(ss.str());															//Re-run the program with the newly generated file.
	} else{
		rectDrawn = false;
	}

}

void TwoDView::draw()
{
	glUseProgram(program);
    
    modelView = glm::scale(glm::mat4(1.0),glm::vec3(1,1,1));

	glUniformMatrix4fv(projectionLocation,1,GL_FALSE,glm::value_ptr(proj));
	
    glUniformMatrix4fv(modelViewLocation,1,GL_FALSE,glm::value_ptr(modelView));
	
    glBindVertexArray(vao);
  
    glDrawElements(GL_LINES,indices.size(),GL_UNSIGNED_INT,BUFFER_OFFSET(0));
	

	glBindVertexArray(0);
	
    glFlush();

	glUseProgram(0);
	
}

GLuint TwoDView::createShaders(ShaderInfo *shaders)
{
    ifstream file;
    GLuint shaderProgram;
    GLint linked;

    ShaderInfo *entries = shaders;

    shaderProgram = glCreateProgram();


    while (entries->type !=GL_NONE)
    {
        file.open(entries->filename.c_str());
        GLint compiled;


        if (!file.is_open())
            return false;

        string source,line;


        getline(file,line);
        while (!file.eof())
        {
            source = source + "\n" + line;
            getline(file,line);
        }
        file.close();


        const char *codev = source.c_str();


        entries->shader = glCreateShader(entries->type);
        glShaderSource(entries->shader,1,&codev,NULL);
        glCompileShader(entries->shader);
        glGetShaderiv(entries->shader,GL_COMPILE_STATUS,&compiled);

        if (!compiled)
        {
            printShaderInfoLog(entries->shader);
            for (ShaderInfo *processed = shaders;processed->type!=GL_NONE;processed++)
            {
                glDeleteShader(processed->shader);
                processed->shader = 0;
            }
            return 0;
        }
        glAttachShader( shaderProgram, entries->shader );
        entries++;
    }

    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram,GL_LINK_STATUS,&linked);

    if (!linked)
    {
        printShaderInfoLog(entries->shader);
        for (ShaderInfo *processed = shaders;processed->type!=GL_NONE;processed++)
        {
            glDeleteShader(processed->shader);
            processed->shader = 0;
        }
        return 0;
    }


    return shaderProgram;
}

void TwoDView::printShaderInfoLog(GLuint shader)
{
    int infologLen = 0;
    int charsWritten = 0;
    GLubyte *infoLog;

    glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&infologLen);
    //	printOpenGLError();
    if (infologLen>0)
    {
        infoLog = (GLubyte *)malloc(infologLen);
        if (infoLog != NULL)
        {
            glGetShaderInfoLog(shader,infologLen,&charsWritten,(char *)infoLog);
            printf("InfoLog: %s\n\n",infoLog);
            free(infoLog);
        }

    }
//	printOpenGLError();
}

void TwoDView::getOpenGLVersion(int *major,int *minor)
{
    const char *verstr = (const char *)glGetString(GL_VERSION);
    if ((verstr == NULL) || (sscanf_s(verstr,"%d.%d",major,minor)!=2))
    {
        *major = *minor = 0;
    }
}

void TwoDView::getGLSLVersion(int *major,int *minor)
{
    int gl_major,gl_minor;

    getOpenGLVersion(&gl_major,&gl_minor);
    *major = *minor = 0;

    if (gl_major==1)
    {
        /* GL v1.x can only provide GLSL v1.00 as an extension */
        const char *extstr = (const char *)glGetString(GL_EXTENSIONS);
        if ((extstr!=NULL) && (strstr(extstr,"GL_ARB_shading_language_100")!=NULL))
        {
            *major = 1;
            *minor = 0;
        }
    }
    else if (gl_major>=2)
    {
        /* GL v2.0 and greater must parse the version string */
        const char *verstr = (const char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
        if ((verstr==NULL) || (sscanf_s(verstr,"%d.%d",major,minor) !=2))
        {
            *major = 0;
            *minor = 0;
        }
    }
}

//Read in file and store variables. Called in View.cpp at line 56.
void TwoDView::parseFile(string fileName){
	mazeArr.clear();
	indices.clear();
	ifstream mazeFile(fileName);
	stringstream ss;
	string line;
	int itr = 0;
	int count = 0;
	int temp;
	while(getline(mazeFile, line)){
		ss.str(line);
		if(itr == 0){
			ss >> row;
			mazeArr.resize(row);
			itr++;
		} else if(itr == 1){
			ss >> col;
			itr++;
		} else if(itr == 2){
			ss >> temp;
			start.push_back(temp);
			ss >> temp;
			start.push_back(temp);
			ss >> temp;
			end.push_back(temp);
			ss >> temp;
			end.push_back(temp);
			itr++;
		} else{
			for(int j = 0; j < col; j++){
				ss >> temp;
				mazeArr[count].push_back(temp);
			}
			count++;
		}
		ss.clear();
	}

	mazeFile.close();

}

void TwoDView::parsePathFile(string pathFile){
	ifstream ifs(pathFile);
	stringstream ss;
	string line;

	path.resize(row);
	for(int i = 0; i < row; i++){
		path[i].resize(col);
	}

	while(getline(ifs, line)){
		ss.str(line);
		PathCoords st, nxt;
		ss >> st.row;
		ss >> st.col;
		ss >> nxt.row;
		ss >> nxt.col;

		path[st.row][st.col] = nxt;
		ss.clear();
	}

	ifs.close();

}

//Create lines in indices array based on wall code. Called in View.cpp at line 92.
void TwoDView::pushWalls(int i, int j, bool walls[4]){
	int topLeftVert, topRightVert, bottomLeftVert, bottomRightVert;
	topLeftVert = i * (col + 1) + j;
	topRightVert = topLeftVert + 1;
	bottomLeftVert = topLeftVert + col + 1;
	bottomRightVert = topLeftVert + col + 2;

	if(walls[0]){
		indices.push_back(topLeftVert);
		indices.push_back(bottomLeftVert);
	}
	if(walls[1]){
		indices.push_back(topLeftVert);
		indices.push_back(topRightVert);
	}
	if(walls[2]){
		indices.push_back(topRightVert);
		indices.push_back(bottomRightVert);
	}
	if(walls[3]){
		indices.push_back(bottomLeftVert);
		indices.push_back(bottomRightVert);
	}
}

void TwoDView::pushPath(int i, int j){

	if(!((i == 0 && j == -1) || (i == -1 && j == 0))){
		int totalVerts = vdata.size() - 1;

		int topLeftVert, topRightVert, bottomLeftVert, bottomRightVert;
		topLeftVert = i * (col + 1) + j;
		topRightVert = topLeftVert + 1;
		bottomLeftVert = topLeftVert + col + 1;
		bottomRightVert = topLeftVert + col + 2;

		float vertX = 0, vertY = 0;

		if(start[0] == i && start[1] == j){
			if(i == 0){
				//top entrance
				vertY = vdata[topLeftVert].position[1];
				vertX = (vdata[topLeftVert].position[0] + vdata[topRightVert].position[0]) / 2.0f;

			}else if(i == row - 1){
				//bottom entrance
				vertY = vdata[bottomLeftVert].position[1];
				vertX = (vdata[bottomLeftVert].position[0] + vdata[bottomRightVert].position[0]) / 2.0f;

			}else if(j == 0){
				//left entrance
				vertY = (vdata[topLeftVert].position[1] + vdata[bottomLeftVert].position[1]) / 2.0f;
				vertX = vdata[topLeftVert].position[0];

			}else if(j == col - 1){
				//right entrance
				vertY = (vdata[topLeftVert].position[1] + vdata[bottomLeftVert].position[1]) / 2.0f;
				vertX = vdata[topRightVert].position[0];

			}

			VAttribs v;
			vdata.push_back(v);
			totalVerts = vdata.size() - 1;

			vdata[totalVerts].position[0] = vertX;
			vdata[totalVerts].position[1] = vertY;
			vdata[totalVerts].position[2] = 0;
			vdata[totalVerts].position[3] = 1.0f;

			vdata[totalVerts].color[0] = 1;
			vdata[totalVerts].color[1] = 0;
			vdata[totalVerts].color[2] = 0;

			indices.push_back(totalVerts);


		} 

		VAttribs v2;
		vdata.push_back(v2);
		totalVerts = vdata.size() - 1;

		vdata[totalVerts].position[0] = (vdata[bottomLeftVert].position[0] + vdata[bottomRightVert].position[0]) / 2.0f;
		vdata[totalVerts].position[1] = (vdata[topLeftVert].position[1] + vdata[bottomLeftVert].position[1]) / 2.0f;
		vdata[totalVerts].position[2] = 0;
		vdata[totalVerts].position[3] = 1.0f;

		vdata[totalVerts].color[0] = 1;
		vdata[totalVerts].color[1] = 0;
		vdata[totalVerts].color[2] = 0;

		indices.push_back(totalVerts);
		indices.push_back(totalVerts);

		if(i == path[i][j].row + 1){
			//top exit
			vertY = vdata[topLeftVert].position[1];
			vertX = (vdata[topLeftVert].position[0] + vdata[topRightVert].position[0]) / 2.0f;

		} else if(i == path[i][j].row - 1){
			//bottom exit
			vertY = vdata[bottomLeftVert].position[1];
			vertX = (vdata[bottomLeftVert].position[0] + vdata[bottomRightVert].position[0]) / 2.0f;

		} else if(j == path[i][j].col + 1){
			//left exit
			vertY = (vdata[topLeftVert].position[1] + vdata[bottomLeftVert].position[1]) / 2.0f;
			vertX = vdata[topLeftVert].position[0];

		} else if(j == path[i][j].col - 1){
			//right exit
			vertY = (vdata[topLeftVert].position[1] + vdata[bottomLeftVert].position[1]) / 2.0f;
			vertX = vdata[topRightVert].position[0];

		} else if((path[i][j].row == 0 && path[i][j].col == -1) || (path[i][j].row == -1 && path[i][j].col == 0)){
			if(i == 0){
				//top exit maze
				vertY = vdata[topLeftVert].position[1];
				vertX = (vdata[topLeftVert].position[0] + vdata[topRightVert].position[0]) / 2.0f;

			}else if(i == row - 1){
				//bottom exit maze
				vertY = vdata[bottomLeftVert].position[1];
				vertX = (vdata[bottomLeftVert].position[0] + vdata[bottomRightVert].position[0]) / 2.0f;

			}else if(j == 0){
				//left exit maze
				vertY = (vdata[topLeftVert].position[1] + vdata[bottomLeftVert].position[1]) / 2.0f;
				vertX = vdata[topLeftVert].position[0];

			}else if(j == col - 1){
				//right exit maze
				vertY = (vdata[topLeftVert].position[1] + vdata[bottomLeftVert].position[1]) / 2.0f;
				vertX = vdata[topRightVert].position[0];

			}
		}

		VAttribs v3;
		vdata.push_back(v3);
		totalVerts = vdata.size() - 1;

		vdata[totalVerts].position[0] = vertX;
		vdata[totalVerts].position[1] = vertY;
		vdata[totalVerts].position[2] = 0;
		vdata[totalVerts].position[3] = 1.0f;

		vdata[totalVerts].color[0] = 1;
		vdata[totalVerts].color[1] = 0;
		vdata[totalVerts].color[2] = 0;

		indices.push_back(totalVerts);
		if(!((path[i][j].row == 0 && path[i][j].col == -1) || (path[i][j].row == -1 && path[i][j].col == 0))){
			indices.push_back(totalVerts);
		}

		pushPath(path[i][j].row, path[i][j].col);
	}
}