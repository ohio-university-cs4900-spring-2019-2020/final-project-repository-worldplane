#ifndef WOPhysXTriangularMesh_H
#define WOPhysXTriangularMesh_H

#include "WO.h"
#include "PxPhysicsAPI.h"
#include "WOGridECEFElevation.h"
#include "./cooking/PxCooking.h"
#include "./cooking/PxTriangleMeshDesc.h"
#include "./geometry/PxTriangleMeshGeometry.h"
#include "./foundation/PxTransform.h"
#include "./foundation/PxVec3.h"

#include "PhysicsCreate.h"

namespace Aftr {
	class WOPhysXTriangularMesh : public WOGridECEFElevation {
	public:
		WOMacroDeclaration(WOPhysXTriangularMesh, WOGridECEFElevation);
		static WOPhysXTriangularMesh* New();
		//WOPhysXTriangularMesh(PhysicsCreate* p);
		void onCreate(WO* wo);
		void createGrid();
		void init(PhysicsCreate* p);

	protected:
		WOPhysXTriangularMesh();
		float* vertexListCopy;
		unsigned int* indicesCopy;
		PhysicsCreate* physEngine;
		physx::PxCooking* cooking;
		physx::PxPhysics* physics;
	};
}
#endif