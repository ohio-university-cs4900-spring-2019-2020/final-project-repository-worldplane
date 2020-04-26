#include "PhysicsCreate.h"
#include "iostream"

using namespace Aftr;
using namespace physx;
using namespace std;

PhysicsCreate::PhysicsCreate() {
	this->foundation = PxCreateFoundation(PX_PHYSICS_VERSION, this->allocator, this->err);
	this->pvd = PxCreatePvd(*this->foundation);
	this->physics = PxCreatePhysics(PX_PHYSICS_VERSION, *this->foundation, PxTolerancesScale(), true, this->pvd);
	this->material = this->physics->createMaterial(0.5f, 0.5f, 0.6f);

	PxSceneDesc s(this->physics->getTolerancesScale());
	s.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	s.gravity = PxVec3(0.0f, 0.0f, 9.8f);
	s.filterShader = PxDefaultSimulationFilterShader ;
	this->scene = this->physics->createScene(s);

	if (this->scene == nullptr) {
		cout << "---!!Error in creating scene!!---" << endl;
	}
	else {
		//set flag eENABLE_ACTIVE_ACTORS from false to true, in order to call getActiveActors()
		this->scene->setFlag(PxSceneFlag::eENABLE_ACTIVE_ACTORS, true);
	}

	this->mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, physx::PxCookingParams(physx::PxTolerancesScale()));
	if (!this->mCooking)
	{
		std::cout << "Cooking error" << std::endl;
		std::cin.get();
	}
}

void PhysicsCreate::shutdown() {
	if (this->foundation != nullptr) {
		this->foundation->release();
	}

	if (this->physics != nullptr) {
		this->physics->release();
	}

	if (this->scene != nullptr) {
		this->scene->release();
	}

	if (this->pvd != nullptr) {
		this->pvd->release();
	}

	if (this->material != nullptr) {
		this->material->release();
	}
}

void PhysicsCreate::addToScene(PxRigidActor* a) {
	if (a == nullptr) {
		cout <<"Acctor is empty"<<endl;
		return;
	}

	this->scene->addActor(*a);
}

PxRigidStatic* PhysicsCreate::createPlane(void* data) {
	PxRigidStatic* actor = PxCreatePlane(*this->physics, PxPlane(PxVec3(0, 0, 1), 0), *this->material);
	actor->userData = data;

	return actor;
}

PxRigidDynamic* PhysicsCreate::createDynamic(WOPhysicX* data) {
	PxTransform trans = PxTransform(PxVec3(data->getPosition().x, data->getPosition().y, data->getPosition().z));
	PxShape* shape = this->physics->createShape(PxBoxGeometry(2.0f, 2.0f, 2.0f), *this->material);
	PxRigidDynamic* actor = PxCreateDynamic(*this->physics, trans, *shape, 10.0f);
	actor->userData = data;

	return actor;
}

PxFilterFlags SampleSubmarineFilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	// let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	// trigger the contact callback for pairs (A,B) where
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return PxFilterFlag::eDEFAULT;
}

void PhysicsCreate::setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask) {
	PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask;  // word1 = ID mask to filter pairs that trigger a
									// contact callback;
	const PxU32 numShapes = actor->getNbShapes();
	PxShape** shapes = (PxShape**)SAMPLE_ALLOC(sizeof(PxShape*) * numShapes);
	actor->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}
	SAMPLE_FREE(shapes);
}

PxScene* PhysicsCreate::getScene() {
	return this->scene;
}

PxCooking* PhysicsCreate::getCooking() {
	return this->mCooking;
}

PxPhysics* PhysicsCreate::getPhysics() {
	return this->physics;
}