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


#include "libmesh/libmesh_config.h"

#ifdef LIBMESH_HAVE_QHULL_API

// C/C++ includes
#include <sstream>

#include "libmesh/mesh_qhull_interface.h"
#include "libmesh/unstructured_mesh.h"
#include "libmesh/point.h"
#include "libmesh/face_tri3.h"

// Qhull headers

// The Qhull C++ classes require dynamic memory allocation, so this
// constant must be set.  Note that we do set it when compiling the
// Qhull library, but we also need to set it when including Qhull
// headers from libmesh (or compile libmesh with -Dqh_QHpointer?).
#ifndef qh_QHpointer
  #define qh_QHpointer 1
#endif

#include "Qhull.h"
#include "RboxPoints.h"
#include "QhullFacetList.h"
#include "QhullVertex.h"
#include "QhullVertexSet.h"

// Bring in the Qhull namespace
using namespace orgQhull;

namespace libMesh
{

// Constructor
QhullInterface::QhullInterface(UnstructuredMesh& mesh)
  : _mesh(mesh),
    _serializer(_mesh)
{}



// Primary function responsible for performing the triangulation
void QhullInterface::triangulate()
{
  // Taken directly from Qhull's user_eg3 program
  RboxPoints rbox;
  rbox.appendPoints("100");
  Qhull qhull;
  qhull.runQhull(rbox, "");

  // Print out the vertices
  // QhullVertexList vertices = qhull.vertexList();
  // libMesh::out << vertices;

  // Print out the elements
  // QhullFacetList facets = qhull.facetList();
  // libMesh::out << facets;

  // Iterate over vertices
  {
    QhullVertex
      it = qhull.beginVertex(),
      end = qhull.endVertex();

    for (; it!=end; it=it.next())
      {
        // libMesh::out << "it.id()=" << it.id() << std::endl;

        // x, y, z coordinates
        QhullPoint pt = it.point();
        // libMesh::out << pt << std::endl;
        // libMesh::out << pt[0] << ", " << pt[1] << ", " << pt[2] << std::endl;

        // Add the point to the libmesh mesh with the Qhull id
        _mesh.add_point(Point(pt[0], pt[1], pt[2]), it.id());
      }
  }

  // Iterate over facets
  {
    QhullFacet
      it = qhull.beginFacet(),
      end = qhull.endFacet();

    for (; it!=end; it=it.next())
      {
        // Construct a new Tri3 element... need to generalize to 3D?

        // Elem* elem = Elem::build(TRI3).release();
        Elem* elem = new Tri3;

        // libMesh::out << "Facet " << it.id() << " has vertices: ";

        // Set the qhull ID on this Elem
        elem->set_id() = it.id();

        QhullVertexSet vset = it.vertices();
        // libMesh::out << "vset has " << vset.count() << " entries." << std::endl;

        QhullVertexSet::const_iterator
          vset_it = vset.begin(),
          vset_end = vset.end();

        // Iterate over vertices
        unsigned ctr=0;
        for (; vset_it != vset_end; ++vset_it)
          {
            QhullVertex vertex = *vset_it;
            // libMesh::out << vertex.id() << " ";

            // Set the Elem's node pointer.  FIXME: be more careful about NULL pointers.
            elem->set_node(ctr++) = _mesh.node_ptr(vertex.id());
          }
        // libMesh::out << std::endl;

        // Finally, add the element to the Mesh
        _mesh.add_elem(elem);
      }
  }

  _mesh.prepare_for_use();
}

} // namespace libMesh

#endif // LIBMESH_HAVE_QHULL_API
