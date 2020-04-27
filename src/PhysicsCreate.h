#ifndef _PHYSICSCREATE
#define _PHYSICSCREATE
#include "PxPhysicsAPI.h"
#include "WOPhysicX.h"
#include "./cooking/PxCooking.h"

namespace Aftr {
	class WOPhysicX;
	class PhysicsCreate {
	public:
		PhysicsCreate();
		void shutdown();
		void addToScene(physx::PxRigidActor* a);
		physx::PxRigidStatic* createPlane(void* data);
		physx::PxRigidDynamic* createDynamic(WOPhysicX* data);

		physx::PxScene* getScene();
		physx::PxCooking* getCooking();
		physx::PxPhysics* getPhysics();

	protected:
		physx::PxDefaultAllocator allocator;
		physx::PxDefaultErrorCallback err;

		physx::PxFoundation* foundation;
		physx::PxPhysics* physics;
		physx::PxScene* scene;
		physx::PxPvd* pvd;
		physx::PxMaterial* material;
		physx::PxCooking* mCooking;
	};
}

#endif