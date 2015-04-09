#define PRINT_PROJECTION_MATRIX 0
#define PRINT_MODELVIEW_MATRIX 0
#include "dirent.h"
#include "renderStaticObjectInRift.h"


//Globals for static Object rendering
extern GLuint shaderProgram;
extern Mat matProjection, matModelView;
extern mat4 projection, modelview, camera;
extern mat4 offsetModelview;
#define HENRY_OFFICE 1
#define DAVID_ROOM 0

extern float globalRotx;
extern float globalRoty;
extern float globalRotz;

float transx = -100;
float transy = 10;
float transz = -280;
float rotx = -230;
float roty = 0;
float rotz = 40;

double rotAngle;
vector<CSurface_F> source;
GLuint surfaceBuffers[2];
int staticObjectsKeyMode = 0;
bool drawGardenGnome = false;

int rsoCurrentTime = 0, rsoPreviousTime = 0;
float previousIntensity = 0.8;

struct scannedObject {
  float *vertexData;
  int *indexData;
  int numVertices;
  int numFaces;
  string fname;
  bool changeIntensities;
  
  float transx;
  float transy;
  float transz;
  float centroidx;
  float centroidy;
  float centroidz;
  float rotx;
  float roty;
  float rotz;
  float scalingFactor;
  float objectIntensity;

  GLuint scannedObjectBuffers[2];
 
  scannedObject();
  scannedObject(string, float, float, float, float, float, float, float);
  void readObjectFromPLY();
  void drawScannedObject();
  void keyCallback(unsigned char);
};

scannedObject scannedObjectArr[2];

scannedObject::scannedObject() {
  vertexData = NULL;
  indexData = NULL;
  numVertices = 0;
  numFaces = 0;
  changeIntensities = false;
  transx = 0;
  transy = 0;
  transz = 0;
  centroidx = 0;
  centroidy = 0;
  centroidz = 0;
  rotx = 0;
  roty = 0;
  rotz = 0;
  scalingFactor = 1.f;
  objectIntensity = 1.f;
}

scannedObject::scannedObject(string name, float f1, float f2, float f3, float f4, float f5, float f6, float f7) {
  vertexData = NULL;
  indexData = NULL;
  numVertices = 0;
  numFaces = 0;
  changeIntensities = false;
  objectIntensity = 1.f;
  centroidx = 0;
  centroidy = 0;
  centroidz = 0;
 
#if USE_RELATIVE_PATHS
  string pathDir = "../../Data/";
#else
  string pathDir = "D:/Lucas/oculusHiballDemo/Data/";
#endif
 
  fname = pathDir + name;
  cout << fname << endl;

  transx = f1;
  transy = f2;
  transz = f3;
  rotx = f4;
  roty = f5;
  rotz = f6;
  scalingFactor = f7;
}

void scannedObject::readObjectFromPLY() {
  float sumX = 0, sumY = 0, sumZ = 0;
  cout << "Loading static object" << endl;
  string line;
 
  ifstream fs(fname);
  if(fs.is_open()) {
    while(getline(fs, line)) {
      if(line.compare(0, 10, "end_header") == 0) {
	break;
      }
      if(line.compare(0, 14,"element vertex") == 0) {
	stringstream ss;
	string dummy[2];
	ss << line;
	ss >> dummy[0] >> dummy[1] >> numVertices;
	vertexData = new float[numVertices*6];
      }
      if(line.compare(0, 12,"element face") == 0) {
	stringstream ss;
	string dummy[2];
	ss << line;
	ss >> dummy[0] >> dummy[1] >> numFaces;
	indexData = new int[numFaces*3];
      }
    }

    for (int i = 0; i < numVertices; i++) {
      getline(fs, line);
      stringstream ss;
      ss << line;
      ss >> vertexData[i*6 ] 
	 >> vertexData[i*6 + 1] 
	 >> vertexData[i*6 + 2] 
	 >> vertexData[i*6 + 3] 
	 >> vertexData[i*6 + 4] 
	 >> vertexData[i*6 + 5] ;

      sumX += vertexData[i*6];
      sumY += vertexData[i*6 + 1];
      sumZ += vertexData[i*6 + 2];
    }

    for (int i = 0; i < numVertices; i++) {
      vertexData[i*6 + 3] = vertexData[i*6 + 3]/255.f; 
      vertexData[i*6 + 4] = vertexData[i*6 + 4]/255.f;
      vertexData[i*6 + 5] = vertexData[i*6 + 5]/255.f;
    }

    for(int i = 0; i < numFaces; i++) {
      getline(fs, line);
      stringstream ss;
      ss << line;
      int dummy;
      ss >> dummy
	 >> indexData[i*3 ] 
	 >> indexData[i*3 + 1] 
	 >> indexData[i*3 + 2] ;
    }
  }
  fs.close();

  centroidx = sumX/numVertices;
  centroidy = sumY/numVertices;
  centroidz = sumZ/numVertices;

  cout << "Read in " << numVertices << " vertices and " << numFaces << " faces." << endl;
  glGenBuffers(2, scannedObjectBuffers);
  glBindBuffer(GL_ARRAY_BUFFER, scannedObjectBuffers[0]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scannedObjectBuffers[1]);

  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*numVertices*6, vertexData, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*numFaces*3, indexData, GL_STATIC_DRAW);
}

