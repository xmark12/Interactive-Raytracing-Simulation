//
//  Primitives.h - Simple 3D Primitives with with Hierarchical Transformations
//
//  
//  (c) Kevin M. Smith  - 24 September 2018
//
#pragma once

#include "ofMain.h"
#include "box.h"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/intersect.hpp"


//  General Purpose Ray class 
//
class Ray {
public:
	Ray(glm::vec3 p, glm::vec3 d) { this->p = p; this->d = d; }
	void draw(float t) { ofDrawLine(p, p + t * d); }

	glm::vec3 evalPoint(float t) {
		return (p + t * d);
	}

	glm::vec3 p, d;
};

//  Base class for any renderable object in the scene
//
class SceneObject {
public:
	virtual void draw() = 0;    // pure virtual funcs - must be overloaded
	virtual bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal) { return false; }

	// commonly used transformations
	//
	glm::mat4 getRotateMatrix() {
		return (glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z)));   // yaw, pitch, roll 
	}
	glm::mat4 getTranslateMatrix() {
		return (glm::translate(glm::mat4(1.0), glm::vec3(position.x, position.y, position.z)));
	}
	glm::mat4 getScaleMatrix() {
		return (glm::scale(glm::mat4(1.0), glm::vec3(scale.x, scale.y, scale.z)));
	}


	glm::mat4 getLocalMatrix() {

		// get the local transformations + pivot
		//
		glm::mat4 scale = getScaleMatrix();
		glm::mat4 rotate = getRotateMatrix();
		glm::mat4 trans = getTranslateMatrix();

		// handle pivot point  (rotate around a point that is not the object's center)
		//
		glm::mat4 pre = glm::translate(glm::mat4(1.0), glm::vec3(-pivot.x, -pivot.y, -pivot.z));
		glm::mat4 post = glm::translate(glm::mat4(1.0), glm::vec3(pivot.x, pivot.y, pivot.z));



		return (trans * post * rotate * pre * scale);

	}

	glm::mat4 getMatrix() {

		// if we have a parent (we are not the root),
		// concatenate parent's transform (this is recursive)
		// 
		if (parent) {
			glm::mat4 M = parent->getMatrix();
			return (M * getLocalMatrix());
		}
		else return getLocalMatrix();  // priority order is SRT
	}

	// get current Position in World Space
	//
	glm::vec3 getPosition() {
		return (getMatrix() * glm::vec4(0.0, 0.0, 0.0, 1.0));
	}

	// set position (pos is in world space)
	//
	void setPosition(glm::vec3 pos) {
		position = glm::inverse(getMatrix()) * glm::vec4(pos, 1.0);
	}

	// return a rotation  matrix that rotates one vector to another
	//
	glm::mat4 rotateToVector(glm::vec3 v1, glm::vec3 v2);

	//  Hierarchy 
	//
	void addChild(SceneObject* child) {
		childList.push_back(child);
		child->parent = this;
	}

	SceneObject* parent = NULL;        // if parent = NULL, then this obj is the ROOT
	vector<SceneObject*> childList;

	// position/orientation 
	//
	glm::vec3 position = glm::vec3(0, 0, 0);   // translate
	glm::vec3 rotation = glm::vec3(0, 0, 0);   // rotate
	glm::vec3 scale = glm::vec3(1, 1, 1);      // scale
	float radius = 1.0;
	float width = 1.0;
	float height = 1.0;
	float depth = 1.0;

	// rotate pivot
	//
	glm::vec3 pivot = glm::vec3(0, 0, 0);

	// material properties (we will ultimately replace this with a Material class - TBD)
	//
	ofColor diffuseColor = ofColor::grey;    // default colors - can be changed.
	ofColor specularColor = ofColor::lightGray;

	// UI parameters
	//
	bool isSelectable = true;
	string name = "SceneObject";
};

