#ifndef _WOPHYSICX
#define _WOPHYSICX
#include "WO.h"
#include "PhysicsCreate.h"

namespace Aftr {
	class PhysicsCreate;
	class WOPhysicX : public WO {
	public:
		static WOPhysicX* New(const std::string& modelFileName, const Vector& scale, MESH_SHADING_TYPE shadingType);
		Mat4 onCreateNVPhysXActor(physx::PxTransform* trans);
		//void updatePoseFromPhysicsEngine(physx::PxTransform* trans);

		void setActor(physx::PxRigidActor* a);
		physx::PxRigidActor* getActor();
		void setEngine(PhysicsCreate* p);
	protected:
		WOPhysicX();
		physx::PxRigidActor* actor;
		PhysicsCreate* engine;
	};
}

#endif
