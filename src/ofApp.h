
//
//  Starter file for Project 3 - Skeleton Builder
//
//  This file includes functionality that supports selection and translate/rotation
//  of scene objects using the mouse.
//
//  Modifer keys for rotatation are x, y and z keys (for each axis of rotation)
//
//  (c) Kevin M. Smith  - 24 September 2018
//

#include "ofMain.h"
#include "box.h"
#include "Primitives.h"
#include "ofxGui.h"


class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	static void drawAxis(glm::mat4 transform = glm::mat4(1.0), float len = 1.0);
	bool mouseToDragPlane(int x, int y, glm::vec3& point);
	void printChannels(SceneObject*);
	bool objSelected() { return (selected.size() ? true : false); };

	void removeObject(SceneObject* parentJoint);
	void ofApp::saveToFile();
	void ofApp::loadFromFile();

	void rayTrace();
	void drawGrid() {}

	// Lights
	//
	ofLight light1;

	// Cameras
	//
	ofEasyCam  mainCam;
	ofCamera sideCam;
	ofCamera topCam;
	ofCamera* theCam;    // set to current camera either mainCam or sideCam

	// Materials
	//
	ofMaterial material;


	// scene components
	//
	vector<SceneObject*> scene;
	vector<SceneObject*> selected;
	ofPlanePrimitive plane;

	Plane* bottom1 = NULL;
	Plane* bottom2 = NULL;

	// state
	//
	bool bDrag = false;
	bool bHide = true;
	bool bAltKeyDown = false;
	bool bRotateX = false;
	bool bRotateY = false;
	bool bRotateZ = false;
	bool bScale = false;
	glm::vec3 lastPoint;

	bool bCreateSphere = false;
	bool bDelete = false;

	bool bCreateLight = false;
	bool bDeleteLight = false;

	// for the create function
	//
	glm::vec3 pos = glm::vec3(0, 0, 0);
	glm::vec3 lightLoadPos = glm::vec3(0, 0, 0);
	string giveName = "nothing";
	string giveLightName = "nothing";
	Joint* joint = NULL;
	int count = 0;
	int lightCount = 0;

	// for the delete function
	//
	glm::vec3 newPos;
	SceneObject* newParent;

	// Values set when loading
	//
	string giveParentName = "nothing";
	float rotX, rotY, rotZ, posX, posY, posZ, loadScale, colorR, colorG, colorB;
	float lightX, lightY, lightZ;

	bool bShowImage = false;
	ofCamera previewCam;

	// set up one render camera to render image throughn
	//
	RenderCam renderCam;
	ofImage image;

	int imageWidth = 1200;
	int imageHeight = 800;

	// for rayTrace function
	//
	glm::vec3 intersectPoint;
	glm::vec3 normal;
	float u;
	float v;
	Ray ray = Ray(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0));
	bool hit;
	float distance;
	SceneObject* closestObj;
	ofColor color;
	glm::vec3 shadedPoint; // for shading
	glm::vec3 normalPoint; // for shading

	ofColor lambert(const glm::vec3& p, const glm::vec3& norm, const ofColor& diffuse);
	ofColor phong(const glm::vec3& p, const glm::vec3& norm, const ofColor& diffuse, const ofColor& specular, float power);

	// for shading
	//
	glm::vec3 lightPos;

	// for both Lambert and Phong shading
	//
	float intensity;
	float dotProd;
	glm::vec3 shadowPoint;
	glm::vec3 shadowNormal;

	// GUI stuff
	//
	ofxPanel gui;
	ofxSlider<float> lightIntensitySlider;
	ofxSlider<float> powerExponentSlider;
	ofxSlider<float> colorSliderR;
	ofxSlider<float> colorSliderG;
	ofxSlider<float> colorSliderB;
	ofxSlider<float> individualIntensitySlider;
	ofxToggle togglePreview;
	ofxToggle toggleLambert;
	ofxToggle togglePhong;
	ofxToggle toggleTextures;

	// For creating point lights
	//
	vector<PointLight*> pointLightObjs;
	PointLight* light;

	bool changeColor = false;
	bool changeIntensity = false;
};