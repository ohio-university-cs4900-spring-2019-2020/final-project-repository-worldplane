#ifndef _PHYSICSCREATE
#define _PHYSICSCREATE
#include "PxPhysicsAPI.h"
#include "WOPhysicX.h"
#include "./cooking/PxCooking.h"

namespace Aftr {
	class WOPhysicX;
	class PhysicsCreate : public physx::PxSimulationEventCallback {
	public:
		PhysicsCreate();
		void shutdown();
		void addToScene(physx::PxRigidActor* a);
		physx::PxRigidStatic* createPlane(void* data);
		physx::PxRigidStatic* createStatic(WOPhysicX* data);
		physx::PxRigidDynamic* createWheeledCar(WOPhysicX* data, bool isMActor);
		physx::PxRigidDynamic* createDynamicMissile(WOPhysicX* data, physx::PxVec3 volecity);
		physx::PxRigidDynamic* createDynamicPlane(WOPhysicX* data, bool isMActor);
		void hitten();
		void removeActorsFromScene();
		int getTargetHealth();

		physx::PxScene* getScene();
		physx::PxCooking* getCooking();
		physx::PxPhysics* getPhysics();

		// virtual functions from PxSimulationEventCallback
		virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
		virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);
		virtual void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) {}
		virtual void onWake(physx::PxActor**, physx::PxU32) {}
		virtual void onSleep(physx::PxActor**, physx::PxU32) {}
		virtual void onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const physx::PxU32) {}

	protected:
		physx::PxDefaultAllocator allocator;
		physx::PxDefaultErrorCallback err;

		physx::PxFoundation* foundation;
		physx::PxPhysics* physics;
		physx::PxScene* scene;
		physx::PxPvd* pvd;
		physx::PxMaterial* material;
		physx::PxCooking* mCooking;

		physx::PxRigidDynamic* mActor;
		physx::PxRigidDynamic* tActor;
		physx::PxRigidDynamic* wheeledCarActor;
		physx::PxRigidDynamic* planeActor;
		std::vector<physx::PxRigidDynamic*> missileActors;
		std::vector<physx::PxRigidDynamic*> explodeMissileActors;
		std::vector<physx::PxRigidDynamic*> removedActors;
	};
}

#endif