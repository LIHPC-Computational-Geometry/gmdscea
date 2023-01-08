/*----------------------------------------------------------------------------*/
/** \file    LimaWriter.t.h
 *  \author  F. LEDOUX
 *  \date    08/08/2008
 */
/*----------------------------------------------------------------------------*/
#ifndef GMDS_LIMAWRITER_H_
#define GMDS_LIMAWRITER_H_
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include "Lima/lima++.h"
#include <GMDS/IG/IG.h>
/*----------------------------------------------------------------------------*/
namespace gmds{
/*----------------------------------------------------------------------------*/
#include "LimaWriter_def.h"
/*----------------------------------------------------------------------------*/
template<typename TMesh>
LimaWriter<TMesh>::LimaWriter(TMesh& AMesh)
:mesh_(AMesh), nodes_connection_(0),lenghtUnit_(1.)
{}
/*----------------------------------------------------------------------------*/
template<typename TMesh>
LimaWriter<TMesh>::~LimaWriter()
{
}
/*----------------------------------------------------------------------------*/
template<typename TMesh>
void LimaWriter<TMesh>::setLengthUnit(double AUnit)
{
	lenghtUnit_ = AUnit;
}
/*----------------------------------------------------------------------------*/
template<typename TMesh>
void LimaWriter<TMesh>::write(const std::string& AFileName, gmds::MeshModel AModel, int ACompact)
{

	nodes_connection_.clear();

	typename TMesh::node_iterator it_nodes     = mesh_.nodes_begin();

	TCellID max_id = 0;
	for(;!it_nodes.isDone();it_nodes.next())
	{
		Node n = it_nodes.value();
		if(n.getID()>max_id)
			max_id = n.getID();
	}
	nodes_connection_.resize(max_id+1);

	Lima::Maillage m;
	m.unite_longueur(lenghtUnit_);

	writeNodes(m);

	if (AModel.has(E) && mesh_.getModel().has(E))
		writeEdges(m);

	if (AModel.has(F) && mesh_.getModel().has(F))
		writeFaces(m);

	if (AModel.has(R) && mesh_.getModel().has(R))
		writeRegions(m);

	try{
		if(mesh_.getModel().has(DIM2)){
			m.dimension(Lima::D2);
		}
		m.ecrire(AFileName, Lima::SUFFIXE,1,ACompact);
	}
	catch(Lima::write_erreur& e)
	{
		std::cerr<<"LIMA ERREUR : "<<e.what()
				<<"\nCela peut venir d'un chemin incorrect, d'un problème de permissions ou de quota."
				<<std::endl;
		throw GMDSException(e.what());
	}
}
/*----------------------------------------------------------------------------*/
template<typename TMesh>
void LimaWriter<TMesh>::writeNodes(Lima::Maillage& ALimaMesh)
{
	typename TMesh::node_iterator it_nodes     = mesh_.nodes_begin();

	for(;!it_nodes.isDone();it_nodes.next())
	{
		Node n = it_nodes.value();
		Lima::Noeud n2(n.getID()+1,n.X(), n.Y(), n.Z());
		/* we keep the connection between lima node and gmds node through local ids.
		 */
		ALimaMesh.ajouter(n2);
		nodes_connection_[n.getID()] = n2;

	}

	typename TMesh::clouds_iterator it_clouds = mesh_.clouds_begin();
	for(;it_clouds!=mesh_.clouds_end();it_clouds++)
	{

		typename TMesh::cloud& cl = *it_clouds;
		Lima::Nuage lima_cl(cl.name());
		ALimaMesh.ajouter(lima_cl);

		std::vector<Node> nodes= cl.cells();
		for(unsigned int node_index=0; node_index<nodes.size();node_index++)
	        lima_cl.ajouter(nodes_connection_[nodes[node_index].getID()]);
	}


}
/*----------------------------------------------------------------------------*/
template<typename TMesh>
void LimaWriter<TMesh>::writeEdges(Lima::Maillage& ALimaMesh)
{
	typename TMesh::edge_iterator it_edges     = mesh_.edges_begin();

	for(;!it_edges.isDone();it_edges.next())
	{
		Edge e = it_edges.value();
		std::vector<TCellID> nodes = e.getIDs<Node>();
		Lima::Noeud n1 = nodes_connection_[nodes[0]];
		Lima::Noeud n2 = nodes_connection_[nodes[1]];
		Lima::Bras e2(e.getID()+1,n1, n2);
		ALimaMesh.ajouter(e2);
	}

	typename TMesh::lines_iterator it_lines = mesh_.lines_begin();
	for(;it_lines!=mesh_.lines_end();it_lines++)
	{

		typename TMesh::line& li = *it_lines;
		Lima::Ligne lima_li(li.name());
		ALimaMesh.ajouter(lima_li);

		std::vector<Edge> edges= li.cells();
		for(unsigned int index=0; index<edges.size();index++){
			Lima::Bras b = ALimaMesh.bras_id(edges[index].getID()+1);
	        lima_li.ajouter(b);
		}
	}


}
/*----------------------------------------------------------------------------*/
template<typename TMesh>
void LimaWriter<TMesh>::writeFaces(Lima::Maillage& ALimaMesh)
{
	typename TMesh::face_iterator it     = mesh_.faces_begin();

	for(;!it.isDone();it.next())
	{
		Face f = it.value();
		std::vector<TCellID> nodes = f.getIDs<Node>();
		switch(f.getType()){
		case GMDS_QUAD:{
			Lima::Noeud n1 = nodes_connection_[nodes[0]];
			Lima::Noeud n2 = nodes_connection_[nodes[1]];
			Lima::Noeud n3 = nodes_connection_[nodes[2]];
			Lima::Noeud n4 = nodes_connection_[nodes[3]];
			Lima::Polygone f2(f.getID()+1,n1, n2,n3,n4);
			ALimaMesh.ajouter(f2);}
			break;
		case GMDS_TRIANGLE:{
			Lima::Noeud n1 = nodes_connection_[nodes[0]];
			Lima::Noeud n2 = nodes_connection_[nodes[1]];
			Lima::Noeud n3 = nodes_connection_[nodes[2]];

			Lima::Polygone f2(f.getID()+1,n1, n2,n3);
			ALimaMesh.ajouter(f2);}
			break;
		case GMDS_POLYGON:{
				switch(nodes.size()){
				case 3:
				{
					Lima::Noeud n1 = nodes_connection_[nodes[0]];
					Lima::Noeud n2 = nodes_connection_[nodes[1]];
					Lima::Noeud n3 = nodes_connection_[nodes[2]];
					Lima::Polygone f2(f.getID()+1,n1, n2, n3);
					ALimaMesh.ajouter(f2);
				}
				break;
				case 4:
				{
					Lima::Noeud n1 = nodes_connection_[nodes[0]];
					Lima::Noeud n2 = nodes_connection_[nodes[1]];
					Lima::Noeud n3 = nodes_connection_[nodes[2]];
					Lima::Noeud n4 = nodes_connection_[nodes[3]];
					Lima::Polygone f2(f.getID()+1,n1, n2, n3, n4);
					ALimaMesh.ajouter(f2);
				}
				break;
				case 5:
				{
					Lima::Noeud n1 = nodes_connection_[nodes[0]];
					Lima::Noeud n2 = nodes_connection_[nodes[1]];
					Lima::Noeud n3 = nodes_connection_[nodes[2]];
					Lima::Noeud n4 = nodes_connection_[nodes[3]];
					Lima::Noeud n5 = nodes_connection_[nodes[4]];
					Lima::Polygone f2(f.getID()+1,n1, n2, n3, n4, n5);
					ALimaMesh.ajouter(f2);
				}
				break;
				case 6:
				{
					Lima::Noeud n1 = nodes_connection_[nodes[0]];
					Lima::Noeud n2 = nodes_connection_[nodes[1]];
					Lima::Noeud n3 = nodes_connection_[nodes[2]];
					Lima::Noeud n4 = nodes_connection_[nodes[3]];
					Lima::Noeud n5 = nodes_connection_[nodes[4]];
					Lima::Noeud n6 = nodes_connection_[nodes[5]];
					Lima::Polygone f2(f.getID()+1,n1, n2, n3, n4, n5, n6);
					ALimaMesh.ajouter(f2);
				}
				break;
				default:
					std::cout<<"Unable to convert a polygon with more than 6 nodes in Lima format"<<std::endl;
				};
			}
			break;
		default:
			std::cout<<"Unable to convert this type of cell"<<std::endl;
		};

	}

	typename TMesh::surfaces_iterator it_surfs = mesh_.surfaces_begin();
	for(;it_surfs!=mesh_.surfaces_end();it_surfs++)
	{

		typename TMesh::surface& surf = *it_surfs;
		Lima::Surface lima_surf(surf.name());
		ALimaMesh.ajouter(lima_surf);

		std::vector<Face> faces = surf.cells();
		for(unsigned int face_index=0; face_index<faces.size();face_index++)
		{
			Lima::Polygone p = ALimaMesh.polygone_id(faces[face_index].getID()+1);
	        lima_surf.ajouter(p);
		}
	}


}
/*----------------------------------------------------------------------------*/
template<typename TMesh>
void LimaWriter<TMesh>::writeRegions(Lima::Maillage& ALimaMesh)
{
	typename TMesh::region_iterator it     = mesh_.regions_begin();
	for(;!it.isDone();it.next())
	{
		Region g = it.value();
		std::vector<TCellID> nodes = g.getIDs<Node>();
		switch(g.getType()){
		case GMDS_TETRA:{
			Lima::Noeud n1 = nodes_connection_[nodes[0]];
			Lima::Noeud n2 = nodes_connection_[nodes[1]];
			Lima::Noeud n3 = nodes_connection_[nodes[2]];
			Lima::Noeud n4 = nodes_connection_[nodes[3]];

			Lima::Polyedre f2(g.getID()+1,n1,n2,n3,n4);
			ALimaMesh.ajouter(f2);
		}
		break;
		case GMDS_HEX:{

			Lima::Noeud n1 = nodes_connection_[nodes[0]];
			Lima::Noeud n2 = nodes_connection_[nodes[1]];
			Lima::Noeud n3 = nodes_connection_[nodes[2]];
			Lima::Noeud n4 = nodes_connection_[nodes[3]];
			Lima::Noeud n5 = nodes_connection_[nodes[4]];
			Lima::Noeud n6 = nodes_connection_[nodes[5]];
			Lima::Noeud n7 = nodes_connection_[nodes[6]];
			Lima::Noeud n8 = nodes_connection_[nodes[7]];


			Lima::Polyedre f2(g.getID()+1,n1,n2,n3,n4,n5,n6,n7,n8);
			ALimaMesh.ajouter(f2);
		}
		break;
		case GMDS_PYRAMID:{

			Lima::Noeud n1 = nodes_connection_[nodes[0]];
			Lima::Noeud n2 = nodes_connection_[nodes[1]];
			Lima::Noeud n3 = nodes_connection_[nodes[2]];
			Lima::Noeud n4 = nodes_connection_[nodes[3]];
			Lima::Noeud n5 = nodes_connection_[nodes[4]];


			Lima::Polyedre f2(g.getID()+1,n1,n2,n3,n4,n5);
			ALimaMesh.ajouter(f2);
		}
		break;
		case GMDS_PRISM3:{

			Lima::Noeud n1 = nodes_connection_[nodes[0]];
			Lima::Noeud n2 = nodes_connection_[nodes[1]];
			Lima::Noeud n3 = nodes_connection_[nodes[2]];
			Lima::Noeud n4 = nodes_connection_[nodes[3]];
			Lima::Noeud n5 = nodes_connection_[nodes[4]];
			Lima::Noeud n6 = nodes_connection_[nodes[5]];

			Lima::Polyedre f2(g.getID()+1,n1,n2,n3,n4,n5,n6);
			ALimaMesh.ajouter(f2);
		}
		break;

		case GMDS_PRISM5:{

			Lima::Noeud n1 = nodes_connection_[nodes[0]];
			Lima::Noeud n2 = nodes_connection_[nodes[1]];
			Lima::Noeud n3 = nodes_connection_[nodes[2]];
			Lima::Noeud n4 = nodes_connection_[nodes[3]];
			Lima::Noeud n5 = nodes_connection_[nodes[4]];
			Lima::Noeud n6 = nodes_connection_[nodes[5]];
			Lima::Noeud n7 = nodes_connection_[nodes[6]];
			Lima::Noeud n8 = nodes_connection_[nodes[7]];
			Lima::Noeud n9 = nodes_connection_[nodes[8]];
			Lima::Noeud n10= nodes_connection_[nodes[9]];

			Lima::Polyedre f2(g.getID()+1,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10);
			ALimaMesh.ajouter(f2);
		}
		break;

		case GMDS_PRISM6:{

			Lima::Noeud n1 = nodes_connection_[nodes[0 ]];
			Lima::Noeud n2 = nodes_connection_[nodes[1 ]];
			Lima::Noeud n3 = nodes_connection_[nodes[2 ]];
			Lima::Noeud n4 = nodes_connection_[nodes[3 ]];
			Lima::Noeud n5 = nodes_connection_[nodes[4 ]];
			Lima::Noeud n6 = nodes_connection_[nodes[5 ]];
			Lima::Noeud n7 = nodes_connection_[nodes[6 ]];
			Lima::Noeud n8 = nodes_connection_[nodes[7 ]];
			Lima::Noeud n9 = nodes_connection_[nodes[8 ]];
			Lima::Noeud n10= nodes_connection_[nodes[9 ]];
			Lima::Noeud n11= nodes_connection_[nodes[10]];
			Lima::Noeud n12= nodes_connection_[nodes[11]];

			Lima::Polyedre f2(g.getID()+1,n1,n2,n3,n4,n5,n6,n7,n8,n9,n10,n11,n12);
			ALimaMesh.ajouter(f2);
		}
		break;
		case GMDS_POLYHEDRA:{
			switch(nodes.size()){
			case 4:
			{
				Lima::Noeud n1 = nodes_connection_[nodes[0]];
				Lima::Noeud n2 = nodes_connection_[nodes[1]];
				Lima::Noeud n3 = nodes_connection_[nodes[2]];
				Lima::Noeud n4 = nodes_connection_[nodes[3]];
				Lima::Polyedre f2(g.getID()+1,n1, n2, n3, n4);
				ALimaMesh.ajouter(f2);
			}
			break;
			case 5:
			{
				Lima::Noeud n1 = nodes_connection_[nodes[0]];
				Lima::Noeud n2 = nodes_connection_[nodes[1]];
				Lima::Noeud n3 = nodes_connection_[nodes[2]];
				Lima::Noeud n4 = nodes_connection_[nodes[3]];
				Lima::Noeud n5 = nodes_connection_[nodes[4]];
				Lima::Polyedre f2(g.getID()+1,n1, n2, n3, n4, n5);
				ALimaMesh.ajouter(f2);
			}
			break;
			case 6:
			{
				Lima::Noeud n1 = nodes_connection_[nodes[0]];
				Lima::Noeud n2 = nodes_connection_[nodes[1]];
				Lima::Noeud n3 = nodes_connection_[nodes[2]];
				Lima::Noeud n4 = nodes_connection_[nodes[3]];
				Lima::Noeud n5 = nodes_connection_[nodes[4]];
				Lima::Noeud n6 = nodes_connection_[nodes[5]];
				Lima::Polyedre f2(g.getID()+1,n1, n2, n3, n4, n5, n6);
				ALimaMesh.ajouter(f2);
			}
			break;
			case 8:
			{
				Lima::Noeud n1 = nodes_connection_[nodes[0]];
				Lima::Noeud n2 = nodes_connection_[nodes[1]];
				Lima::Noeud n3 = nodes_connection_[nodes[2]];
				Lima::Noeud n4 = nodes_connection_[nodes[3]];
				Lima::Noeud n5 = nodes_connection_[nodes[4]];
				Lima::Noeud n6 = nodes_connection_[nodes[5]];
				Lima::Noeud n7 = nodes_connection_[nodes[6]];
				Lima::Noeud n8 = nodes_connection_[nodes[7]];
				Lima::Polyedre f2(g.getID()+1,n1, n2, n3, n4 , n5 , n6, n7, n8);
				ALimaMesh.ajouter(f2);
			}
			break;
			case 10:
			{
				Lima::Noeud n1 = nodes_connection_[nodes[0 ]];
				Lima::Noeud n2 = nodes_connection_[nodes[1 ]];
				Lima::Noeud n3 = nodes_connection_[nodes[2 ]];
				Lima::Noeud n4 = nodes_connection_[nodes[3 ]];
				Lima::Noeud n5 = nodes_connection_[nodes[4 ]];
				Lima::Noeud n6 = nodes_connection_[nodes[5 ]];
				Lima::Noeud n7 = nodes_connection_[nodes[6 ]];
				Lima::Noeud n8 = nodes_connection_[nodes[7 ]];
				Lima::Noeud n9 = nodes_connection_[nodes[8 ]];
				Lima::Noeud n10= nodes_connection_[nodes[9 ]];
				Lima::Polyedre f2(g.getID()+1,n1, n2, n3, n4 , n5 , n6, n7, n8, n9, n10);
				ALimaMesh.ajouter(f2);
			}
			break;
			case 12:
			{
				Lima::Noeud n1 = nodes_connection_[nodes[0 ]];
				Lima::Noeud n2 = nodes_connection_[nodes[1 ]];
				Lima::Noeud n3 = nodes_connection_[nodes[2 ]];
				Lima::Noeud n4 = nodes_connection_[nodes[3 ]];
				Lima::Noeud n5 = nodes_connection_[nodes[4 ]];
				Lima::Noeud n6 = nodes_connection_[nodes[5 ]];
				Lima::Noeud n7 = nodes_connection_[nodes[6 ]];
				Lima::Noeud n8 = nodes_connection_[nodes[7 ]];
				Lima::Noeud n9 = nodes_connection_[nodes[8 ]];
				Lima::Noeud n10= nodes_connection_[nodes[9 ]];
				Lima::Noeud n11= nodes_connection_[nodes[10]];
				Lima::Noeud n12= nodes_connection_[nodes[11]];
				Lima::Polyedre f2(g.getID()+1,n1, n2, n3, n4 , n5 , n6,
								  n7, n8, n9, n10, n11, n12);
				ALimaMesh.ajouter(f2);
			}
			break;
			default:
				std::cout<<"Unable to convert a polyhedron with 7, 9 or more than 12 nodes in Lima format"<<std::endl;
			}
		}
		break;
		default:
			std::cout<<"Unable to convert a polyhedron with 7, 9 or more than 12 nodes in Lima format"<<std::endl;
		}
	}

	typename TMesh::volumes_iterator it_vols = mesh_.volumes_begin();
	for(;it_vols!=mesh_.volumes_end();it_vols++)
	{

		typename TMesh::volume& vol = *it_vols;
		Lima::Volume lima_vol(vol.name());
		ALimaMesh.ajouter(lima_vol);

		std::vector<Region> regions= vol.cells();
		for(unsigned int region_index=0; region_index<regions.size();region_index++)
		{
			Lima::Polyedre p = ALimaMesh.polyedre_id(regions[region_index].getID()+1);
	        lima_vol.ajouter(p);
		}
	}

}
/*----------------------------------------------------------------------------*/
}
/*----------------------------------------------------------------------------*/
#endif /* GMDS_LIMAWRITER_H_ */
/*----------------------------------------------------------------------------*/
