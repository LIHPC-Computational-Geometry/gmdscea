/*----------------------------------------------------------------------------*/
#include<string>
/*----------------------------------------------------------------------------*/
#include <gtest/gtest.h>
#include <Lima/malipp.h>
/*----------------------------------------------------------------------------*/
#include <GMDS/IG/IG.h>
#include <GMDSCEA/LimaReader.h>
#include <GMDSCEA/LimaWriter.h>
#include <GMDSCEA/LimaWriterAPI.h>
/*----------------------------------------------------------------------------*/
using namespace gmds;
/*----------------------------------------------------------------------------*/
class LimaAPITest: public ::testing::Test {

  protected:
	LimaAPITest(){;}
    virtual ~LimaAPITest(){;}
};
/*----------------------------------------------------------------------------*/
TEST_F(LimaAPITest,read) {
	MeshModel mod = DIM3|N|R|R2N;
	IGMesh mesh(mod);

	LimaReader<IGMesh> reader(mesh);
	reader.read("Data/bar.mli",R|N);

	EXPECT_NE(0,mesh.getNbNodes());
	EXPECT_NE(0,mesh.getNbRegions());

}

/*----------------------------------------------------------------------------*/
TEST_F(LimaAPITest,write) {
	MeshModel mod = DIM3|N|E|F|E2N|F2N;
	IGMesh mesh(mod);

	Node n1 = mesh.newNode(0,0,0);
	Node n2 = mesh.newNode(1,0,0);
	Node n3 = mesh.newNode(1,1,0);
	Node n4 = mesh.newNode(0,1,0);
	Node n5 = mesh.newNode(0,1,1);
	mesh.newQuad(n1,n2,n3,n4);
	mesh.newTriangle(n3, n4, n5);

	mesh.newEdge(n1,n2);
	mesh.newEdge(n2,n3);

//	LimaWriter<IGMesh> w(mesh);
//	w.write("Data/out2D.mli",F|N);

	gmds::IGMesh::cloud& cl0 = mesh.newCloud("cloud0");
	gmds::IGMesh::cloud& cl1 = mesh.newCloud("cloud1");
	gmds::IGMesh::cloud& cl2 = mesh.newCloud("cloud2");

	cl0.add(n2);
	cl0.add(n3);
	cl1.add(n2);

	gmds::LimaWriterAPI writer (mesh);
	writer.write("Data/poyop.mli",mod);

	LimaWriter<IGMesh> w(mesh);
	w.write("Data/base.mli",F|E|N);


	gmds::IGMesh mesh2(mod);
	gmds::LimaReader<gmds::IGMesh> reader(mesh2);
	reader.read("Data/poyop.mli",R|F|E|N);

	EXPECT_EQ(mesh.getNbNodes(),mesh2.getNbNodes());
	EXPECT_EQ(mesh.getNbEdges(),mesh2.getNbEdges());
	EXPECT_EQ(mesh.getNbFaces(),mesh2.getNbFaces());

	EXPECT_EQ(mesh.getNbClouds(),mesh2.getNbClouds());
	EXPECT_EQ(mesh.getNbLines(),mesh2.getNbLines());
	EXPECT_EQ(mesh.getNbSurfaces(),mesh2.getNbSurfaces());
}
/*----------------------------------------------------------------------------*/
TEST_F(LimaAPITest,write2) {
	MeshModel mod = DIM3|N|E|F|R|E2N|F2N|R2N;
	IGMesh mesh(mod);

	LimaReader<IGMesh> reader(mesh);
	reader.read("Data/poyop.mli",R|F|N);

	gmds::LimaWriterAPI writer (mesh);
	writer.write("Data/chess.mli",mod);

	IGMesh mesh2(mod);
	LimaReader<IGMesh> reader2(mesh2);
	reader2.read("Data/chess.mli",R|F|N);
}
/*----------------------------------------------------------------------------*/