void scannedObject::drawScannedObject() {
  if(changeIntensities) {
    rsoCurrentTime = glutGet(GLUT_ELAPSED_TIME);
    int timeInterval = rsoCurrentTime - rsoPreviousTime;
    if(timeInterval > 1000) {
      rsoPreviousTime = rsoCurrentTime;
      float temp = objectIntensity;
      objectIntensity += (1 - previousIntensity);
      previousIntensity = temp;
      // cout << "object intensity: " << objectIntensity << endl;
    }
  }
  
  glBindBuffer(GL_ARRAY_BUFFER, scannedObjectBuffers[0]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, scannedObjectBuffers[1]);

  int positionIndex = glGetAttribLocation(shaderProgram, "position");
  dieOnInvalidIndex(positionIndex, "position");
    
  int colorIndex = glGetAttribLocation(shaderProgram, "color");
  dieOnInvalidIndex(colorIndex, "color");

  glEnableVertexAttribArray(positionIndex);
  glVertexAttribPointer(positionIndex, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), BUFFER_OFFSET(0));
	
  glEnableVertexAttribArray(colorIndex);
  glVertexAttribPointer(colorIndex, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), BUFFER_OFFSET(3*sizeof(float)));
  
#if PRINT_PROJECTION_MATRIX
  printf("Current projection matrix /n");
  printMat4(projection);
#endif

  setUniformMat4(shaderProgram, "projection", projection);
  setUniformMat4(shaderProgram, "camera", camera);

  // offsetModelview = translation3D(vec3(transx, transy, transz)) * rotation3D(vec3(1,0,0), rotx) * rotation3D(vec3(0,1,0), roty) * rotation3D(vec3(0,0,1), rotz)*scaling3D(vec3(scalingFactor));
  // offsetModelview = translation3D(vec3(0, 0, -2))*rotation3D(vec3(1,0,0), 90);
  offsetModelview = translation3D(vec3(transx, transy, transz)) 
    * rotation3D(vec3(1,0,0), rotx) 
    * rotation3D(vec3(0,1,0), roty) 
    * rotation3D(vec3(0,0,1), rotz)
    * scaling3D(vec3(scalingFactor)) 
    * rotation3D(vec3(1,0,0), globalRotx) 
    * rotation3D(vec3(0,1,0), globalRoty) 
    * rotation3D(vec3(0,0,1), globalRotz) 
    * translation3D(vec3(-centroidx, - centroidy, -centroidz)) ;

#if PRINT_OFFSETMODELVIEW_MATRIX 
  printf("Current offsetModelView matrix /n");
  printMat4(offsetModelview);
#endif

#if PRINT_MODELVIEW_MATRIX
  printf("Current modelview matrix /n");
  printMat4(modelview);
