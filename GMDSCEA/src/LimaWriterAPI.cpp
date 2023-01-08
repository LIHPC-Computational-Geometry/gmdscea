/*----------------------------------------------------------------------------*/
/** \file    LimaWriterAPI.cpp
 *  \author  legoff
 *  \date    30/06/2016
 */
/*----------------------------------------------------------------------------*/
#include "GMDSCEA/LimaWriterAPI.h"
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include <Lima/malipp.h>
#include <Lima/erreur.h>
#include <Lima/polyedre.h>
#include <GMDS/IG/IG.h>
/*----------------------------------------------------------------------------*/
namespace gmds{
/*----------------------------------------------------------------------------*/
LimaWriterAPI::LimaWriterAPI(gmds::IGMesh& AMesh)
:mesh_(AMesh),lenghtUnit_(1.)
{

}
/*----------------------------------------------------------------------------*/
LimaWriterAPI::~LimaWriterAPI()
{

}
/*----------------------------------------------------------------------------*/
void
LimaWriterAPI::setLengthUnit(double AUnit)
{
	lenghtUnit_ = AUnit;
}
/*----------------------------------------------------------------------------*/
void
LimaWriterAPI::activateZlibCompression()
{
  //WARNING: only available using the hdf145 extension
  //writer_->activer_compression_zlib();
}
/*----------------------------------------------------------------------------*/
void
LimaWriterAPI::write(const std::string& AFileName, gmds::MeshModel AModel, int ACompact)
{
	try {
		writer_ = new Lima::MaliPPWriter2(AFileName, 1);

		writer_->unite_longueur(lenghtUnit_);
		Lima::dim_t dim;
		if(mesh_.getDim() == 3) {
			dim = Lima::D3;
		} else if(mesh_.getDim() == 2) {
			dim = Lima::D2;
		} else {
			dim = Lima::D1;
		}
		writer_->dimension(dim);

		writer_->beginWrite();



		writeNodes();
		writeEdges();
		writeFaces();
		writeRegions();

		writeClouds();
		writeLines();
		writeSurfaces();
		writeVolumes();

		writeNodesAttributes();
		writeEdgesAttributes();
		writeFacesAttributes();
		writeRegionsAttributes();

		writeCloudsAttributes();
		writeLinesAttributes();
		writeSurfacesAttributes();
		writeVolumesAttributes();

		writer_->close ( );
	}
	catch(Lima::write_erreur& e) {
		std::cerr<<"GMDSCEA ERREUR LimaWriterAPI::write : "<<e.what()<<std::endl;
		throw GMDSException(e.what());
	}
}
/*----------------------------------------------------------------------------*/
void
LimaWriterAPI::writeNodes()
{
	// check whether the ids are contiguous
	bool isContiguous = true;
	gmds::TCellID minID = gmds::NullID;
	{
		if(mesh_.getNbNodes() > 0) {

			gmds::IGMesh::node_iterator it_nodes     = mesh_.nodes_begin();
			gmds::TCellID currentID = it_nodes.value().getID();
			minID = currentID+1; // +1 because mli ids begin at 1
			it_nodes.next();
			for(;!it_nodes.isDone();it_nodes.next()) {
				Node n = it_nodes.value();
				if(n.getID() != currentID+1) {
					isContiguous = false;
					break;
				}
				currentID = n.getID();
			}
		}
	}

	try {
		writer_->writeNodesInfo(isContiguous,mesh_.getNbNodes(),minID);
	}
	catch(Lima::write_erreur& e) {
		std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
		throw GMDSException(e.what());
	}

	const Lima::id_type LimaWriterAPI_NBNODES_CHUNK = 10000;

	double* xccords = new double[LimaWriterAPI_NBNODES_CHUNK];
	double* yccords = new double[LimaWriterAPI_NBNODES_CHUNK];
	double* zccords = new double[LimaWriterAPI_NBNODES_CHUNK];
	Lima::id_type* ids = new Lima::id_type[LimaWriterAPI_NBNODES_CHUNK];

	Lima::id_type chunkSize = 0;

	gmds::IGMesh::node_iterator it_nodes     = mesh_.nodes_begin();
	for(;!it_nodes.isDone();it_nodes.next())
	{
		Node n = it_nodes.value();
		xccords[chunkSize] = n.X();
		yccords[chunkSize] = n.Y();
		zccords[chunkSize] = n.Z();
		ids[chunkSize] = n.getID()+1; // +1 because mli ids begin at 1

		chunkSize++;
		if(chunkSize==LimaWriterAPI_NBNODES_CHUNK) {
			try {
				writer_->writeNodes(chunkSize,xccords,yccords,zccords,ids);
			}
			catch(Lima::write_erreur& e) {
				std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
				throw GMDSException(e.what());
			}
			chunkSize = 0;
		}
	}

	if(chunkSize>0) {
		try {
			writer_->writeNodes(chunkSize,xccords,yccords,zccords,ids);
		}
		catch(Lima::write_erreur& e) {
			std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
			throw GMDSException(e.what());
		}
	}

	delete[] xccords;
	delete[] yccords;
	delete[] zccords;
	delete[] ids;
}
/*----------------------------------------------------------------------------*/
void
LimaWriterAPI::writeEdges()
{
	// check whether the ids are contiguous
	bool isContiguous = true;
	gmds::TCellID minID = gmds::NullID;
	{
		if(mesh_.getNbEdges() > 0) {

			gmds::IGMesh::edge_iterator it_edges     = mesh_.edges_begin();
			gmds::TCellID currentID = it_edges.value().getID();
			minID = currentID+1; // +1 because mli ids begin at 1
			it_edges.next();
			for(;!it_edges.isDone();it_edges.next()) {
				Edge e = it_edges.value();
				if(e.getID() != currentID+1) {
					isContiguous = false;
					break;
				}
				currentID = e.getID();
			}
		}
	}

	try {
		writer_->writeEdgesInfo(isContiguous,mesh_.getNbEdges(),minID);
	}
	catch(Lima::write_erreur& e) {
		std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
		throw GMDSException(e.what());
	}

	const Lima::id_type LimaWriterAPI_NBEDGES_CHUNK = 10000;

	Lima::id_type* edge2nodeIDs = new Lima::id_type[2*LimaWriterAPI_NBEDGES_CHUNK];
	Lima::id_type* ids= new Lima::id_type[LimaWriterAPI_NBEDGES_CHUNK];

	Lima::id_type chunkSize = 0;

	gmds::IGMesh::edge_iterator it_edges     = mesh_.edges_begin();
	for(;!it_edges.isDone();it_edges.next())
	{
		Edge e = it_edges.value();
		std::vector<gmds::TCellID> nodesIDs = e.getAllIDs<gmds::Node>();
		edge2nodeIDs[2*chunkSize  ] = nodesIDs[0]+1;
		edge2nodeIDs[2*chunkSize+1] = nodesIDs[1]+1;
		ids[chunkSize] = e.getID()+1;

		chunkSize++;
		if(chunkSize==LimaWriterAPI_NBEDGES_CHUNK) {
			try {
				writer_->writeEdges(chunkSize,edge2nodeIDs,ids);
			}
			catch(Lima::write_erreur& e) {
				std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
				throw GMDSException(e.what());
			}
			chunkSize = 0;
		}
	}

	if(chunkSize>0) {
		try {
			writer_->writeEdges(chunkSize,edge2nodeIDs,ids);
		}
		catch(Lima::write_erreur& e) {
			std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
			throw GMDSException(e.what());
		}
	}

	delete[] edge2nodeIDs;
	delete[] ids;
}
/*----------------------------------------------------------------------------*/
void
LimaWriterAPI::writeFaces()
{
	// check whether the ids are contiguous
	bool isContiguous = true;
	gmds::TCellID minID = gmds::NullID;
	{
		if(mesh_.getNbFaces() > 0) {

			gmds::IGMesh::face_iterator it_faces     = mesh_.faces_begin();
			gmds::TCellID currentID = it_faces.value().getID();
			minID = currentID+1; // +1 because mli ids begin at 1
			it_faces.next();
			for(;!it_faces.isDone();it_faces.next()) {
				Face f = it_faces.value();
				if(f.getID() != currentID+1) {
					isContiguous = false;
					break;
				}
				currentID = f.getID();
			}
		}
	}

	try {
		writer_->writeFacesInfo(isContiguous,mesh_.getNbFaces(),minID);
	}
	catch(Lima::write_erreur& e) {
		std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
		throw GMDSException(e.what());
	}

	const Lima::id_type LimaWriterAPI_NBFACES_CHUNK = 10000;
	const Lima::id_type LimaWriterAPI_MAX_NBNODES_PER_FACE = 15; //Lima::MAX_NOEUDS;

	Lima::id_type* face2nodeIDs = new Lima::id_type[LimaWriterAPI_MAX_NBNODES_PER_FACE*LimaWriterAPI_NBFACES_CHUNK];
	Lima::id_type* nbNodesPerFace = new Lima::id_type[LimaWriterAPI_NBFACES_CHUNK];
	Lima::id_type* ids = new Lima::id_type[LimaWriterAPI_NBFACES_CHUNK];

	Lima::id_type chunkSize = 0;
	Lima::id_type currentIndex = 0;

	gmds::IGMesh::face_iterator it_faces     = mesh_.faces_begin();
	for(;!it_faces.isDone();it_faces.next())
	{
		Face f = it_faces.value();
		std::vector<gmds::TCellID> nodesIDs = f.getAllIDs<gmds::Node>();
		nbNodesPerFace[chunkSize] = nodesIDs.size();

		if(nodesIDs.size() > LimaWriterAPI_MAX_NBNODES_PER_FACE) {
			throw GMDSException("LimaWriterAPI::writeFaces a face has too many nodes (> 15 == Lima::MAX_NOEUDS).");
		}

		for(int i = 0; i<nodesIDs.size(); i++) {
			face2nodeIDs[currentIndex] = nodesIDs[i]+1;
			currentIndex++;
		}

		ids[chunkSize] = f.getID()+1;

		chunkSize++;
		if(chunkSize==LimaWriterAPI_NBFACES_CHUNK) {
			try {
				writer_->writeFaces(chunkSize,face2nodeIDs,nbNodesPerFace,ids);
			}
			catch(Lima::write_erreur& e) {
				std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
				throw GMDSException(e.what());
			}
			chunkSize = 0;
			currentIndex = 0;
		}
	}

	if(chunkSize>0) {
		try {
			writer_->writeFaces(chunkSize,face2nodeIDs,nbNodesPerFace,ids);
		}
		catch(Lima::write_erreur& e) {
			std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
			throw GMDSException(e.what());
		}
	}

	delete[] face2nodeIDs;
	delete[] nbNodesPerFace;
	delete[] ids;
}
/*----------------------------------------------------------------------------*/
void
LimaWriterAPI::writeRegions()
{
	// check whether the ids are contiguous
	bool isContiguous = true;
	gmds::TCellID minID = gmds::NullID;
	{
		if(mesh_.getNbRegions() > 0) {

			gmds::IGMesh::region_iterator it_regions     = mesh_.regions_begin();
			gmds::TCellID currentID = it_regions.value().getID();
			minID = currentID+1; // +1 because mli ids begin at 1
			it_regions.next();
			for(;!it_regions.isDone();it_regions.next()) {
				Region r = it_regions.value();
				if(r.getID() != currentID+1) {
					isContiguous = false;
					break;
				}
				currentID = r.getID();
			}
		}
	}

	try {
		writer_->writeRegionsInfo(isContiguous,mesh_.getNbRegions(),minID);
	}
	catch(Lima::write_erreur& e) {
		std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
		throw GMDSException(e.what());
	}

	const Lima::id_type LimaWriterAPI_NBREGIONS_CHUNK = 10000;
	const Lima::id_type LimaWriterAPI_MAX_NBNODES_PER_REGION = 15; //Lima::MAX_NOEUDS;

	Lima::id_type* region2nodeIDs = new Lima::id_type[LimaWriterAPI_MAX_NBNODES_PER_REGION*LimaWriterAPI_NBREGIONS_CHUNK];
	Lima::Polyedre::PolyedreType* regionTypes = new Lima::Polyedre::PolyedreType[LimaWriterAPI_NBREGIONS_CHUNK];
	Lima::id_type* ids = new Lima::id_type[LimaWriterAPI_NBREGIONS_CHUNK];

	Lima::id_type chunkSize = 0;
	Lima::id_type currentIndex = 0;

	gmds::IGMesh::region_iterator it_regions     = mesh_.regions_begin();
	for(;!it_regions.isDone();it_regions.next())
	{
		Region r = it_regions.value();
		std::vector<gmds::TCellID> nodesIDs = r.getAllIDs<gmds::Node>();

		switch(r.getType()) {
		case gmds::GMDS_TETRA :
			regionTypes[chunkSize] = Lima::Polyedre::TETRAEDRE;
			break;
		case gmds::GMDS_PYRAMID :
			regionTypes[chunkSize] = Lima::Polyedre::PYRAMIDE;
			break;
		case gmds::GMDS_PRISM3 :
			regionTypes[chunkSize] = Lima::Polyedre::PRISME;
			break;
		case gmds::GMDS_HEX :
			regionTypes[chunkSize] = Lima::Polyedre::HEXAEDRE;
			break;
		default:
			throw GMDSException("LimaWriterAPI::writeRegions cell type not handled by Lima.");
		}

		if(nodesIDs.size() > LimaWriterAPI_MAX_NBNODES_PER_REGION) {
			throw GMDSException("LimaWriterAPI::writeRegions a face has too many nodes (> 15 == Lima::MAX_NOEUDS).");
		}

		for(int i = 0; i<nodesIDs.size(); i++) {
			region2nodeIDs[currentIndex] = nodesIDs[i]+1;
			currentIndex++;
		}

		ids[chunkSize] = r.getID()+1;

		chunkSize++;
		if(chunkSize==LimaWriterAPI_NBREGIONS_CHUNK) {
			try {
				writer_->writeRegions(chunkSize,region2nodeIDs,regionTypes,ids);
			}
			catch(Lima::write_erreur& e) {
				std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
				throw GMDSException(e.what());
			}
			chunkSize = 0;
			currentIndex = 0;
		}
	}

	if(chunkSize>0) {
		try {
			writer_->writeRegions(chunkSize,region2nodeIDs,regionTypes,ids);
		}
		catch(Lima::write_erreur& e) {
			std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
			throw GMDSException(e.what());
		}
	}

	delete[] region2nodeIDs;
	delete[] regionTypes;
	delete[] ids;
}
/*----------------------------------------------------------------------------*/
void
LimaWriterAPI::writeClouds()
{
	const Lima::id_type LimaWriterAPI_NBNODES_CHUNK = 10000;

	std::vector<std::string> names;
	std::vector<Lima::id_type> sizes;

	for(int i=0; i<mesh_.getNbClouds(); i++) {
		gmds::IGMesh::cloud& cl = mesh_.getCloud(i);
		names.push_back(cl.name());
		sizes.push_back(cl.size());
	}

	Lima::id_type nbClouds = mesh_.getNbClouds();

	try {
		writer_->writeNodeSetInfo(nbClouds,names,sizes);
	}
	catch(Lima::write_erreur& e) {
		std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
		throw GMDSException(e.what());
	}

	Lima::id_type ids[LimaWriterAPI_NBNODES_CHUNK];
	Lima::id_type chunkSize = 0;

	for(int i=0; i<mesh_.getNbClouds(); i++) {
		gmds::IGMesh::cloud& cl = mesh_.getCloud(i);

		std::vector<gmds::TCellID> nodeIDs= cl.cellIDs();

		for(int iNode=0; iNode<nodeIDs.size(); iNode++) {
			ids[chunkSize] = nodeIDs[iNode]+1;
			chunkSize++;

			if(chunkSize==LimaWriterAPI_NBNODES_CHUNK) {
				try {
					writer_->writeNodeSetData(cl.name(),chunkSize,ids);
				}
				catch(Lima::write_erreur& e) {
					std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
					throw GMDSException(e.what());
				}
				chunkSize = 0;
			}
		}

		if(chunkSize > 0) {
			try {
				writer_->writeNodeSetData(cl.name(),chunkSize,ids);
			}
			catch(Lima::write_erreur& e) {
				std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
				throw GMDSException(e.what());
			}
			chunkSize = 0;
		}

	}
}
/*----------------------------------------------------------------------------*/
void
LimaWriterAPI::writeLines()
{
	const Lima::id_type LimaWriterAPI_NBEDGES_CHUNK = 10000;

	std::vector<std::string> names;
	std::vector<Lima::id_type> sizes;

	for(int i=0; i<mesh_.getNbLines(); i++) {
		gmds::IGMesh::line& l = mesh_.getLine(i);
		names.push_back(l.name());
		sizes.push_back(l.size());
	}

	Lima::id_type nbLines = mesh_.getNbLines();

	try {
		writer_->writeEdgeSetInfo(nbLines,names,sizes);
	}
	catch(Lima::write_erreur& e) {
		std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
		throw GMDSException(e.what());
	}

	Lima::id_type ids[LimaWriterAPI_NBEDGES_CHUNK];
	Lima::id_type chunkSize = 0;

	for(int i=0; i<mesh_.getNbLines(); i++) {
		gmds::IGMesh::line& l = mesh_.getLine(i);

		std::vector<gmds::TCellID> edgeIDs= l.cellIDs();

		for(int iEdge=0; iEdge<edgeIDs.size(); iEdge++) {
			ids[chunkSize] = edgeIDs[iEdge]+1;
			chunkSize++;

			if(chunkSize==LimaWriterAPI_NBEDGES_CHUNK) {
				try {
					writer_->writeEdgeSetData(l.name(),chunkSize,ids);
				}
				catch(Lima::write_erreur& e) {
					std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
					throw GMDSException(e.what());
				}
				chunkSize = 0;
			}
		}

		if(chunkSize > 0) {
			try {
				writer_->writeEdgeSetData(l.name(),chunkSize,ids);
			}
			catch(Lima::write_erreur& e) {
				std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
				throw GMDSException(e.what());
			}
			chunkSize = 0;
		}

	}
}
/*----------------------------------------------------------------------------*/
void
LimaWriterAPI::writeSurfaces()
{
	const Lima::id_type LimaWriterAPI_NBFACES_CHUNK = 10000;

	std::vector<std::string> names;
	std::vector<Lima::id_type> sizes;

	for(int i=0; i<mesh_.getNbSurfaces(); i++) {
		gmds::IGMesh::surface& surf = mesh_.getSurface(i);
		names.push_back(surf.name());
		sizes.push_back(surf.size());
	}

	Lima::id_type nbSurfaces = mesh_.getNbSurfaces();

	try {
		writer_->writeFaceSetInfo(nbSurfaces,names,sizes);
	}
	catch(Lima::write_erreur& e) {
		std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
		throw GMDSException(e.what());
	}

	Lima::id_type ids[LimaWriterAPI_NBFACES_CHUNK];
	Lima::id_type nbNodes[LimaWriterAPI_NBFACES_CHUNK];
	Lima::id_type chunkSize = 0;

	for(int i=0; i<mesh_.getNbSurfaces(); i++) {
		gmds::IGMesh::surface& surf = mesh_.getSurface(i);

		std::vector<gmds::TCellID> faceIDs= surf.cellIDs();

		for(int iFace=0; iFace<faceIDs.size(); iFace++) {
			ids[chunkSize] = faceIDs[iFace]+1;
			nbNodes[chunkSize] = (mesh_.get<gmds::Face> (faceIDs[iFace])).getNbNodes();
			chunkSize++;

			if(chunkSize==LimaWriterAPI_NBFACES_CHUNK) {
				try {
					writer_->writeFaceSetData(surf.name(),chunkSize,ids,nbNodes);
				}
				catch(Lima::write_erreur& e) {
					std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
					throw GMDSException(e.what());
				}
				chunkSize = 0;
			}
		}

		if(chunkSize > 0) {
			try {
				writer_->writeFaceSetData(surf.name(),chunkSize,ids,nbNodes);
			}
			catch(Lima::write_erreur& e) {
				std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
				throw GMDSException(e.what());
			}
			chunkSize = 0;
		}

	}
}
/*----------------------------------------------------------------------------*/
void
LimaWriterAPI::writeVolumes()
{
	const Lima::id_type LimaWriterAPI_NBREGIONS_CHUNK = 10000;

	std::vector<std::string> names;
	std::vector<Lima::id_type> sizes;

	for(int i=0; i<mesh_.getNbVolumes(); i++) {
		gmds::IGMesh::volume& vol = mesh_.getVolume(i);
		names.push_back(vol.name());
		sizes.push_back(vol.size());
	}

	Lima::id_type nbVolumes = mesh_.getNbVolumes();

	try {
		writer_->writeRegionSetInfo(nbVolumes,names,sizes);
	}
	catch(Lima::write_erreur& e) {
		std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
		throw GMDSException(e.what());
	}

	Lima::id_type ids[LimaWriterAPI_NBREGIONS_CHUNK];
	Lima::Polyedre::PolyedreType types[LimaWriterAPI_NBREGIONS_CHUNK];
	Lima::id_type chunkSize = 0;

	for(int i=0; i<mesh_.getNbVolumes(); i++) {
		gmds::IGMesh::volume& vol = mesh_.getVolume(i);

		std::vector<gmds::TCellID> regionIDs= vol.cellIDs();

		for(int iRegion=0; iRegion<regionIDs.size(); iRegion++) {
			ids[chunkSize] = regionIDs[iRegion]+1;
			switch((mesh_.get<gmds::Region>(regionIDs[iRegion])).getType()) {
			case gmds::GMDS_TETRA :
				types[chunkSize] = Lima::Polyedre::TETRAEDRE;
				break;
			case gmds::GMDS_PYRAMID :
				types[chunkSize] = Lima::Polyedre::PYRAMIDE;
				break;
			case gmds::GMDS_PRISM3 :
				types[chunkSize] = Lima::Polyedre::PRISME;
				break;
			case gmds::GMDS_HEX :
				types[chunkSize] = Lima::Polyedre::HEXAEDRE;
				break;
			default:
				throw GMDSException("LimaWriterAPI::writeRegions cell type not handled by Lima.");
			}
			chunkSize++;

			if(chunkSize==LimaWriterAPI_NBREGIONS_CHUNK) {
				try {
					writer_->writeRegionSetData(vol.name(),chunkSize,ids,types);
				}
				catch(Lima::write_erreur& e) {
					std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
					throw GMDSException(e.what());
				}
				chunkSize = 0;
			}
		}

		if(chunkSize > 0) {
			try {
				writer_->writeRegionSetData(vol.name(),chunkSize,ids,types);
			}
			catch(Lima::write_erreur& e) {
				std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
				throw GMDSException(e.what());
			}
			chunkSize = 0;
		}

	}
}
/*----------------------------------------------------------------------------*/
void
LimaWriterAPI::writeNodesAttributes()
{
	writer_->writeNodeAttributes();
}
/*----------------------------------------------------------------------------*/
void
LimaWriterAPI::writeEdgesAttributes()
{
	writer_->writeEdgeAttributes();
}
/*----------------------------------------------------------------------------*/
void
LimaWriterAPI::writeFacesAttributes()
{
	writer_->writeFaceAttributes();
}
/*----------------------------------------------------------------------------*/
void
LimaWriterAPI::writeRegionsAttributes()
{
	writer_->writeRegionAttributes();
}
/*----------------------------------------------------------------------------*/
void
LimaWriterAPI::writeCloudsAttributes()
{
	writer_->writeNodeSetsAttributes();
}
/*----------------------------------------------------------------------------*/
void
LimaWriterAPI::writeLinesAttributes()
{
	writer_->writeEdgeSetsAttributes();
}
/*----------------------------------------------------------------------------*/
void
LimaWriterAPI::writeSurfacesAttributes()
{
	writer_->writeFaceSetsAttributes();
}
/*----------------------------------------------------------------------------*/
void
LimaWriterAPI::writeVolumesAttributes()
{
	writer_->writeRegionSetsAttributes();
}
/*----------------------------------------------------------------------------*/
}  // namespace gmds
/*----------------------------------------------------------------------------*/
