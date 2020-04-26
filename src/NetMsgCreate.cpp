#include "NetMsgCreate.h"

#include "ManagerGLView.h"
#include "WorldContainer.h"
#include "GLView.h"
#include "Model.h"
#include "WOPhysicX.h"

using namespace Aftr;
using namespace std;

NetMsgMacroDefinition(NetMsgCreate);

NetMsgCreate::NetMsgCreate() {
	this->setCamPos(Vector(0, 0, 0));
	this->setObjPos(Vector(0, 0, 0));
	this->setCamLookDir(Vector(0, 0, 0));
	this->setRotateZ(0.0f);
	this->setActorIndex(0);
	this->setModelPath("");
}

bool NetMsgCreate::toStream(NetMessengerStreamBuffer& os) const {
	os << this->objPos.x << this->objPos.y << this->objPos.z << this->camPos.x << this->camPos.y << this->camPos.z
		<< this->camLookDir.x << this->camLookDir.y << this->camLookDir.z << this->rotateZ << this->actorIndex << this->modelPath 
		<< this->m[0] << this->m[1] << this->m[2] << this->m[3] << this->m[4] << this->m[5] << this->m[6] << this->m[7] << this->m[8] << this->m[9]
		<< this->m[10] << this->m[11] << this->m[12] << this->m[13] << this->m[14] << this->m[15];

	return true;
}

bool NetMsgCreate::fromStream(NetMessengerStreamBuffer& is) {
	is >> this->objPos.x >> this->objPos.y >> this->objPos.z >> this->camPos.x >> this->camPos.y >> this->camPos.z
		>> this->camLookDir.x >> this->camLookDir.y >> this->camLookDir.z >> this->rotateZ >> this->actorIndex >> modelPath >> this->m[0]
		>> this->m[1] >> this->m[2] >> this->m[3] >> this->m[4] >> this->m[5] >> this->m[6] >> this->m[7] >> this->m[8] >> this->m[9]
		>> this->m[10] >> this->m[11] >> this->m[12] >> this->m[13] >> this->m[14] >> this->m[15];

	return true;
}

void NetMsgCreate::onMessageArrived() {
	WorldContainer* actorList = ManagerGLView::getGLView()->getActorLst();

	if (this->modelPath != "") {
		WOPhysicX* wo = WOPhysicX::New(this->modelPath, Vector(1, 1, 1), MESH_SHADING_TYPE::mstFLAT);
		wo->setPosition(this->getObjPos());
		wo->setLabel("Created by Net Msg");
		wo->getModel()->rotateAboutGlobalZ(this->getRotateZ());

		ManagerGLView::getGLView()->getWorldContainer()->push_back(wo);
		ManagerGLView::getGLView()->getActorLst()->push_back(wo);
		ManagerGLView::getGLView()->getCamera()->setPosition(this->getCamPos());
	}

	else if (this->moveCam) {
		ManagerGLView::getGLView()->getCamera()->setPosition(this->getCamPos());
		ManagerGLView::getGLView()->getCamera()->setCameraLookDirection(this->getCamLookDir());
	}

	else if (actorList != nullptr && actorList->size() > 0) {
		actorList->at(this->actorIndex)->setPosition(this->getObjPos());
		actorList->at(this->actorIndex)->getModel()->rotateAboutGlobalZ(this->getRotateZ());
		actorList->at(this->actorIndex)->getModel()->setDisplayMatrix(this->getDisplayMatrix());
		ManagerGLView::getGLView()->getCamera()->setPosition(this->getCamPos());
		ManagerGLView::getGLView()->getCamera()->setCameraLookDirection(this->getCamLookDir());
	}

}

string NetMsgCreate::toString() const {
	stringstream ss;
	ss << NetMsg::toString();
	ss << "Payload is: Object Position: " << this->objPos << "...\nCamera Position: " << this->camPos
		<< "....\nRotateZ: " << this->rotateZ << "....\nCamera Look Direction: " << this->camLookDir << endl;

	return ss.str();
}

Vector NetMsgCreate::getObjPos() {
	return this->objPos;
}

void NetMsgCreate::setObjPos(Vector v) {
	this->objPos = v;
}


Vector NetMsgCreate::getCamPos() {
	return this->camPos;
}

void NetMsgCreate::setCamPos(Vector v) {
	this->camPos = v;
}

Vector NetMsgCreate::getCamLookDir() {
	return this->camLookDir;
}

void NetMsgCreate::setCamLookDir(Vector dir) {
	this->camLookDir = dir;
}

float NetMsgCreate::getRotateZ() {
	return this->rotateZ;
}

void NetMsgCreate::setRotateZ(float rotate) {
	this->rotateZ = rotate;
}

void NetMsgCreate::setActorIndex(int i) {
	this->actorIndex = i;
}

void NetMsgCreate::setModelPath(std::string p) {
	this->modelPath = p;
}

void NetMsgCreate::setDisplayMatrix(Mat4 m) {
	this->m = m;
}

Mat4 NetMsgCreate::getDisplayMatrix() {
	return this->m;
}

void NetMsgCreate::setMoveCam(bool b) {
	this->moveCam = b;
}