#endif

  setUniformMat4(shaderProgram, "modelview", modelview*offsetModelview);
  // setUniformMat4(shaderProgram, "modelview", modelview*offsetModelview);
  // setUniformMat4(shaderProgram, "modelview", offsetModelview*modelview);
  setUniformFloat(shaderProgram, "intensityFactor", objectIntensity);
  // setUniformMat4(shaderProgram, "intensityfactor", scaling3D(vec3(objectIntensity)));
  glDrawElements(GL_TRIANGLES, numFaces*3, GL_UNSIGNED_INT, 0);

  glDisableVertexAttribArray(positionIndex);
  glDisableVertexAttribArray(colorIndex);
}

void drawScannedRoom(int number) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  //printf("drawing room #%i\n", number);
  CSurface_F surf = source[(int)number];

  GLuint buffers[2];
	glGenBuffers(2, buffers);

  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);

  int positionIndex = glGetAttribLocation(shaderProgram, "position");
  dieOnInvalidIndex(positionIndex, "position");
    
  int colorIndex = glGetAttribLocation(shaderProgram, "color");
  dieOnInvalidIndex(colorIndex, "color");

  glEnableVertexAttribArray(positionIndex);
  glVertexAttribPointer(positionIndex, 3, GL_FLOAT, GL_FALSE, surf.vtDim*sizeof(float), BUFFER_OFFSET(0));
	
  glEnableVertexAttribArray(colorIndex);
  glVertexAttribPointer(colorIndex, 3, GL_FLOAT, GL_FALSE, surf.vtDim*sizeof(float), BUFFER_OFFSET(sizeof(float)*(surf.vtDim-3)));
  
#if PRINT_PROJECTION_MATRIX
  printf("Current projection matrix /n");
  printMat4(projection);
#endif

  setUniformMat4(shaderProgram, "projection", projection);
  setUniformMat4(shaderProgram, "camera", camera);
	
  offsetModelview = translation3D(vec3(transx, transy, transz)) 
    * rotation3D(vec3(1,0,0), rotx) 
    * rotation3D(vec3(0,1,0), roty) 
    * rotation3D(vec3(0,0,1), rotz)
    * rotation3D(vec3(1,0,0), globalRotx) 
    * rotation3D(vec3(0,1,0), globalRoty) 
    * rotation3D(vec3(0,0,1), globalRotz);

  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*surf.vtNum*surf.vtDim, surf.vtData, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*surf.triNum*3, surf.triangles, GL_STATIC_DRAW);


#if PRINT_OFFSETMODELVIEW_MATRIX 
  printf("Current offsetModelView matrix /n");
  printMat4(offsetModelview);
#endif

#if PRINT_MODELVIEW_MATRIX
  printf("Current modelview matrix /n");
  printMat4(modelview);
#endif

  setUniformMat4(shaderProgram, "modelview", modelview*offsetModelview);
  setUniformFloat(shaderProgram, "intensityFactor", 1.f);
  // setUniformMat4(shaderProgram, "intensityfactor", scaling3D(vec3(1)));
  glDrawElements(GL_TRIANGLES, surf.triNum*3, GL_UNSIGNED_INT, 0);

#define DRAW_COORDINATE_AXES 0
#if DRAW_COORDINATE_AXES
  float axis_vertices[] = {
    0, 0, 0,   1, 1, 1,
    1, 0, 0,   1, 0, 0,
    0, 1, 0,   0, 1, 0,
    0, 0, 1,   0, 0, 1
  };
  unsigned int axis_indices[] = {0, 1, 0, 2, 0, 3};
  GLuint buffers[2];
  glGenBuffers(2, buffers);
  glVertexAttribPointer(positionIndex, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), BUFFER_OFFSET(0));
  glVertexAttribPointer(colorIndex, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), BUFFER_OFFSET(3*sizeof(float)));
  glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*24, axis_vertices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*6, axis_indices, GL_STATIC_DRAW);
  glLineWidth(3);
  setUniformMatrix(shaderProgram, modelview*scaling3D(vec3(100)), "modelview");
  glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
  glDeleteBuffers(2, buffers);