class Cone : public SceneObject {
public:
	Cone(ofColor color = ofColor::blue) {
		diffuseColor = color;
	}
	Cone(glm::vec3 tran, glm::vec3 rot, glm::vec3 sc, ofColor color = ofColor::blue) {
		position = tran;
		rotation = rot;
		scale = sc;
		diffuseColor = color;
	}
	void draw();
	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal);

	void setRadius(float rad) {
		radius = rad;
	}
	void setHeight(float hei) {
		height = hei;
	}
};

class Cube : public SceneObject {
public:
	Cube(ofColor color = ofColor::blue) {
		diffuseColor = color;
	}
	Cube(glm::vec3 tran, glm::vec3 rot, glm::vec3 sc, ofColor color = ofColor::blue) {
		position = tran;
		rotation = rot;
		scale = sc;
		diffuseColor = color;
	}
	void draw();
	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal);
};

//  General purpose sphere  (assume parametric)
//
class Sphere : public SceneObject {
public:
	Sphere(glm::vec3 p, float r, ofColor diffuse = ofColor::lightGray) { position = p; radius = r; diffuseColor = diffuse; }
	Sphere() {}
	~Sphere() { cout << "in Sphere destructor (~Sphere)" << endl; }
	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal);
	void draw();
};

// Joint class repurposed from Project 3 to create spheres
//
class Joint : public Sphere {
public:
	// Constructor sets sphere's name, radius (which is used for scaling), and its color
	//
	Joint(string n, float r, ofColor diffuse) {
		this->name = n;
		this->radius = r;
		this->diffuseColor = diffuse;
	}
	~Joint() { cout << "in Joint destructor (~Joint)" << endl; } // Used when destroying the sphere
	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal); // Used for when ray intersects sphere
	void draw(); // Draws the sphere when called
};

class Mesh : public SceneObject {
	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal) { return false; }
	void draw() { }
};


// General purpose plane, taken from Project 2 and modified with Project 3 values
//
class Plane : public SceneObject {
public:
	Plane(glm::vec3 p, glm::vec3 n, ofColor diffuse = ofColor::green, float w =
		20, float h = 20) {
		position = p; normal = n;
		width = w;
		height = h;
		diffuseColor = diffuse;
		if (normal == glm::vec3(0, 1, 0))
			plane.rotateDeg(-90, 1, 0, 0);
		else if (normal == glm::vec3(0, -1, 0))
			plane.rotateDeg(90, 1, 0, 0);
		else if (normal == glm::vec3(1, 0, 0))
			plane.rotateDeg(90, 0, 1, 0);
		else if (normal == glm::vec3(-1, 0, 0))
			plane.rotateDeg(-90, 0, 1, 0);
		isSelectable = false;
	}
	Plane() {
		isSelectable = false;
		normal = glm::vec3(0, 1, 0);
		plane.rotateDeg(90, 1, 0, 0);
	}
	~Plane() { cout << "in Plane destructor (~Plane)" << endl; }
	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal);
	float sdf(const glm::vec3& p);
	glm::vec3 getNormal(const glm::vec3& p) { return this->normal; }
	void draw() {
		plane.setPosition(position);
		plane.setWidth(width);
		plane.setHeight(height);
		plane.setResolution(4, 4);
		// plane.drawWireframe();
		//plane.draw();

		material.begin();
		material.setDiffuseColor(diffuseColor);
		plane.drawFaces();
		material.end();
	}
	ofPlanePrimitive plane;
	ofMaterial material;
	glm::vec3 normal;
	float width = 20;
	float height = 20;
};

// view plane for render camera
// 
class  ViewPlane : public Plane {
public:
	ViewPlane(glm::vec2 p0, glm::vec2 p1) { min = p0; max = p1; }

	ViewPlane() {                         // create reasonable defaults (6x4 aspect)
		min = glm::vec2(-3, -2);
		max = glm::vec2(3, 2);
		position = glm::vec3(0, 0, 5);
		normal = glm::vec3(0, 0, 1);      // viewplane currently limited to Z axis orientation
	}

	void setSize(glm::vec2 min, glm::vec2 max) { this->min = min; this->max = max; }
	float getAspect() { return width() / height(); }

