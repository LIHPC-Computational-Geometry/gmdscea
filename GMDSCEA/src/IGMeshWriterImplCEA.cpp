/*
 * IGMeshWriterImplCEA.cpp
 *
 *  Created on: 21 mai 2014
 *      Author: ledouxf
 */




#include <GMDSCEA/LimaReader.h>
#include <GMDSCEA/LimaWriter.h>
#include <GMDS/IG/IGMesh.h>

namespace gmds{

template class LimaReader<IGMesh>;
template class LimaWriter<IGMesh>;

}
