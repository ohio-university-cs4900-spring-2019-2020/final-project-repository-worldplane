#include "WOPhysXTriangularMesh.h"

#include "Model.h"
#include "ModelDataShared.h"
#include "ModelDataSharedID.h"
#include "Vector.h"
#include "Mat4.h"

#include "ManagerGLView.h"
#include "WorldContainer.h"
#include "GLView.h"


using namespace Aftr;
using namespace std;
using namespace physx;

WOMacroDeclaration(WOPhysXTriangularMesh, WOGridECEFElevation);

WOPhysXTriangularMesh* WOPhysXTriangularMesh::New() {
	WOPhysXTriangularMesh* wo = new WOPhysXTriangularMesh();

	return wo;
}

WOPhysXTriangularMesh::WOPhysXTriangularMesh()
	: IFace(this) {
	this->physEngine = nullptr;
	this->cooking = nullptr;
	this->physics = nullptr;
}

void WOPhysXTriangularMesh::init(PhysicsCreate* p) {
	this->physEngine = p;
	this->cooking = p->getCooking();
	this->physics = p->getPhysics();
}

void WOPhysXTriangularMesh::onCreate(WO* wo)
{
	//make copy of vertex list and index list
	size_t vertexListSize = wo->getModel()->getModelDataShared()->getCompositeVertexList().size();
	size_t indexListSize = wo->getModel()->getModelDataShared()->getCompositeIndexList().size();

	this->vertexListCopy = new float[vertexListSize * 3];
	this->indicesCopy = new unsigned int[indexListSize];

	for (size_t i = 0; i < vertexListSize; i++) {
		this->vertexListCopy[i * 3 + 0] = wo->getModel()->getModelDataShared()->getCompositeVertexList().at(i).x;
		this->vertexListCopy[i * 3 + 1] = wo->getModel()->getModelDataShared()->getCompositeVertexList().at(i).y;
		this->vertexListCopy[i * 3 + 2] = wo->getModel()->getModelDataShared()->getCompositeVertexList().at(i).z;
	}

	for (size_t i = 0; i < indexListSize; i++)
		this->indicesCopy[i] = wo->getModel()->getModelDataShared()->getCompositeIndexList().at(i);

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = vertexListSize;
	meshDesc.points.stride = sizeof(float) * 3;
	meshDesc.points.data = this->vertexListCopy;

	meshDesc.triangles.count = indexListSize / 3;
	meshDesc.triangles.stride = 3 * sizeof(unsigned int);
	meshDesc.triangles.data = this->indicesCopy;

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	bool status = this->cooking->cookTriangleMesh(meshDesc, writeBuffer, &result);
	if (!status) {
		std::cout << "Failed to create Triangular mesh" << std::endl;
		std::cin.get();
	}

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxTriangleMesh* mesh = this->physics->createTriangleMesh(readBuffer);

	PxMaterial* gMaterial = this->physics->createMaterial(0.5f, 0.5f, 0.6f);
	PxShape* shape = this->physics->createShape(PxTriangleMeshGeometry(mesh), *gMaterial, true);
	PxTransform t({ 0,0,0 });

	PxRigidStatic* a = this->physics->createRigidStatic(t);
	bool b = a->attachShape(*shape);

	Mat4 mat = wo->getDisplayMatrix();
	PxMat44 pmat = PxMat44();
	pmat[0][0] = mat[0]; pmat[0][1] = mat[1]; pmat[0][2] = mat[2]; pmat[0][3] = mat[3];
	pmat[1][0] = mat[4]; pmat[1][1] = mat[5]; pmat[1][2] = mat[6]; pmat[1][3] = mat[7];
	pmat[2][0] = mat[8]; pmat[2][1] = mat[9]; pmat[2][2] = mat[10]; pmat[2][3] = mat[11];
	pmat[3][0] = mat[12]; pmat[3][1] = mat[13]; pmat[3][2] = mat[14]; pmat[3][3] = mat[15];
	pmat.setPosition(PxVec3(wo->getPosition().x, wo->getPosition().y, wo->getPosition().z));

	a->setGlobalPose(PxTransform(pmat));

	a->userData = wo;
	this->physEngine->addToScene(a);
}

void WOPhysXTriangularMesh::createGrid() {
	float top = 34.2072593790098f;
	float bottom = 33.9980272592999f;

	float left = -118.65234375f;
	float right = -118.443603515625f;

	float vert = top - bottom;
	float horz = right - left;

	VectorD offset((top + bottom) / 2, (left + right) / 2, 0);
	Vector centerOfWorld = offset.toECEFfromWGS84().toVecS();
	Vector gravityDirection = centerOfWorld;
	gravityDirection.normalize();
	

	const int gran = 50;

	VectorD scale = VectorD(0.5, 0.5, 0.5);
	VectorD upperLeft((double)top, (double)left, 0);
	VectorD lowerRight((double)bottom, (double)right, 0);
	string elevation = ManagerEnvironmentConfiguration::getLMM() + "/images/woodland.tif";
	//WOGridECEFElevation::onCreate(upperLeft, lowerRight, 0, offset, scale, elevation, 2, 0, false);

	WO* grid = WOGridECEFElevation::New(upperLeft, lowerRight, 0, offset, scale, elevation);
	grid->setLabel("grid");

	VectorD v = ((upperLeft + lowerRight) / 2.0);
	//VectorD v = (upperLeft, lowerRight, 0.0);
	VectorD zAxis = v.toECEFfromWGS84();
	zAxis.normalize();
	VectorD xAxis = (VectorD(1.0, 1.0, 0.0).toECEFfromWGS84() - v.toECEFfromWGS84()).vectorProjectOnToPlane(zAxis);
	//xAxis = xAxis.vectorProjectOnToPlane(zAxis);
	xAxis.normalize();
	VectorD yAxis = zAxis.crossProduct(xAxis);
	yAxis.normalize();
	float mat[16] = {
		xAxis.x, xAxis.y, xAxis.z, 0.0f,
		yAxis.x, yAxis.y, yAxis.z, 0.0f,
		zAxis.x, zAxis.y, zAxis.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	grid->getModel()->setDisplayMatrix(Mat4(mat).transposeUpperLeft3x3());
	grid->setPosition(0, 0, -500.0);
	ManagerGLView::getGLView()->getWorldContainer()->push_back(grid);

	for (size_t i = 0; i < grid->getModel()->getModelDataShared()->getModelMeshes().size(); i++)
		grid->getModel()->getModelDataShared()->getModelMeshes().at(i)->getSkin().getMultiTextureSet().at(0) = ManagerTexture::loadTexture(ManagerEnvironmentConfiguration::getLMM() + "/images/woodland.bmp");
	//for (ModelMesh* mesh : grid->getModel()->getModelDataShared()->getModelMeshes()) {
	//	mesh->getSkin().getMultiTextureSet().at(0) = ManagerTexture::loadTexture(ManagerEnvironmentConfiguration::getLMM() + "/images/woodland.bmp");
	//}
	grid->getModel()->isUsingBlending(false);

	this->onCreate(grid);
}
