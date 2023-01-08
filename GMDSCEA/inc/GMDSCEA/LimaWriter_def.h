/*----------------------------------------------------------------------------*/
/** \file    LimaWriter.h
 *  \author  F. LEDOUX
 *  \date    08/08/2008
 */
/*----------------------------------------------------------------------------*/
template<typename TMesh>
class LimaWriter{
public:

    /*------------------------------------------------------------------------*/
    /** \brief  Constructor.
     *
     *  \param AMesh the mesh we want to write into a file.
     */
	LimaWriter(TMesh& AMesh);

    /*------------------------------------------------------------------------*/
    /** \brief  Destructor.	*/
	virtual ~LimaWriter();

	/*------------------------------------------------------------------------*/
	/** \brief  Set the mesh length unit. It is the conversion factor from meters
	 */
	void setLengthUnit(double AUnit);

    /*------------------------------------------------------------------------*/
    /** \brief  Write the content of mesh_ into the file named AFileName.
     */
	void write(const std::string& AFileName, gmds::MeshModel AModel, int ACompact=false);

protected:

	void writeNodes(Lima::Maillage& ALimaMesh);
	void writeEdges(Lima::Maillage& ALimaMesh);
	void writeFaces(Lima::Maillage& ALimaMesh);
	void writeRegions(Lima::Maillage& ALimaMesh);

	/* a mesh */
	TMesh& mesh_;

	/* connection between original nodes ID and lima nodes */
	std::vector<Lima::Noeud> nodes_connection_;

	/* length unit */
	double lenghtUnit_;
};
/*----------------------------------------------------------------------------*/
