#include "testApp.h"

aiMatrix4x4 & toAi(ofMatrix4x4 & m){
	return *((aiMatrix4x4*)&m);
}

ofMatrix4x4 randomRotation(float amt = 180) {
	ofQuaternion quat;
	ofMatrix4x4 mat;
	ofVec3f axis(ofRandomf(), ofRandomf(), ofRandomf());
	axis.normalize();
	quat.makeRotate(ofRandom(-amt, amt), axis);
	quat.get(mat);
	return mat;
}

ofBoundingBox::ofBoundingBox(){

}

ofBoundingBox::ofBoundingBox(const ofVec3f & _min, const ofVec3f & _max, const ofMatrix4x4 & _trafo){
	set(_min,_max,_trafo);
}

void ofBoundingBox::set(const ofVec3f & _min, const ofVec3f & _max, const ofMatrix4x4 & _trafo){
	min = _min;
	max = _max;
	trafo = _trafo;
	mesh.clear();
	mesh.setMode(OF_PRIMITIVE_TRIANGLES);
	mesh.addVertex(trafo*min);
	mesh.addVertex(trafo*ofVec3f(max.x,min.y,min.z));
	mesh.addVertex(trafo*ofVec3f(max.x,min.y,max.z));
	mesh.addVertex(trafo*ofVec3f(min.x,min.y,max.z));

	mesh.addVertex(trafo*ofVec3f(min.x,max.y,min.z));
	mesh.addVertex(trafo*ofVec3f(max.x,max.y,min.z));
	mesh.addVertex(trafo*max);
	mesh.addVertex(trafo*ofVec3f(min.x,max.y,max.z));

	mesh.addIndex(0);
	mesh.addIndex(1);
	mesh.addIndex(2);

	mesh.addIndex(2);
	mesh.addIndex(3);
	mesh.addIndex(0);

	mesh.addIndex(0);
	mesh.addIndex(1);
	mesh.addIndex(4);

	mesh.addIndex(4);
	mesh.addIndex(5);
	mesh.addIndex(1);

	mesh.addIndex(1);
	mesh.addIndex(2);
	mesh.addIndex(5);

	mesh.addIndex(5);
	mesh.addIndex(6);
	mesh.addIndex(2);

	mesh.addIndex(2);
	mesh.addIndex(3);
	mesh.addIndex(6);

	mesh.addIndex(6);
	mesh.addIndex(7);
	mesh.addIndex(3);

	mesh.addIndex(3);
	mesh.addIndex(0);
	mesh.addIndex(7);

	mesh.addIndex(7);
	mesh.addIndex(4);
	mesh.addIndex(0);

	mesh.addIndex(4);
	mesh.addIndex(5);
	mesh.addIndex(6);

	mesh.addIndex(6);
	mesh.addIndex(7);
	mesh.addIndex(5);

}

void testApp::randomPose() {
	Pose pose = model.getPose();
	for(Pose::iterator i = pose.begin(); i != pose.end(); i++) {
		aiMatrix4x4& cur = i->second;
		ofMatrix4x4 mat = randomRotation(45);
		cur.a1 = mat(0, 0);
		cur.a2 = mat(0, 1);
		cur.a3 = mat(0, 2);
		cur.b1 = mat(1, 0);
		cur.b2 = mat(1, 1);
		cur.b3 = mat(1, 2);
		cur.c1 = mat(2, 0);
		cur.c2 = mat(2, 1);
		cur.c3 = mat(2, 2);
	}
	model.pose(pose);
	//curRotation = randomRotation();
}

//--------------------------------------------------------------
void testApp::setup(){
	model.loadModel("humanfem2.dae");
	ofBackground(0,0,0);

	//cout << "numBones:" << model.getNumBones();

    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	glEnable(GL_DEPTH_TEST);
	/*ofMatrix4x4 matrix;
	model.*/
	randomPose();
	model.calculateSkeleton();
	body = model.getCurrentAnimatedMesh(0);
	body.clearColors();
	ofEnableLighting();
    glShadeModel(GL_SMOOTH);
    light.enable();
    //light.setSpotlight();
    ofEnableSeparateSpecularLight();

    wireframe = false;
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){

	ofTranslate(ofGetWidth()*0.5,ofGetHeight()*0.5);
	ofRotateX((float(mouseY)/float(ofGetHeight())*2.f-1.f)*180.f);
	ofRotateY((float(mouseX)/float(ofGetWidth())*2.f-1.f)*180.f);

	ofTranslate(model.getPosition());

	ofRotate(180, 0, 0, 1);
	ofTranslate(-model.getSceneCenter().x, -model.getSceneCenter().y, model.getSceneCenter().z);

	ofScale(model.getNormalizedScale() , model.getNormalizedScale(), model.getNormalizedScale());

	//ofScale(scale.x, scale.y, scale.z);

	if(wireframe){
		body.drawWireframe();
		for(int i=0;i<(int)model.skeleton.size();i++){
			model.skeleton[i].mesh.drawFaces();
		}

	}else{
		body.drawFaces();
		for(int i=0;i<(int)model.skeleton.size();i++){
			model.skeleton[i].mesh.drawWireframe();
		}
	}

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key=='w'){
		wireframe = !wireframe;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	randomPose();
	model.calculateSkeleton();
	body = model.getCurrentAnimatedMesh(0);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
