/*----------------------------------------------------------------------------*/
/** \file    LimaWriterAPI.h
 *  \author  legoff
 *  \date    11/07/2016
 */
/*----------------------------------------------------------------------------*/
#ifndef GMDS_GMDSCEAWRITER_H_
#define GMDS_GMDSCEAWRITER_H_
/*----------------------------------------------------------------------------*/
#include <GMDS/IG/IG.h>
/*----------------------------------------------------------------------------*/
namespace gmds{
/*----------------------------------------------------------------------------*/
class GMDSCEAWriter{
public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructor.
     *
     *  \param AMesh the mesh we want to write into a file.
     */
	GMDSCEAWriter(gmds::IGMesh& AMesh);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructor.	*/
	virtual ~GMDSCEAWriter();

	/*------------------------------------------------------------------------*/
	/** \brief  Set the mesh length unit. It is the conversion factor from meters
	 */
	void setLengthUnit(double AUnit);

    /*------------------------------------------------------------------------*/
    /** \brief  Write the content of mesh_ into the file named AFileName.
     */
	void write(const std::string& AFileName, gmds::MeshModel AModel, int ACompact=false);

protected:

	/* a mesh */
	gmds::IGMesh& mesh_;

	/* length unit */
	double lenghtUnit_;
};
/*----------------------------------------------------------------------------*/
}  // namespace gmds
/*----------------------------------------------------------------------------*/
#endif  // GMDS_GMDSCEAWRITER_H_
/*----------------------------------------------------------------------------*/
