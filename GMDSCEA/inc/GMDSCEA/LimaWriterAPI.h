/*----------------------------------------------------------------------------*/
/** \file    LimaWriterAPI.h
 *  \author  legoff
 *  \date    30/06/2016
 */
/*----------------------------------------------------------------------------*/
#ifndef GMDS_LIMAWRITERAPI_H_
#define GMDS_LIMAWRITERAPI_H_
/*----------------------------------------------------------------------------*/
#include <Lima/malipp2.h>
#include <GMDS/IG/IG.h>
/*----------------------------------------------------------------------------*/
namespace gmds{
/*----------------------------------------------------------------------------*/
class LimaWriterAPI{
public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructor.
     *
     *  \param AMesh the mesh we want to write into a file.
     */
	LimaWriterAPI(gmds::IGMesh& AMesh);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructor.	*/
	virtual ~LimaWriterAPI();

	/*------------------------------------------------------------------------*/
	/** \brief  Set the mesh length unit. It is the conversion factor from meters
	 */
	void setLengthUnit(double AUnit);

    /*------------------------------------------------------------------------*/
    /** \brief  Write the content of mesh_ into the file named AFileName.
     */
	void write(const std::string& AFileName, gmds::MeshModel AModel, int ACompact=false);

	/*------------------------------------------------------------------------*/
	/** \brief  Activate the zlib compression.
	 */
	void activateZlibCompression();

protected:

	void writeNodes();
	void writeEdges();
	void writeFaces();
	void writeRegions();

	void writeClouds();
	void writeLines();
	void writeSurfaces();
	void writeVolumes();

	void writeNodesAttributes();
	void writeEdgesAttributes();
	void writeFacesAttributes();
	void writeRegionsAttributes();

	void writeCloudsAttributes();
	void writeLinesAttributes();
	void writeSurfacesAttributes();
	void writeVolumesAttributes();

	/* a mesh */
	gmds::IGMesh& mesh_;

	/* connection between original nodes ID and lima nodes */
//	std::vector<Lima::Noeud> nodes_connection_;

	/* length unit */
	double lenghtUnit_;

	Lima::MaliPPWriter2* writer_;
};
/*----------------------------------------------------------------------------*/
}  // namespace gmds
/*----------------------------------------------------------------------------*/
#endif  // GMDS_LIMAWRITERAPI_H_
/*----------------------------------------------------------------------------*/
