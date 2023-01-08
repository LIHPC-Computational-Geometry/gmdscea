/*----------------------------------------------------------------------------*/
#include<string>
/*----------------------------------------------------------------------------*/
#include <gtest/gtest.h>
/*----------------------------------------------------------------------------*/
#include <GMDS/IG/IG.h>
#include <GMDSCEA/LimaReader.h>
#include <GMDSCEA/LimaWriter.h>
/*----------------------------------------------------------------------------*/
using namespace gmds;
/*----------------------------------------------------------------------------*/
class LimaTest: public ::testing::Test {

  protected:
	LimaTest(){;}
    virtual ~LimaTest(){;}
};
/*----------------------------------------------------------------------------*/
TEST_F(LimaTest,read) {
	MeshModel mod = DIM3|N|R|R2N;
	IGMesh mesh(mod);

	LimaReader<IGMesh> reader(mesh);
	reader.read("Data/bar.mli",R|N);

	EXPECT_NE(0,mesh.getNbNodes());
	EXPECT_NE(0,mesh.getNbRegions());

}

/*----------------------------------------------------------------------------*/
TEST_F(LimaTest,write) {
	MeshModel mod = DIM3|N|F|F2N;
	IGMesh mesh(mod);

	Node n1 = mesh.newNode(0,0,0);
	Node n2 = mesh.newNode(1,0,0);
	Node n3 = mesh.newNode(1,1,0);
	Node n4 = mesh.newNode(0,1,0);
	Node n5 = mesh.newNode(0,1,1);
	mesh.newQuad(n1,n2,n3,n4);
	mesh.newTriangle(n3, n4, n5);
	LimaWriter<IGMesh> w(mesh);
	w.write("Data/out2D.mli",F|N);


}
