#include <iostream>
/*----------------------------------------------------------------------------*/
#include <GMDS/IG/IG.h>
/*----------------------------------------------------------------------------*/
using namespace gmds;
int main() {
        MeshModel mod = DIM2|N|F|F2N;
        IGMesh mesh(mod);
        mesh.newNode(0,0);
        mesh.newNode(0,1);
        Variable<int> *v = mesh.newVariable<int>(GMDS_NODE,"var1");

        mesh.newNode(1,0);
        mesh.newNode(1,1);

        IGMesh::node_iterator it = mesh.nodes_begin();

        for(;!it.isDone();it.next()){
                TCellID i = (it.value()).getID();
                int n = (*v)[i];
                (*v)[i]=1;
        }
        for(it=mesh.nodes_begin();!it.isDone();it.next()){
                TCellID i = (it.value()).getID();
                int n = (*v)[i];
        }
    return 0;
}
