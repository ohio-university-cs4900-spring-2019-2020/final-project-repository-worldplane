#include "WOPhysicX.h"
#include "Model.h"
#include "iostream"

using namespace Aftr;
using namespace physx;


WOPhysicX* WOPhysicX::New(const std::string& modelFileName, const Vector& scale, MESH_SHADING_TYPE shadingType) {
	WOPhysicX* wo = new WOPhysicX();
	wo->onCreate(modelFileName, scale, shadingType);

	return wo;
}

WOPhysicX::WOPhysicX()
	: IFace((WO*)this) {

}

Mat4 WOPhysicX::onCreateNVPhysXActor(PxTransform* trans) {
	PxTransform t = *trans;
	PxMat44 pm = PxMat44(t);

	Mat4 m;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m[i * 4 + j] = pm[i][j];
		}
	}
	this->getModel()->setDisplayMatrix(m);
	this->setPosition(t.p.x, t.p.y, t.p.z);

	return m;
}

void WOPhysicX::setActor(PxRigidActor* a) {
	this->actor = a;
}

PxRigidActor* WOPhysicX::getActor() {
	return this->actor;
}

void WOPhysicX::setEngine(PhysicsCreate* p) {
	this->engine = p;
}
