#include "Albany_SimOutput.hpp"

#include <SimField.h>
#include <SimModel.h>
#include <SimPartitionedMesh.h>
#include <apfSIM.h>

namespace Albany {

static std::string
removeExtension(std::string const& s)
{
  std::size_t pos = s.find_last_of('.');
  if (pos == std::string::npos) { return s; }
  return s.substr(0, pos);
}

SimOutput::SimOutput(
    const Teuchos::RCP<APFMeshStruct>&      meshStruct_,
    const Teuchos::RCP<const Teuchos_Comm>& comm_)
    : comm(comm_), index(1)
{
  filename = removeExtension(meshStruct_->outputFileName);
  mesh     = dynamic_cast<apf::MeshSIM*>(meshStruct_->getMesh());
}

void
SimOutput::writeFile(const double time_value)
{
  (void)time_value;
  assert(comm->getSize() == 1);
  pParMesh    spm = mesh->getMesh();
  pMesh       sm  = PM_mesh(spm, 0);
  std::string meshname;
  {
    std::stringstream ss;
    ss << filename << '_' << index << ".sms";
    meshname = ss.str();
  }
  PM_write(spm, meshname.c_str(), 0);
  for (int i = 0; i < mesh->countFields(); ++i) {
    apf::Field* field = mesh->getField(i);
    /* this is a cheap filter for non-nodal fields */
    if (apf::getShape(field) != mesh->getShape()) continue;
    pField      sf = apf::getSIMField(field);
    std::string fieldname;
    {
      std::stringstream ss;
      ss << filename << '_' << apf::getName(field) << '_' << index << ".fld";
      fieldname = ss.str();
    }
    Field_write(sf, fieldname.c_str(), 0, 0, 0);
  }
  ++index;
}

void
SimOutput::setFileName(const std::string& fname)
{
  filename = removeExtension(fname);
}

}  // namespace Albany