#endif
	
  glDisableVertexAttribArray(positionIndex);
  glDisableVertexAttribArray(colorIndex);
  glDeleteBuffers(2, buffers);
	
	glFlush();
	glutSwapBuffers();
}

void renderStaticObjectInRift(int number) {   
	//printf("redering #%i", number);
  switch(2){
  case 1:{
    glutReshapeWindow(800, 800);  
    drawScannedRoom(number);
    break;
  }
  case 2:{
    drawScannedRoom(number);
    break;
  }
  default: printf("Invalid option /n"); break;
  }

}

void scannedObject::keyCallback(unsigned char k) {
  if(k == 'M') {
    cout << fname <<  " selected." << endl;
    return;
  }

  switch(k) {
  case '1': transx -= 1; printf("transx = %f \n", transx); break;  
  case '2': transx += 1; printf("transx = %f \n", transx); break; 
  case '3': transy -= 1; printf("transy = %f \n", transy); break; 
  case '4': transy += 1; printf("transy = %f \n", transy); break; 
  case '5': transz -= 1; printf("transz = %f \n", transz); break; 
  case '6': transz += 1; printf("transz = %f \n", transz); break; 
  case 'q': rotx -= 1; printf("rotx = %f \n", rotx); break;  
  case 'w': rotx += 1; printf("rotx = %f \n", rotx); break; 
  case 'e': roty -= 1; printf("roty = %f \n", roty); break; 
  case 'r': roty += 1; printf("roty = %f \n", roty); break; 
  case 't': rotz -= 1; printf("rotz = %f \n", rotz); break; 
  case 'y': rotz += 1; printf("rotz = %f \n", rotz); break; 
  case '-': {scalingFactor -= 1; scalingFactor = (scalingFactor < 1)?1:scalingFactor; transy = /*transy - 1; +*/ (scalingFactor/(scalingFactor+1))*transy; printf("Scaling factor = %f \n", scalingFactor); break;} 
  case '+': {scalingFactor += 1; transy = /*transy + 1;*/ (scalingFactor/(scalingFactor-1))*transy; printf("Scaling factor = %f \n", scalingFactor); break;}
  case '[': objectIntensity -= 1; printf("Object Intensity = %f \n", objectIntensity); break; 
  case ']': objectIntensity += 1; printf("Object Intensity = %f \n", objectIntensity); break;
  default: printf("Invalid key for moving static object. \n"); break;
  }
}

void key_renderStaticObjectInRift(unsigned char k, int x, int y) {
  if(k == 'm') {
    printf("choosing renderStaticObjectInRift keymode \n");
    return;
  }

  if(k == 'M') {
    staticObjectsKeyMode = (++staticObjectsKeyMode)%3;
    if(staticObjectsKeyMode > 0){
      cout << "Choosing model: " << scannedObjectArr[staticObjectsKeyMode - 1].fname << endl << endl;
    }
    // switch(staticObjectsKeyMode) {
    // case 0: {
    //   scannedObjectArr[0].changeIntensities = false;
    //   scannedObjectArr[1].changeIntensities = false;
    //   break;
    // }
    // case 1: {
    //   scannedObjectArr[0].changeIntensities = true;
    //   scannedObjectArr[1].changeIntensities = false;
    //   break;
    // }
    // case 2: {
    //   scannedObjectArr[0].changeIntensities = false;
    //   scannedObjectArr[1].changeIntensities = true;
    //   break;
    // }
    // default: cout << "Invalid staticObjectKeyMode" << endl; break;
    // }
    return;
  }
  
  if(k == 'd') {
    drawGardenGnome = !drawGardenGnome;
    return;
  }
 
  if(k == 'D') {
    scannedObjectArr[0].transx = -205;
    scannedObjectArr[0].transy = -90;
    scannedObjectArr[0].transz = -94;
    scannedObjectArr[0].rotx = -91;
    scannedObjectArr[0].roty = -1;
    scannedObjectArr[0].rotz = 175;
    scannedObjectArr[0].scalingFactor = 200;
    return;
  }

  switch(staticObjectsKeyMode) {
  case 0:{
    switch(k) {
    case '1': transx -= 10; printf("transx = %f \n", transx); break;  
    case '2': transx += 10; printf("transx = %f \n", transx); break; 
    case '3': transy -= 10; printf("transy = %f \n", transy); break; 
    case '4': transy += 10; printf("transy = %f \n", transy); break; 
    case '5': transz -= 10; printf("transz = %f \n", transz); break; 
    case '6': transz += 10; printf("transz = %f \n", transz); break; 
    case 'q': rotx -= 10; printf("rotx = %f \n", rotx); break;  
    case 'w': rotx += 10; printf("rotx = %f \n", rotx); break; 
    case 'e': roty -= 10; printf("roty = %f \n", roty); break; 
    case 'r': roty += 10; printf("roty = %f \n", roty); break; 
    case 't': rotz -= 10; printf("rotz = %f \n", rotz); break; 
    case 'y': rotz += 10; printf("rotz = %f \n", rotz); break; 
    case 'M': cout << "Static room selected." << endl; break;
    default: printf("Invalid key for moving static room. \n"); break;
    }
    break;
  }
  case 1: {
    scannedObjectArr[0].keyCallback(k);
    break;
  }
  case 2: {
    scannedObjectArr[1].keyCallback(k);
    break;
  }
  default:{
    cout << "Invalid keymode in renderStaticObjectInRift." << endl;
    break;
  }
  }
}

