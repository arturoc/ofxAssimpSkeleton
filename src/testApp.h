#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

inline ofMatrix4x4 & toOf(aiMatrix4x4 & m){
	return *((ofMatrix4x4*)&m);
}

class ofBoundingBox{
public:
	ofBoundingBox();
	ofBoundingBox(const ofVec3f & min, const ofVec3f & max, const ofMatrix4x4 & trafo=ofMatrix4x4());

	void set(const ofVec3f & min, const ofVec3f & max, const ofMatrix4x4 & trafo=ofMatrix4x4());

	ofVec3f min;
	ofVec3f max;
	ofMesh mesh;
	ofMatrix4x4 trafo;
};

typedef map<string, aiMatrix4x4> Pose;
class RiggedModel : public ofxAssimpModelLoader {
public:
	Pose getPose(int which = 0) {
		const aiMesh* mesh = modelMeshes[which].mesh;
		int n = mesh->mNumBones;
		Pose pose;
		for(int a = 0; a < n; a++) {
			const aiBone* bone = mesh->mBones[a];
			aiNode* node = scene->mRootNode->FindNode(bone->mName);
			pose[node->mName.data] = aiMatrix4x4();
		}
		return pose;
	}
	void pose(Pose& pose, int which = 0) {
		currentPose = pose;
		const aiMesh* mesh = modelMeshes[which].mesh;
		int n = mesh->mNumBones;
		vector<aiMatrix4x4> boneMatrices(n);
		for(int a = 0; a < n; a++) {
			const aiBone* bone = mesh->mBones[a];
			aiNode* node = scene->mRootNode->FindNode(bone->mName);
			boneMatrices[a] = bone->mOffsetMatrix;
			const aiNode* tempNode = node;
			while(tempNode) {
				aiMatrix4x4 curPose = pose[tempNode->mName.data];
				boneMatrices[a] = (tempNode->mTransformation * curPose) * boneMatrices[a];
				tempNode = tempNode->mParent;
			}
			modelMeshes[which].hasChanged = true;
			modelMeshes[which].validCache = false;
		}

		modelMeshes[which].animatedPos.assign(modelMeshes[which].animatedPos.size(),0);
		if(mesh->HasNormals()){
			modelMeshes[which].animatedNorm.assign(modelMeshes[which].animatedNorm.size(),0);
		}

		// loop through all vertex weights of all bones
		for(int a = 0; a < n; a++) {
			const aiBone* bone = mesh->mBones[a];
			const aiMatrix4x4& posTrafo = boneMatrices[a];
			for( size_t b = 0; b < bone->mNumWeights; b++) {
				const aiVertexWeight& weight = bone->mWeights[b];
				size_t vertexId = weight.mVertexId;
				const aiVector3D& srcPos = mesh->mVertices[vertexId];
				modelMeshes[which].animatedPos[vertexId] += weight.mWeight * (posTrafo * srcPos);
			}
			if(mesh->HasNormals()) {
				// 3x3 matrix, contains the bone matrix without the translation, only with rotation and possibly scaling
				aiMatrix3x3 normTrafo = aiMatrix3x3( posTrafo);
				for( size_t b = 0; b < bone->mNumWeights; b++) {
					const aiVertexWeight& weight = bone->mWeights[b];
					size_t vertexId = weight.mVertexId;
					const aiVector3D& srcNorm = mesh->mNormals[vertexId];
					modelMeshes[which].animatedNorm[vertexId] += weight.mWeight * (normTrafo * srcNorm);
				}
			}
		}
		updateGLResources();
	}

	void calculateSkeleton(int which=0){
		skeleton.clear();
		const aiMesh* mesh = modelMeshes[which].mesh;
		int n = mesh->mNumBones;
		vector<aiMatrix4x4> boneMatrices(n);
		vector<aiMatrix4x4> currBoneMatrices(n);
		Pose pose = getPose();
		for(int a = 0; a < n; a++) {
			const aiBone* bone = mesh->mBones[a];
			aiNode* node = scene->mRootNode->FindNode(bone->mName);
			boneMatrices[a] = currBoneMatrices[a] = bone->mOffsetMatrix;
			const aiNode* tempNode = node;
			while(tempNode) {
				aiMatrix4x4 curPose = pose[tempNode->mName.data];
				aiMatrix4x4 curcurPose = currentPose[tempNode->mName.data];
				boneMatrices[a] = (tempNode->mTransformation * curPose) * boneMatrices[a];
				currBoneMatrices[a] = (tempNode->mTransformation * curcurPose) * currBoneMatrices[a];
				tempNode = tempNode->mParent;
			}
			ofVec3f min(9999,9999,9999),max(-9999,-9999,-9999);
			for( size_t b = 0; b < bone->mNumWeights; b++) {
				const aiVertexWeight& weight = bone->mWeights[b];
				size_t vertexId = weight.mVertexId;
				aiVector3D tmp = modelMeshes[which].mesh->mVertices[vertexId];
				//aiTransformVecByMatrix4(&tmp,&boneMatrices[a]);
				min.x = MIN(min.x,tmp.x);
				min.y = MIN(min.y,tmp.y);
				min.z = MIN(min.z,tmp.z);

				max.x = MAX(max.x,tmp.x);
				max.y = MAX(max.y,tmp.y);
				max.z = MAX(max.z,tmp.z);
			}
			skeleton.push_back(ofBoundingBox(min,max,toOf(currBoneMatrices[a])));
		}
	}

	vector<ofBoundingBox> skeleton;
	Pose currentPose;
};



class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void randomPose();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		RiggedModel model;
		ofVboMesh body;
		ofLight light;
		bool wireframe;
};