	glm::vec3 toWorld(float u, float v);   //   (u, v) --> (x, y, z) [ world space ]

	void draw() {
		//glm::mat4 m = getMatrix();

		//   push the current stack matrix and multiply by this object's
		//   matrix. now all vertices dran will be transformed by this matrix
		//
		//ofPushMatrix();
		//ofMultMatrix(m);
		ofDrawRectangle(glm::vec3(min.x, min.y, position.z), width(), height());
		//ofPopMatrix();
	}


	float width() {
		return (max.x - min.x);
	}
	float height() {
		return (max.y - min.y);
	}

	// some convenience methods for returning the corners
	//
	glm::vec2 topLeft() { return glm::vec2(min.x, max.y); }
	glm::vec2 topRight() { return max; }
	glm::vec2 bottomLeft() { return min; }
	glm::vec2 bottomRight() { return glm::vec2(max.x, min.y); }

	//  To define an infinite plane, we just need a point and normal.
	//  The ViewPlane is a finite plane so we need to define the boundaries.
	//  We will define this in terms of min, max  in 2D.  
	//  (in local 2D space of the plane)
	//  ultimately, will want to locate the ViewPlane with RenderCam anywhere
	//  in the scene, so it is easier to define the View rectangle in a local'
	//  coordinate system.
	//
	glm::vec2 min, max;

	/*
	glm::vec3 toWorld(float u, float v);
	ofPlanePrimitive plane;
	glm::vec3 n;
	glm::vec3 p = glm::vec3(0, 0, 0);
	float w = 1200 / 100.;
	float h = 800 / 100.;
*/
};


//  render camera  - currently must be z axis aligned
//
class RenderCam : public SceneObject {
public:
	RenderCam() {
		position = glm::vec3(0, 0, 10);
		aim = glm::vec3(0, 0, -1);
	}
	Ray getRay(float u, float v);
	void draw() { 
		glm::mat4 m = getMatrix();

		//   push the current stack matrix and multiply by this object's
		//   matrix. now all vertices dran will be transformed by this matrix
		//
		ofPushMatrix();
		ofMultMatrix(m);
		ofDrawBox(position, 1.0);
		ofPopMatrix();
	};
	void drawFrustum();

	glm::vec3 aim;
	ViewPlane view;          // The camera viewplane, this is the view that we will render 
};

// Light Class repurposed from Project 2 to extend the Point Light Class, seen below
//
class Light : public Sphere {
public:
	Light(glm::vec3 p, float i, ofColor diffuse) {
		position = p;
		intensity = i;
		diffuseColor = diffuse;
		radius = 0.1f;
		isSelectable = true;
	}
	Light() {
		position = glm::vec3(0, 0, 0);
		radius = 0.1f;
		intensity = 0;
		isSelectable = true;
	}

	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal) {
		return (glm::intersectRaySphere(ray.p, ray.d, position, radius, point, normal));
	}

	virtual int getRaySamples(glm::vec3 p, std::vector<Ray>& samples) {
		return 1;
	}

	ofColor color = ofColor::white;
	float intensity;
};

// Point Light Class, which is used to create light spheres
//
class PointLight : public Light {
public:
	PointLight(string n, float i, ofColor diffuse) {
		this->name = n;
		//position = p;
		intensity = i;
		diffuseColor = diffuse;
		radius = 0.1f;
		isSelectable = true;
	}
	PointLight() {
		position = glm::vec3(0, 0, 0);
		radius = 0.1f;
		intensity = 0;
		isSelectable = true;
	}

	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal) {
		return (glm::intersectRaySphere(ray.p, ray.d, position, radius, point, normal));
	}
	
	int getRaySamples(glm::vec3 p, std::vector<Ray>& samples) {
		int numSamples = 1;

		samples.clear();

		for (int i = 0; i < numSamples; i++) {
			Ray sampleRay(p + (glm::normalize(position - p) * 0.001f), glm::normalize(position - p));
			samples.push_back(sampleRay);
		}

		return numSamples;
	}
};