#include "PhysicsCreate.h"
#include "iostream"
#include "ManagerGLView.h"
#include "WorldContainer.h"
#include "GLView.h"


using namespace Aftr;
using namespace physx;
using namespace std;


static PxI32 targetHealth = 200;

struct FilterGroup
{
	enum Enum
	{
		eSUBMARINE = (1 << 0),
		eMINE_HEAD = (1 << 1),
		eMINE_LINK = (1 << 2),
		eCRAB = (1 << 3),
		eHEIGHTFIELD = (1 << 4),
	};
};

// customized filter shader
PxFilterFlags WorldPlaneFilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	//// let triggers through
	//if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	//{
	//	pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
	//	return PxFilterFlag::eDEFAULT;
	//}
	//// generate contacts for all that were not filtered above
	//pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	//// trigger the contact callback for pairs (A,B) where 
	//// the filtermask of A contains the ID of B and vice versa.
	//if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	//	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;


	pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return PxFilterFlag::eDEFAULT;
}

void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask;  // word1 = ID mask to filter pairs that trigger a contact callback;
	const PxU32 numShapes = actor->getNbShapes();
	PxShape** shapes = new PxShape * [numShapes];
	actor->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}
	delete[] shapes;
}

PhysicsCreate::PhysicsCreate() {
	this->foundation = PxCreateFoundation(PX_PHYSICS_VERSION, this->allocator, this->err);
	this->pvd = PxCreatePvd(*this->foundation);
	this->physics = PxCreatePhysics(PX_PHYSICS_VERSION, *this->foundation, PxTolerancesScale(), true, this->pvd);
	this->material = this->physics->createMaterial(0.f, 0.f, 0.f);
	this->planeActor = nullptr;
	this->wheeledCarActor = nullptr;
	this->mActor = nullptr;
	this->tActor = nullptr;

	PxSceneDesc s(this->physics->getTolerancesScale());
	s.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	s.gravity = PxVec3(0.0f, 0.0f, -9.8f);
	//s.filterShader = PxDefaultSimulationFilterShader ;
	s.filterShader = WorldPlaneFilterShader;
	s.simulationEventCallback = this;
	//s.flags|= PxSceneFlag::eREQUIRE_RW_LOCK;
	
	//s.filterShader = SampleSubmarineFilterShader ;
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

	if (this->planeActor != nullptr) {
		this->planeActor->release();
	}

	if (!this->missileActors.empty()) {
		//const size_t num = missileActors.size();
		//for (PxU32 i = 0; i < num; i++)
		//	delete this->missileActors[i];
		missileActors.clear();
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

PxRigidStatic* PhysicsCreate::createStatic(WOPhysicX* data) {
	PxTransform trans = PxTransform(PxVec3(data->getPosition().x, data->getPosition().y, data->getPosition().z));
	PxShape* shape = this->physics->createShape(PxBoxGeometry(2.0f, 2.0f, 2.0f), *this->material);
	PxRigidStatic* actor = PxCreateStatic(*this->physics, trans, *shape);
	actor->userData = data;

	return actor;
}

PxRigidDynamic* PhysicsCreate::createWheeledCar(WOPhysicX* data, bool isMActor) {
	PxTransform trans = PxTransform(PxVec3(data->getPosition().x, data->getPosition().y, data->getPosition().z));
	PxShape* shape = this->physics->createShape(PxBoxGeometry(2.0f, 2.0f, 2.0f), *this->material);
	this->wheeledCarActor = PxCreateDynamic(*this->physics, trans, *shape, 10.0f);
	this->wheeledCarActor->userData = data;
	this->wheeledCarActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	setupFiltering(this->wheeledCarActor, FilterGroup::eSUBMARINE, FilterGroup::eMINE_HEAD | FilterGroup::eMINE_LINK);

	if (isMActor) {
		this->mActor = this->wheeledCarActor;
	}
	else {
		this->tActor = this->wheeledCarActor;
	}

	return this->wheeledCarActor;
}

PxRigidDynamic* PhysicsCreate::createDynamicMissile(WOPhysicX* data, PxVec3 velocity) {
	PxTransform trans = PxTransform(PxVec3(data->getPosition().x, data->getPosition().y, data->getPosition().z));
	PxShape* shape = this->physics->createShape(PxBoxGeometry(2.0f, 2.0f, 2.0f), *this->material);
	PxRigidDynamic* actor = PxCreateDynamic(*this->physics, trans, *shape, 10.0f);
	actor->setLinearVelocity(velocity);
	actor->setMass(0.001f);
	actor->setMassSpaceInertiaTensor(PxVec3(0.001f));
	actor->userData = data;
	//actor->setLinearDamping(0.15f);
	//actor->setAngularDamping(15.f);

	this->missileActors.push_back(actor);

	return actor;
}

PxRigidDynamic* PhysicsCreate::createDynamicPlane(WOPhysicX* data, bool isMActor) {
	PxTransform trans = PxTransform(PxVec3(data->getPosition().x, data->getPosition().y, data->getPosition().z));
	PxShape* shape = this->physics->createShape(PxBoxGeometry(2.0f, 2.0f, 2.0f), *this->material);
	this->planeActor = PxCreateDynamic(*this->physics, trans, *shape, 10.0f);
	this->planeActor->userData = data;
	this->planeActor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);

	setupFiltering(this->planeActor, FilterGroup::eSUBMARINE, FilterGroup::eMINE_HEAD | FilterGroup::eMINE_LINK);

	if (isMActor) {
		this->mActor = this->planeActor;
	}
	else {
		this->tActor = this->planeActor;
	}

	return this->planeActor;
}

void PhysicsCreate::hitten() {
	cout << "!!!!Entering HITTEN()!!!!!!" << endl;
	if (this->tActor) {
		const size_t nExplodeMissiles = this->explodeMissileActors.size();

		for (PxU32 i = 0; i < nExplodeMissiles; i++) {
			PxRigidDynamic* currentExplode = this->explodeMissileActors[i];
			PxVec3 explosionPos = currentExplode->getGlobalPose().p;

			std::vector<PxRigidDynamic*>::iterator missileIter = std::find(
				this->missileActors.begin(), this->missileActors.end(), currentExplode);
			//remove missile			
			if (missileIter != this->missileActors.end()) {
				WOPhysicX* wo = static_cast<WOPhysicX*>((*missileIter)->userData);
				wo->isVisible = false;
				//int id = ManagerGLView::getGLView()->getWorldContainer()->getIndexOfWO(wo);
				ManagerGLView::getGLView()->getWorldContainer()->eraseViaWOptr(wo);
				ManagerGLView::getGLView()->getActorLst()->eraseViaWOptr(wo);
				this->missileActors.erase(missileIter);
			}
			explodeMissileActors.erase(explodeMissileActors.begin() + i);
			//currentExplode->release();			
			
			//damage to target
			static const PxReal strength = 100.0f;
			PxVec3 explosion = this->tActor->getGlobalPose().p - explosionPos;
			PxReal len = explosion.normalize();
			PxReal damage = strength * (1.0f / len);
			explosion *= damage;
			targetHealth = PxMax(targetHealth - PxI32(damage), 0);

			if (targetHealth <= 0) //GAME OVER
			{
				this->shutdown();
				//PxShape* shape = this->physics->createShape(PxBoxGeometry(2.0f, 2.0f, 2.0f), *this->material);
				////TODO: GAME OVER
				//PxTransform pose = PxShapeExt::getGlobalPose(*shape, *this->tActor);
				//PxGeometryHolder geom = shape->getGeometry();

				//// create new actor from shape (to split compound)
				//PxRigidDynamic* newActor = this->physics->createRigidDynamic(pose);

				//PxShape* newShape = PxRigidActorExt::createExclusiveShape(*newActor, geom.any(), *this->material);

				//newActor->setActorFlag(PxActorFlag::eVISUALIZATION, true);
				//newActor->setLinearDamping(10.5f);
				//newActor->setAngularDamping(0.5f);
				//PxRigidBodyExt::updateMassAndInertia(*newActor, 1.0f);
				//this->scene->addActor(*newActor);

				//PxVec3 newExplosion = pose.p - this->tActor->getGlobalPose().p;
				//PxReal newLen = newExplosion.normalize();
				//newExplosion *= (damage / newLen);
				//newActor->setLinearVelocity(newExplosion);
				//newActor->setAngularVelocity(PxVec3(1, 2, 3));

				//this->scene->removeActor(*this->tActor);
				break;
			}
		}
	}
}

void PhysicsCreate::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];

		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			if ((pairHeader.actors[0] == this->tActor) || (pairHeader.actors[1] == this->tActor))
			{
				PxActor* otherActor = (this->tActor == pairHeader.actors[0]) ? pairHeader.actors[1] : pairHeader.actors[0];
				//PxTransform trans = PxTransform(PxVec3(data->getPosition().x, data->getPosition().y, data->getPosition().z));
				//PxShape* shape = this->physics->createShape(PxBoxGeometry(2.0f, 2.0f, 2.0f), *this->material);
				//PxRigidDynamic* missile = PxCreateDynamic(*this->physics, trans, *shape, 10.0f);
				PxRigidDynamic* missile = reinterpret_cast<PxRigidDynamic*>(otherActor);
				// insert only once
				if (std::find(this->explodeMissileActors.begin(), this->explodeMissileActors.end(), missile) == this->explodeMissileActors.end())
					this->explodeMissileActors.push_back(missile);
				explodeMissileActors[0]->getGlobalPose().p;
				break;
			}
		}

		const PxContactPair& current = *pairs++;
		if (current.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			printf("Shape is entering trigger volume\n");
		if (current.events & PxPairFlag::eNOTIFY_TOUCH_LOST)
			printf("Shape is leaving trigger volume\n");
	}

	this->hitten();
}

void PhysicsCreate::onTrigger(PxTriggerPair* pairs, PxU32 count) {
	//for (PxU32 i = 0; i < count; i++)
	//{
	//	// ignore pairs when shapes have been deleted
	//	if (pairs[i].flags & (PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
	//		continue;

	//	if ((pairs[i].otherActor == this->planeActor) && (pairs[i].triggerActor == gTreasureActor))
	//	{
	//		gTreasureFound = true;
	//	}
	//}
	while (count--)
	{
		const PxTriggerPair& current = *pairs++;
		if (current.status & PxPairFlag::eNOTIFY_TOUCH_FOUND)
			printf("Shape is entering trigger volume\n");
		if (current.status & PxPairFlag::eNOTIFY_TOUCH_LOST)
			printf("Shape is leaving trigger volume\n");
	}
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
