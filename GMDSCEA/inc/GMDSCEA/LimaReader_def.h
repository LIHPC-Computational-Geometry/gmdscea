/*----------------------------------------------------------------------------*/
/** \file    LimaReader_def.h
 *  \author  F. LEDOUX
 *  \date    09/09/2008
 */
/*----------------------------------------------------------------------------*/
template<typename TMesh>
class LimaReader:public IReader<TMesh>
{

public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructor.
     *
     *  \param AMesh the mesh in which we want to copy the content of a Lima
     *  	   file.
     */
	LimaReader(TMesh& AMesh);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructor.	*/
	virtual ~LimaReader();

	/*------------------------------------------------------------------------*/
	/** \brief  Set the mesh length unit. It is the conversion factor from meters
	 */
	double getLengthUnit();

    /*------------------------------------------------------------------------*/
    /** \brief  Read the content of the file named outputName_ and write it in
     *   		mesh_.
     */
	void read(const std::string& AFileName, gmds::MeshModel AModel);

protected:

	void readNodes(Lima::Maillage& ALimaMesh);
	void readEdges(Lima::Maillage& ALimaMesh);
	void readFaces(Lima::Maillage& ALimaMesh);
	void readRegions(Lima::Maillage& ALimaMesh);


	/* connection between original nodes ID and GMDS nodes */
	std::vector<Node> nodes_connection_;

	/* length unit */
	double lenghtUnit_;
};
/*----------------------------------------------------------------------------*/



