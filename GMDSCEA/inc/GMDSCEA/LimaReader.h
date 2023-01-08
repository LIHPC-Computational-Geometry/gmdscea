/*----------------------------------------------------------------------------*/
/** \file    LimaReader.h
 *  \author  F. LEDOUX
 *  \date    09/11/2008
 */
/*----------------------------------------------------------------------------*/
#ifndef LIMAREADER_H_
#define LIMAREADER_H_
/*----------------------------------------------------------------------------*/
// headers of STL files
#include <iostream>
#include <fstream>
#include <map>
#include "Lima/lima++.h"
/*----------------------------------------------------------------------------*/
// headers of VTK files
/*----------------------------------------------------------------------------*/
#include <GMDS/IG/IGMesh.h>
#include <GMDS/IO/IReader.h>
/*----------------------------------------------------------------------------*/
namespace gmds{
/*----------------------------------------------------------------------------*/
#include "LimaReader_def.h"
/*----------------------------------------------------------------------------*/
template<typename TMesh>
LimaReader<TMesh>::LimaReader(TMesh& AMesh)
:IReader<TMesh>(AMesh), lenghtUnit_(1.)
{}

/*----------------------------------------------------------------------------*/
template<typename TMesh>
LimaReader<TMesh>::~LimaReader()
{}
/*----------------------------------------------------------------------------*/
template<typename TMesh>
double LimaReader<TMesh>::getLengthUnit()
{
	return lenghtUnit_;
}
/*----------------------------------------------------------------------------*/
template<typename TMesh>
void LimaReader<TMesh>::read(const std::string& AFileName, gmds::MeshModel AModel)
{
	Lima::Maillage m;
	try{

	m.lire(AFileName);
	} catch(...)
	{
		throw GMDSException("Lima cannot read the file "+AFileName);
	}
	lenghtUnit_ = m.unite_longueur();
	readNodes(m);
	if (this->mesh_.getModel().has(E) && AModel.has(E))
		readEdges(m);

	if (this->mesh_.getModel().has(F) && AModel.has(F))
		readFaces(m);

	if (this->mesh_.getModel().has(R) && AModel.has(R))
		readRegions(m);
}
/*----------------------------------------------------------------------------*/
template<typename TMesh>
void LimaReader<TMesh>::readNodes(Lima::Maillage& ALimaMesh)
{
	/** look for the highest node id*/
	size_t max_id=0;
	for(unsigned int i = 0; i<ALimaMesh.nb_noeuds(); i++)
		if(ALimaMesh.noeud(i).id()>max_id)
			max_id=ALimaMesh.noeud(i).id();

	nodes_connection_.resize(max_id);

	for(unsigned int i = 0; i < ALimaMesh.nb_noeuds(); i++)
	{
	      Lima::Noeud ni = ALimaMesh.noeud(i);
	      Node n = this->mesh_.newNode(ni.x(),ni.y(),ni.z());
	      nodes_connection_[ni.id()-1] = n;
	}

	for(unsigned int index=0;index<ALimaMesh.nb_nuages();index++)
	{
		Lima::Nuage lima_nuage = ALimaMesh.nuage(index);
		int nbNodesInCloud = lima_nuage.nb_noeuds();

		typename TMesh::cloud& cl = this->mesh_.newCloud(lima_nuage.nom());

		for(unsigned int node_index = 0; node_index<nbNodesInCloud;node_index++)
		{
			cl.add(nodes_connection_[lima_nuage.noeud(node_index).id()-1]);
		}
	}

}
/*----------------------------------------------------------------------------*/
template<typename TMesh>
void LimaReader<TMesh>::readEdges(Lima::Maillage& ALimaMesh)
{
	int max_id=0;
	for(unsigned int i = 0; i < ALimaMesh.nb_bras(); i++)
	{
		if(ALimaMesh.bras(i).id()>max_id)
				max_id=ALimaMesh.bras(i).id();
	}

	std::vector<Edge> edges_connection;
	edges_connection.resize(max_id);
	for(unsigned int i = 0; i < ALimaMesh.nb_bras(); i++)
	{
		Lima::Bras  b = ALimaMesh.bras(i);
		Edge e = this->mesh_.newEdge(nodes_connection_[b.noeud(0).id()-1],
								nodes_connection_[b.noeud(1).id()-1]);
		edges_connection[b.id()-1]=e;
	}

	for(unsigned int index=0;index<ALimaMesh.nb_lignes();index++)
	{
		Lima::Ligne lima_ligne = ALimaMesh.ligne(index);
		int nbEdgesInLine= lima_ligne.nb_bras();

		typename TMesh::line& li = this->mesh_.newLine(lima_ligne.nom());

		for(unsigned int edge_index = 0; edge_index<nbEdgesInLine;edge_index++)
		{
			li.add(edges_connection[lima_ligne.bras(edge_index).id()-1]);
		}
	}


}
/*----------------------------------------------------------------------------*/
template<typename TMesh>
void LimaReader<TMesh>::readFaces(Lima::Maillage& ALimaMesh)
{

	int max_id=0;
	for(unsigned int i = 0; i < ALimaMesh.nb_polygones(); i++)
	{
		if(ALimaMesh.polygone(i).id()>max_id)
				max_id=ALimaMesh.polygone(i).id();
	}

	std::vector<Face> faces_connection;
	faces_connection.resize(max_id);

	for(unsigned int i = 0; i < ALimaMesh.nb_polygones(); i++)
	{
		Lima::Polygone  p = ALimaMesh.polygone(i);
		Face f;
		switch(p.nb_noeuds()){
		case 3:
	        {
	        	f=this->mesh_.newTriangle(nodes_connection_[p.noeud(0).id()-1],
									nodes_connection_[p.noeud(1).id()-1],
									nodes_connection_[p.noeud(2).id()-1]);
	        }
	        break;
	      case 4:
	        {
	        	f=this->mesh_.newQuad(nodes_connection_[p.noeud(0).id()-1],
								nodes_connection_[p.noeud(1).id()-1],
								nodes_connection_[p.noeud(2).id()-1],
								nodes_connection_[p.noeud(3).id()-1]);
	        }
	        break;
	      case 5:
	        {
	        	std::vector<Node> nodes;
	        	nodes.resize(5);
	        	nodes[0] = nodes_connection_[p.noeud(0).id()-1];
	        	nodes[1] = nodes_connection_[p.noeud(1).id()-1];
	        	nodes[2] = nodes_connection_[p.noeud(2).id()-1];
	        	nodes[3] = nodes_connection_[p.noeud(3).id()-1];
	        	nodes[4] = nodes_connection_[p.noeud(4).id()-1];
	        	f=this->mesh_.newPolygon(nodes);
	        }
	        break;
	      case 6:
	        {
	        	std::vector<Node> nodes;
	        	nodes.resize(6);
	        	nodes[0] = nodes_connection_[p.noeud(0).id()-1];
	        	nodes[1] = nodes_connection_[p.noeud(1).id()-1];
	        	nodes[2] = nodes_connection_[p.noeud(2).id()-1];
	        	nodes[3] = nodes_connection_[p.noeud(3).id()-1];
	        	nodes[4] = nodes_connection_[p.noeud(4).id()-1];
	        	nodes[5] = nodes_connection_[p.noeud(5).id()-1];
	        	f=this->mesh_.newPolygon(nodes);
	        }
	        break;
	      }
		faces_connection[p.id()-1]=f;
	}

	for(unsigned int index=0;index<ALimaMesh.nb_surfaces();index++)
	{
		Lima::Surface lima_surf = ALimaMesh.surface(index);
		int nbFacesInSurf= lima_surf.nb_polygones();

		typename IGMesh::surface& su = this->mesh_.newSurface(lima_surf.nom());
		for(unsigned int face_index = 0; face_index<nbFacesInSurf;face_index++)
		{
			su.add(faces_connection[lima_surf.polygone(face_index).id()-1]);
		}
	}

}
/*----------------------------------------------------------------------------*/
template<typename TMesh>
void LimaReader<TMesh>::readRegions(Lima::Maillage& ALimaMesh)
{

	int max_id=0;
	for(unsigned int i = 0; i < ALimaMesh.nb_polyedres(); i++)
	{
		if(ALimaMesh.polyedre(i).id()>max_id)
				max_id=ALimaMesh.polyedre(i).id();
	}

	std::vector<Region> regions_connection;
	regions_connection.resize(max_id);

	for(unsigned int i = 0; i < ALimaMesh.nb_polyedres(); ++i)
	{
		Lima::Polyedre  p = ALimaMesh.polyedre(i);
		Region  r;
		switch(p.nb_noeuds()){
		case 4:
			{
				r=this->mesh_.newTet(nodes_connection_[p.noeud(0).id()-1],
							   nodes_connection_[p.noeud(1).id()-1],
							   nodes_connection_[p.noeud(2).id()-1],
							   nodes_connection_[p.noeud(3).id()-1]);
			}
			break;
		  case 5:
			{
				r=this->mesh_.newPyramid(nodes_connection_[p.noeud(0).id()-1],
								   nodes_connection_[p.noeud(1).id()-1],
							       nodes_connection_[p.noeud(2).id()-1],
							       nodes_connection_[p.noeud(3).id()-1],
							       nodes_connection_[p.noeud(4).id()-1]);
			}
			break;
		  case 6:
			{
				r=this->mesh_.newPrism3(nodes_connection_[p.noeud(0).id()-1],
							      nodes_connection_[p.noeud(1).id()-1],
							      nodes_connection_[p.noeud(2).id()-1],
							      nodes_connection_[p.noeud(3).id()-1],
							      nodes_connection_[p.noeud(4).id()-1],
								  nodes_connection_[p.noeud(5).id()-1]);
			}
			break;
		  case 8:
			{
				r=this->mesh_.newHex(nodes_connection_[p.noeud(0).id()-1],
							   nodes_connection_[p.noeud(1).id()-1],
							   nodes_connection_[p.noeud(2).id()-1],
							   nodes_connection_[p.noeud(3).id()-1],
							   nodes_connection_[p.noeud(4).id()-1],
							   nodes_connection_[p.noeud(5).id()-1],
							   nodes_connection_[p.noeud(6).id()-1],
							   nodes_connection_[p.noeud(7).id()-1]);
			}
			break;
		  case 10:
			{
				throw GMDSException("Prism5 type not yet implemented");
//				r=this->mesh_.newPrism5(nodes_connection_[p.noeud(0).id()-1],
//							      nodes_connection_[p.noeud(1).id()-1],
//							      nodes_connection_[p.noeud(2).id()-1],
//							      nodes_connection_[p.noeud(3).id()-1],
//							      nodes_connection_[p.noeud(4).id()-1],
//							      nodes_connection_[p.noeud(5).id()-1],
//							      nodes_connection_[p.noeud(6).id()-1],
//							      nodes_connection_[p.noeud(7).id()-1],
//							      nodes_connection_[p.noeud(8).id()-1],
//								  nodes_connection_[p.noeud(9).id()-1]);
			}
			break;
		  case 12:
			{
				throw GMDSException("Prism6 type not yet implemented");
//				r=this->mesh_.newPrism6(nodes_connection_[p.noeud(0).id()-1],
//							      nodes_connection_[p.noeud(1).id()-1],
//							      nodes_connection_[p.noeud(2).id()-1],
//							      nodes_connection_[p.noeud(3).id()-1],
//							      nodes_connection_[p.noeud(4).id()-1],
//							      nodes_connection_[p.noeud(5).id()-1],
//							      nodes_connection_[p.noeud(6).id()-1],
//							      nodes_connection_[p.noeud(7).id()-1],
//							      nodes_connection_[p.noeud(8).id()-1],
//							      nodes_connection_[p.noeud(9).id()-1],
//							      nodes_connection_[p.noeud(10).id()-1],
//								  nodes_connection_[p.noeud(11).id()-1]);
			}
			break;
		  }
		regions_connection[p.id()-1]=r;
	}

	for(unsigned int index=0;index<ALimaMesh.nb_volumes();index++)
	{
		Lima::Volume lima_vol = ALimaMesh.volume(index);
		int nbRegionsInVol= lima_vol.nb_polyedres();

		typename TMesh::volume& vo = this->mesh_.newVolume(lima_vol.nom());

		for(unsigned int r_index = 0; r_index<nbRegionsInVol;r_index++)
		{
			vo.add(regions_connection[lima_vol.polyedre(r_index).id()-1]);
		}
	}

}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
#endif /* LIMAREADER_H_ */
/*----------------------------------------------------------------------------*/

