
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


#include "ofApp.h"

//--------------------------------------------------------------
//
void ofApp::setup() {

	ofSetBackgroundColor(ofColor::black);
	theCam = &mainCam;
	mainCam.setDistance(10);
	mainCam.setNearClip(.1);

	sideCam.setPosition(40, 0, 0);
	sideCam.lookAt(glm::vec3(0, 0, 0));
	topCam.setNearClip(.1);
	topCam.setPosition(0, 16, 0);
	topCam.lookAt(glm::vec3(0, 0, 0));
	previewCam.setPosition(renderCam.position);
	previewCam.lookAt(renderCam.aim);
	previewCam.setNearClip(.1);
	theCam = &mainCam;

	//  create a scene consisting of a ground plane and wall plane
	//
	bottom1 = new Plane(glm::vec3(0, -4, -10), glm::vec3(0, 0, 1), ofColor::darkGreen, 30, 30);
	scene.push_back(bottom1);

	bottom2 = new Plane(glm::vec3(0, -2, 0), glm::vec3(0, 1, 0), ofColor::darkRed, 30, 20);
	scene.push_back(bottom2);

	// Initialize GUI sliders
	gui.setup();  // Initialize GUI panel
	gui.setPosition(10, 10);  // Set position of GUI panel on the screen

	// Add sliders to GUI panel
	gui.add(lightIntensitySlider.setup("Light Intensity", 0, 0, 1)); // Slider for light intensity (min: 0, max: 1)
	gui.add(powerExponentSlider.setup("Power Exponent", 10, 0, 100)); // Slider for power exponent (min: 0, max: 100)
	gui.add(colorSliderR.setup("Color R", 255, 0, 255));
	gui.add(colorSliderG.setup("Color G", 255, 0, 255));
	gui.add(colorSliderB.setup("Color B", 255, 0, 255));
	gui.add(individualIntensitySlider.setup("Individual Light Intensity", 0.4, 0.1, 1));
	gui.add(togglePreview.setup("Toggle Preview", true));
	gui.add(toggleLambert.setup("Toggle Lambert", false));
	gui.add(togglePhong.setup("Toggle Phong", false));
	gui.add(toggleTextures.setup("Toggle Textures", false));

	// The following is to set up controls on the console to understand how to use the
	// program better. 
	//
	cout << "Controls:" << endl;
	cout << "1 = create sphere\n";
	cout << "2 = create light\n";
	cout << "c = stop camera movement (use to select objects)\n";
	cout << "mouse left click on object = select object; hold to drag object\n";
	cout << "selected + d = delete object\n";
	cout << "selected + x, y, or z + mouse drag = rotate\n";
	cout << "selected + t + mouse drag = scale\n";
	cout << "selected + GUI + j = change sphere color\n";
	cout << "selected + GUI + i = change light intensity\n";
	cout << "s = save current setup\n";
	cout << "l = load saved setup\n";
}

//--------------------------------------------------------------
void ofApp::exit() {
	delete bottom1;
	delete bottom2;
}

//--------------------------------------------------------------
void ofApp::update() {
	
	// create sphere
	//
	if (bCreateSphere) {
		mouseToDragPlane(ofGetMouseX(), ofGetMouseY(), pos);
		giveName = "sphere" + to_string(count);
		joint = new Joint(giveName, ofRandom(0.5f, 1), ofColor(ofRandom(0, 255), ofRandom(0, 255), ofRandom(0, 255)));
		joint->setPosition(pos);

		scene.push_back(joint);

		selected.clear();
		selected.push_back(joint);

		cout << "successful in creating " << giveName << endl;
		count++;
		bCreateSphere = false;
	}

	// create point light
	//
	if (bCreateLight) {
		mouseToDragPlane(ofGetMouseX(), ofGetMouseY(), pos);
		giveLightName = "light" + to_string(lightCount);
		light = new PointLight(giveLightName, 0.4f, ofColor::yellow);
		light->setPosition(pos);

		pointLightObjs.push_back(light);

		selected.clear();
		selected.push_back(light);

		cout << "successful in creating " << giveLightName << endl;
		lightCount++;
		bCreateLight = false;
	}
	
	for (int i = 0; i < scene.size(); i++) {
		if (objSelected() && scene[i] == selected[0] && changeColor) {
			scene[i]->diffuseColor = ofColor(colorSliderR, colorSliderG, colorSliderB);
			changeColor = false;
		}
	}

	for (int i = 0; i < pointLightObjs.size(); i++) {
		if (objSelected() && pointLightObjs[i] == selected[0] && changeIntensity) {
			pointLightObjs[i]->intensity = individualIntensitySlider;
			changeIntensity = false;
		}
	}

	// delete object
	//
	if (bDelete) {
		if (objSelected()) {
			removeObject(selected[0]);
		}
		bDelete = false;
	}
}