void renderStaticObjectInRiftTimer(int t) {
  glutPostRedisplay();
  glutTimerFunc(30, renderStaticObjectInRiftTimer, 0);
}

void filter(std::vector<std::string>& strings, std::string pattern)
{
    auto pos = std::remove_if(std::begin(strings), std::end(strings), 
                              [&](std::string& s) { return s.find(pattern) == std::string::npos ; }) ; 
    strings.erase(pos, std::end(strings)) ;
}

void loadScannedRoom() {
//#if USE_RELATIVE_PATHS
//  //char fname[] = "D:/Lucas/oculusHiballDemo/Data/davidRoom.bin";
//  char fname[] = "D:/Lucas/oculusHiballDemo/Data/surface_all_0036.bin";
//#else
//  //char fname[] = "D:/Lucas/oculusHiballDemo/Data/davidRoom.bin";
//	char fname[] = "D:/Lucas/oculusHiballDemo/Data/surface_all_0036.bin";
//#endif

  DIR *dir;
	struct dirent *ent;
	vector<string> files;
	if ((dir = opendir ("D:/Lucas/3dDataRendering/data/seq_bingjie_sit/")) != NULL) {
	  /* print all the files and directories within directory */
	  while ((ent = readdir (dir)) != NULL) {
		//printf ("%s\n", ent->d_name);
		  files.push_back(ent->d_name);
	  }
	  closedir (dir);
	} else {
	  perror ("Could not open directory");
	}
	filter(files, "surface_all_");
	printf("Obtained file list total: %i", files.size());
	//print(files);
	for(int i = 0; i < NUM_BUFFER; i++) {
		string file = "D:/Lucas/3dDataRendering/data/seq_bingjie_sit/" + files[i];
		//std::cout << file << endl;
		CSurface_F input;
		if (!input.readFromFile(file.c_str())) {
			cout << "Could not read the surface file!" << endl;
			die();
		} else {
			source.push_back(input);
		}
	}

  /*if (!surf.readFromFile(fname)) {
    cout << "Could not read the surface file!" << endl;
    die();
  }*/
  CSurface_F surf = source[100];

  glGenBuffers(2, surfaceBuffers);
  glBindBuffer(GL_ARRAY_BUFFER, surfaceBuffers[0]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, surfaceBuffers[1]);

  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*surf.vtNum*surf.vtDim, surf.vtData, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*surf.triNum*3, surf.triangles, GL_STATIC_DRAW);
}

void initStaticObjectRenderer() {
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_MULTISAMPLE_ARB);
  glClearColor(0.0, 0.0, 0.0, 1.0); // Black

  rotAngle = 0.0;
  glViewport(0, 0, 800, 800);

  loadScannedRoom();

  glutTimerFunc(30, renderStaticObjectInRiftTimer, 0);

}

