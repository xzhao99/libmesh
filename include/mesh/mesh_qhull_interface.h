// The libMesh Finite Element Library.
// Copyright (C) 2002-2014 Benjamin S. Kirk, John W. Peterson, Roy H. Stogner

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#ifndef LIBMESH_MESH_QHULL_INTERFACE_H
#define LIBMESH_MESH_QHULL_INTERFACE_H


#include "libmesh/libmesh_config.h"

#ifdef LIBMESH_HAVE_QHULL_API

// Local Includes
#include "libmesh/enum_elem_type.h"
#include "libmesh/libmesh.h"
#include "libmesh/mesh_serializer.h"

// C++ includes
#include <cstddef>
#include <vector>

namespace libMesh
{

// Forward Declarations

class UnstructuredMesh;

/**
 * A libMesh interface to the Qhull library.
 */
class QhullInterface
{
public:
  /**
   * The constructor.  A reference to the mesh containing the points
   * which are to be triangulated must be provided.  Unless otherwise
   * specified, a convex hull will be computed for the set of input points
   * and the convex hull will be meshed.
   */
  explicit
  QhullInterface(UnstructuredMesh& mesh);

  /**
   * Empty destructor.
   */
  ~QhullInterface() {}

  /**
   * This is the main public interface for this class.
   */
  void triangulate();

private:
  /**
   * Reference to the mesh which is to be created by Qhull.
   */
  UnstructuredMesh& _mesh;

  /**
   * Qhull only operates on serial meshes.
   */
  MeshSerializer _serializer;
};

} // namespace libMesh



#endif // LIBMESH_HAVE_QHULL_API

#endif // ifndef LIBMESH_MESH_QHULL_INTERFACE_H