void ofApp::removeObject(SceneObject* obj) {
	if (selected[0] == obj) selected.clear();
	for (int i = 0; i < scene.size(); i++) {
		if (scene[i] == obj) {
			scene.erase(scene.begin() + i);
			delete obj;
			break;
		}
	}
	for (int i = 0; i < pointLightObjs.size(); i++) {
		if (pointLightObjs[i] == obj) {
			pointLightObjs.erase(pointLightObjs.begin() + i);
			delete obj;
			break;
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	theCam->begin();
	ofDisableLighting();

	//  draw the sphere objects in scene
	//
	material.begin();
	ofFill();
	for (int i = 0; i < scene.size(); i++) {
		ofSetColor(scene[i]->diffuseColor);
		scene[i]->draw();
	}

	// draws the point light spheres
	//
	for (int i = 0; i < pointLightObjs.size(); i++) {
		ofSetColor(pointLightObjs[i]->diffuseColor);
		pointLightObjs[i]->draw();
	}

	ofSetColor(ofColor::black);

	// draws the preview cam border
	//
	ofSetColor(ofColor::white);
	ofNoFill();
	renderCam.view.draw();

	// draws the image
	//
	if (togglePreview == true)
		image.draw(-renderCam.view.width() / 2, -renderCam.view.height() / 2, 5.0f, renderCam.view.width(), renderCam.view.height());

	material.end();
	theCam->end();

	if (bHide) {
		gui.draw();
	}
}

// 
// Draw an XYZ axis in RGB at transform
//
void ofApp::drawAxis(glm::mat4 m, float len) {

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(glm::vec3(m * glm::vec4(0, 0, 0, 1)), glm::vec3(m * glm::vec4(len, 0, 0, 1)));


	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(glm::vec3(m * glm::vec4(0, 0, 0, 1)), glm::vec3(m * glm::vec4(0, len, 0, 1)));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(glm::vec3(m * glm::vec4(0, 0, 0, 1)), glm::vec3(m * glm::vec4(0, 0, len, 1)));
}

// print C++ code for obj tranformation channels. (for debugging);
//
void ofApp::printChannels(SceneObject* obj) {
	cout << "position = glm::vec3(" << obj->position.x << "," << obj->position.y << "," << obj->position.z << ");" << endl;
	cout << "rotation = glm::vec3(" << obj->rotation.x << "," << obj->rotation.y << "," << obj->rotation.z << ");" << endl;
	cout << "scale = glm::vec3(" << obj->scale.x << "," << obj->scale.y << "," << obj->scale.z << ");" << endl;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

	switch (key) {
	case OF_KEY_ALT:
		bAltKeyDown = false;
		mainCam.disableMouseInput();
		break;
	case 't':
		bScale = false;
		break;
	case 'x':
		bRotateX = false;
		break;
	case 'y':
		bRotateY = false;
		break;
	case 'z':
		bRotateZ = false;
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case 'C':
	case 'c':
		if (mainCam.getMouseInputEnabled()) mainCam.disableMouseInput();
		else mainCam.enableMouseInput();
		break;
	case 'F':
	case 'b':
		break;
	case 'f':
		ofToggleFullscreen();
		break;
	case 'h':
		bHide = !bHide;
		break;
	case 'i':
		changeIntensity = true;
		break;
	case 'l':
		loadFromFile();
		break;
	case 'j':
		changeColor = true;
		break;
	case 'd':
		bDelete = true;
		break;
	case 'n':
		break;
	case 'p':
		if (objSelected()) printChannels(selected[0]);
		break;
	case 'r':
		rayTrace();
		break;
	case 's':
		cout << "Saving to file..." << endl;
		saveToFile();
		break;
	case 't':
		bScale = true;
		break;
	case 'x':
		bRotateX = true;
		break;
	case 'y':
		bRotateY = true;
		break;
	case 'z':
		bRotateZ = true;
		break;
	case '1':
		bCreateSphere = true;
		break;
	case '2':
		bCreateLight = true;
		break;
	case OF_KEY_F1:
		theCam = &mainCam;
		break;
	case OF_KEY_F2:
		theCam = &sideCam;
		break;
	case OF_KEY_F3:
		theCam = &topCam;
		break;
	case OF_KEY_F4:
		theCam = &previewCam;
	case OF_KEY_ALT:
		bAltKeyDown = true;
		if (!mainCam.getMouseInputEnabled()) mainCam.enableMouseInput();
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	if (objSelected() && bDrag) {
		glm::vec3 point;
		mouseToDragPlane(x, y, point);
		if (bRotateX) {
			selected[0]->rotation += glm::vec3((point.x - lastPoint.x) * 20.0, 0, 0);
		}
		else if (bRotateY) {
			selected[0]->rotation += glm::vec3(0, (point.x - lastPoint.x) * 20.0, 0);
		}
		else if (bRotateZ) {
			selected[0]->rotation += glm::vec3(0, 0, (point.x - lastPoint.x) * 20.0);
		}
		else if (bScale) {
			// Scale values; only radius is really relevant for spheres
			//
			selected[0]->radius += point.x - lastPoint.x;
			selected[0]->width += point.x - lastPoint.x;
			selected[0]->height += point.x - lastPoint.x;
			selected[0]->depth += point.x - lastPoint.x;
		}
		else {
			selected[0]->position += (point - lastPoint);
		}
		lastPoint = point;
	}

}

//  This projects the mouse point in screen space (x, y) to a 3D point on a plane
//  normal to the view axis of the camera passing through the point of the selected object.
//  If no object selected, the plane passing through the world origin is used.
//
bool ofApp::mouseToDragPlane(int x, int y, glm::vec3& point) {
	glm::vec3 p = theCam->screenToWorld(glm::vec3(x, y, 0));
	glm::vec3 d = p - theCam->getPosition();
	glm::vec3 dn = glm::normalize(d);

	float dist;
	glm::vec3 pos;
	if (objSelected()) {
		pos = selected[0]->position;
	}
	else pos = glm::vec3(0, 0, 0);
	if (glm::intersectRayPlane(p, dn, pos, glm::normalize(theCam->getZAxis()), dist)) {
		point = p + dn * dist;
		return true;
	}
	return false;
}

//--------------------------------------------------------------
//
// Provides functionality of single selection and if something is already selected,
// sets up state for translation/rotation of object using mouse.
//
void ofApp::mousePressed(int x, int y, int button) {

	// if we are moving the camera around, don't allow selection
	//
	if (mainCam.getMouseInputEnabled()) return;

	// clear selection list
	//
	selected.clear();

	//
	// test if something selected
	//
	vector<SceneObject*> hits;

	glm::vec3 p = theCam->screenToWorld(glm::vec3(x, y, 0));
	glm::vec3 d = p - theCam->getPosition();
	glm::vec3 dn = glm::normalize(d);

	// check for selection of scene objects
	//
	for (int i = 0; i < scene.size(); i++) {

		glm::vec3 point, norm;

		//  We hit an object
		//
		if (scene[i]->isSelectable && scene[i]->intersect(Ray(p, dn), point, norm)) {
			hits.push_back(scene[i]);
		}
	}
	for (int i = 0; i < pointLightObjs.size(); i++) {
		
		glm::vec3 point, norm;

		//  We hit an object
		//
		if (pointLightObjs[i]->isSelectable && pointLightObjs[i]->intersect(Ray(p, dn), point, norm)) {
			hits.push_back(pointLightObjs[i]);
		}
	}


	// if we selected more than one, pick nearest
	//
	SceneObject* selectedObj = NULL;
	if (hits.size() > 0) {
		selectedObj = hits[0];
		float nearestDist = std::numeric_limits<float>::infinity();
		for (int n = 0; n < hits.size(); n++) {
			float dist = glm::length(hits[n]->position - theCam->getPosition());
			if (dist < nearestDist) {
				nearestDist = dist;
				selectedObj = hits[n];
			}
		}
	}
	if (selectedObj) {
		selected.push_back(selectedObj);
		bDrag = true;
		mouseToDragPlane(x, y, lastPoint);
	}
	else {
		selected.clear();
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bDrag = false;

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

ofColor ofApp::lambert(const glm::vec3& p, const glm::vec3& norm, const ofColor& diffuse) {
	intensity = lightIntensitySlider;
	ofColor lighting = ofColor::black;

	for (auto light : pointLightObjs) {
		lightPos = glm::normalize(light->position - p);
		dotProd = glm::dot(norm, lightPos);

		vector<Ray> lightRays;
		int n = light->getRaySamples(intersectPoint, lightRays);

		// Shadows are created here
		//
		bool inShadow = false;

		for (int i = 0; i < n; i++) {
			Ray shadowRay(p + norm * 0.0001f, lightPos);
			for (auto object : scene) {
				if (object->intersect(shadowRay, shadowPoint, shadowNormal)) {
					inShadow = true;
				}
			}

			// Lambert lighting is made here
			//
			if (!inShadow) {
				lighting += diffuse * (light->intensity + intensity) * light->color * glm::max(dotProd, 0.0f);
				//lighting = closestObj->diffuseColor;
			}
			if (inShadow) {
				lighting += ofColor::black;
			}
		}
	}

	return lighting;
}

ofColor ofApp::phong(const glm::vec3& p, const glm::vec3& norm, const ofColor& diffuse, const ofColor& specular, float power) {
	intensity = lightIntensitySlider;
	power = powerExponentSlider;
	ofColor lighting = ofColor::black;

	for (auto light : pointLightObjs) {
		lightPos = glm::normalize(light->position - p);
		dotProd = glm::dot(norm, lightPos);

		vector<Ray> lightRays;
		int n = light->getRaySamples(intersectPoint, lightRays);

		// Shadows are created here
		//
		bool inShadow = false;

		for (int i = 0; i < n; i++) {
			Ray shadowRay(p + norm * 0.0001f, lightPos);
			for (auto object : scene) {
				if (object->intersect(shadowRay, shadowPoint, shadowNormal)) {
					inShadow = true;
				}
			}

			// Phong lighting is made here
			//
			if (!inShadow) {
				lighting += (diffuse * light->intensity * light->color * glm::max(dotProd, 0.0f)) +
					(specular * glm::pow(glm::max(glm::dot(glm::normalize(renderCam.view.position),
						glm::reflect(-lightPos, norm)), 0.0f), power) * light->color * (light->intensity + intensity));
				//lighting = closestObj->diffuseColor;
			}
			if (inShadow) {
				lighting += ofColor::black;
			}
		}
	}

	return lighting;
}

// Your main ray trace loop
//
void ofApp::rayTrace() {
	// Assumptions
	//
	//  we are rendering an ofImage  (part of OpenFrameworks)
	//  we have allocated an image to render  that is Nx x Ny in size
	//    see ofImage::allocate
	//
	image.allocate(imageWidth, imageHeight, OF_IMAGE_COLOR);

	ofImage imageBottom;
	imageBottom.load("floor.jpg");

	ofImage imageWall;
	imageWall.load("wall3.jpg");

	//for each j  (row)  until j = Ny
	for (int j = 0; j < imageHeight; j++) {
		//for each i  (columnn)  until i = Nx
		for (int i = 0; i < imageWidth; i++) {
			//
			//  convert each i, j to (u, v)

			// Nx  (image resolution in X)
			// Ny  (image resolution in Y)
			// See Raycaster

			//u = (i + 0.5) / Nx;
			//v = (j + 0.5) / Ny;
			u = (i + 0.5) / imageWidth;
			v = (j + 0.5) / imageHeight;

			// see Raycaster
			//
			//ray = camera.getRay(u, v)
			ray = renderCam.getRay(u, v);

			hit = false;

			//distance = infinity;   // you can initialize to a very big number OR  std::numeric_limits<float>::infinity();
			distance = std::numeric_limits<float>::infinity();

			closestObj = NULL;

			float uFloor;
			float vFloor;
			float uWall;
			float vWall;

			//for each obj in scene
			for (int i = 0; i < scene.size(); i++) {
				// determine if we hit the object and save closest obj
				// record in a variable "closestObject"
				// record closest distance

				//if (obj.intersect(ray, -> intersectPoint, -> normal))
				//	determine if object is closest

				//	if (closestObject) hit = true;
				if (scene[i]->intersect(ray, intersectPoint, normal)) {
					hit = true;
					float temp = glm::distance(ray.p, intersectPoint);
					if (temp < distance) {
						distance = temp;
						shadedPoint = intersectPoint;
						normalPoint = normal;
						closestObj = scene[i];
						uFloor = ofMap(intersectPoint.x, bottom2->position.x - bottom2->width / 2,
							bottom2->position.x + bottom2->width / 2, 0, imageBottom.getWidth());
						vFloor = ofMap(intersectPoint.z, bottom2->position.z - bottom2->height / 2,
							bottom2->position.z + bottom2->height / 2, 0, imageBottom.getHeight());

						uWall = ofMap(intersectPoint.x, bottom1->position.x - bottom1->width / 2,
							bottom1->position.x + bottom1->width / 2, 0, imageWall.getWidth());
						vWall = ofMap(intersectPoint.y, bottom1->position.y - bottom1->height / 2,
							bottom1->position.y + bottom1->height / 2, 0, imageWall.getHeight());
					}
				}
			}
			//if (hit)
				//	so something if hit
				//	obj was closest object hit
				//
				//	color = obj.diffuseColor;
				//
				//	image.setColor(i, j, color);
				//
			if (hit) {
				if (toggleLambert == true) {
					if (toggleTextures) {
						if (closestObj == scene[1]) {
							color = lambert(shadedPoint, normalPoint, imageBottom.getColor(uFloor, vFloor));
						}
						else if (closestObj == scene[0]) {
							color = lambert(shadedPoint, normalPoint, imageWall.getColor(uWall, vWall));
						}
						else {
							color = lambert(shadedPoint, normalPoint, closestObj->diffuseColor);
						}
					}
					else {
						color = lambert(shadedPoint, normalPoint, closestObj->diffuseColor);
					}
				}
				if (togglePhong == true) {
					if (toggleTextures) {
						if (closestObj == scene[1]) {
							color = phong(shadedPoint, normalPoint, imageBottom.getColor(uFloor, vFloor), closestObj->specularColor, lightIntensitySlider);
						}
						else if (closestObj == scene[0]) {
							color = phong(shadedPoint, normalPoint, imageWall.getColor(uWall, vWall), closestObj->specularColor, lightIntensitySlider);
						}
						else {
							color = phong(shadedPoint, normalPoint, closestObj->diffuseColor, closestObj->specularColor, lightIntensitySlider);
						}
					}
					else {
						color = phong(shadedPoint, normalPoint, closestObj->diffuseColor, closestObj->specularColor, lightIntensitySlider);
					}
				}
				if (toggleLambert == false && togglePhong == false) {
					if (toggleTextures) {
						if (closestObj == scene[1]) {
							color = imageBottom.getColor(uFloor, vFloor);
						}
						else if (closestObj == scene[0]) {
							color = imageWall.getColor(uWall, vWall);
						}
						else {
							color = closestObj->diffuseColor;
						}
					}
					else {
						color = closestObj->diffuseColor;
					}
				}
				image.setColor(i, j, color);
			}
			//else
			//	image.setColor(i, j, backgroundColor)
			else {
				image.setColor(i, j, ofGetBackgroundColor());
			}
		}
	}

	//--> don't forget to save image...
	//
	//	  see ofImage functions:
	//
	//	  image.load();
	//	  image.save();
	//	  image.allocate();
	image.mirror(true, false); //  is image upside down ? "flip"
	image.update();
	image.save("test.png");
}

void ofApp::saveToFile() {
	ofFile newFile("savedFile.txt", ofFile::WriteOnly);
	newFile.create();

	newFile.open("savedFile.txt", ofFile::WriteOnly);
	for (int i = 2; i < scene.size(); i++) {
		newFile << fixed << setprecision(1) << "create -sphere " << ofToString(scene.at(i)->name)
				<< " -rotate <" << ofToString(scene.at(i)->rotation)
				<< "> -scale " << ofToString(scene.at(i)->radius)
				<< " -translate <" << ofToString(scene.at(i)->getPosition()) 
				<< "> -color <" << ofToString(scene.at(i)->diffuseColor)
				<< ">" << "\n";
	}
	newFile.close();
	cout << "Successfully added objects to savedFile.txt" << endl;
}

void ofApp::loadFromFile() {
	scene.erase(scene.begin() + 2, scene.end());
	selected.clear();
	count = 0;

	ofBuffer buffer = ofBufferFromFile("savedFile.txt");

	for (auto line : buffer.getLines()) {
		istringstream iss(line);
		string letter;
		iss >> letter;

		while (iss >> letter) {

			if (letter == "create") {
				cout << "Continue ";
			}
			if (letter == "-sphere") { //find sphere name
				iss >> giveName;
				cout << "\n" << giveName << " ";
			}
			if (letter == "-rotate") {
				vector<string> fLine;
				istringstream iss(line);
				string letter;

				while (iss >> letter) {
					fLine.push_back(letter);
				}

				for (int i = 1; i < fLine.size(); i++) {
					string temp;
					for (char c : fLine[i]) {
						if (c == '<') {
							fLine[i] = temp;
							temp = "";
							continue;
						}
						else if (c == ',' || c == '>') {
							fLine[i] = temp;
							temp = "";
							break;
						}
						temp += c;
					}
				}
				cout << "rotX: " << fLine[4] << " ";
				cout << "rotY: " << fLine[5] << " ";
				cout << "rotZ " << fLine[6] << " ";
				rotX = stoi(fLine[4]);
				rotY = stoi(fLine[5]);
				rotZ = stoi(fLine[6]);
			}
			if (letter == "-scale") {
				iss >> loadScale;
				cout << "scale: " << loadScale << " ";
			}
			if (letter == "-translate") {
				vector<string> fLine;
				istringstream iss(line);
				string letter;

				while (iss >> letter) {
					fLine.push_back(letter);
				}

				for (int i = 1; i < fLine.size(); i++) {
					string temp;
					for (char c : fLine[i]) {
						if (c == '<') {
							fLine[i] = temp;
							temp = "";
							continue;
						}
						else if (c == ',' || c == '>') {
							fLine[i] = temp;
							temp = "";
							break;
						}
						temp += c;
					}
				}
				cout << "posX: " << fLine[10] << " ";
				cout << "posY: " << fLine[11] << " ";
				cout << "posZ: " << fLine[12] << " ";
				posX = stoi(fLine[10]);
				posY = stoi(fLine[11]);
				posZ = stoi(fLine[12]);
				pos = glm::vec3(posX, posY, posZ);
			}
			if (letter == "-color") {
				vector<string> fLine;
				istringstream iss(line);
				string letter;

				while (iss >> letter) {
					fLine.push_back(letter);
				}

				for (int i = 1; i < fLine.size(); i++) {
					string temp;
					for (char c : fLine[i]) {
						if (c == '<') {
							fLine[i] = temp;
							temp = "";
							continue;
						}
						else if (c == ',' || c == '>') {
							fLine[i] = temp;
							temp = "";
							break;
						}
						temp += c;
					}
				}
				cout << "colorR: " << fLine[14] << " ";
				cout << "colorG: " << fLine[15] << " ";
				cout << "colorB: " << fLine[16] << " ";
				colorR = stoi(fLine[14]);
				colorG = stoi(fLine[15]);
				colorB = stoi(fLine[16]);
			}
		}
		for (int i = 0; i < scene.size(); i++) {
			joint = new Joint(giveName, loadScale, ofColor(colorR, colorG, colorB));
			joint->rotation = glm::vec3(rotX, rotY, rotZ);
			joint->setPosition(pos);
			scene.push_back(joint);
			count++;
			break;
		}
	}
}