/*----------------------------------------------------------------------------*/
/** \file    LimaWriterAPI.cpp
 *  \author  legoff
 *  \date    30/06/2016
 */
/*----------------------------------------------------------------------------*/
#include "GMDSCEA/GMDSCEAWriter.h"
/*----------------------------------------------------------------------------*/
#include <vector>
/*----------------------------------------------------------------------------*/
#include <Lima/malipp.h>
#include <Lima/lima++.h>
#include <LimaP/reader.h>
#include "GMDSCEA/LimaWriter.h"
#include "GMDSCEA/LimaWriterAPI.h"
/*----------------------------------------------------------------------------*/
namespace gmds{
/*----------------------------------------------------------------------------*/
GMDSCEAWriter::GMDSCEAWriter(gmds::IGMesh& AMesh)
:mesh_(AMesh),lenghtUnit_(1.)
{

}
/*----------------------------------------------------------------------------*/
GMDSCEAWriter::~GMDSCEAWriter()
{

}
/*----------------------------------------------------------------------------*/
void
GMDSCEAWriter::setLengthUnit(double AUnit)
{
	lenghtUnit_ = AUnit
}
/*----------------------------------------------------------------------------*/
void
GMDSCEAWriter::write(const std::string& AFileName, gmds::MeshModel AModel, int ACompact)
{
	/* Detection du format par le suffixe du nom du fichier. */
	Lima::format_t format = Lima::SUFFIXE;
	format = Lima::_Reader::detectFormat(AFileName);

	switch(format) {
	case Lima::SUFFIXE :
		throw GMDSException("GMDSCEAWriter::write Extension de fichier inconnue\n");
		break;
	case Lima::INCONNU :
		throw GMDSException("GMDSCEAWriter::write Extension de fichier inconnue\n");
		break;
	case Lima::MALIPP2 :
	{
		try {
			gmds::LimaWriterAPI w(mesh_);
			w.setLengthUnit(lenghtUnit_);
			w.write(AFileName,AModel,ACompact);
		}
		catch(gmds::GMDSException& e) {
			std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
			throw GMDSException(e.what());
		}
	}
		break;
		//next option only available with hdf145
		/*	case Lima::MALIPP_Z :
	{
		try {
			gmds::LimaWriterAPI w(mesh_);
			w.setLengthUnit(lenghtUnit_);
			w.activateZlibCompression();
			w.write(AFileName,AModel,ACompact);
		}
		catch(gmds::GMDSException& e) {
			std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
			throw GMDSException(e.what());
		}
	}
		break;
		*/
	default :
	{
		try {
			gmds::LimaWriter<gmds::IGMesh> w(mesh_);
			w.setLengthUnit(lenghtUnit_);
			w.write(AFileName,AModel,ACompact);
		}
		catch(gmds::GMDSException& e) {
			std::cerr<<"GMDSCEA ERREUR : "<<e.what()<<std::endl;
			throw GMDSException(e.what());
		}
	}
		break;
	}
}
/*----------------------------------------------------------------------------*/
}  // namespace gmds
/*----------------------------------------------------------------------------*/
