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
#include "libmesh/cell_tet4.h"

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
  rbox.appendPoints("10");

  // Debugging: Try to decipher what these commands should be in the general case.
  libMesh::out << "rbox.comment()=" << rbox.comment() << std::endl;
  libMesh::out << "rbox.dimension()=" << rbox.dimension() << std::endl;
  libMesh::out << "rbox.count()=" << rbox.count() << std::endl;
  {
    const Real* coords = rbox.coordinates();
    unsigned ctr = 0;
    for (int i=0; i<rbox.dimension() * rbox.count(); i+=rbox.dimension())
      libMesh::out << "Input vertex " << ctr++ << ": " << coords[i] << " " << coords[i+1] << " " << coords[i+2] << std::endl;
  }

  Qhull qhull;
  try
    {
      qhull.runQhull(rbox, "");
    }
  catch (QhullError& e)
    {
      libmesh_error_msg("Qhull threw an exception: " << e.what());
    }



//  // Use the other runQhull interface.
//
//  // Take the points currently in the Mesh and use them for defining
//  // the convex hull...  TODO: handle the LIBMESH_DIM=2 case
//  // elegantly.
//
//  std::vector<Real> coords (_mesh.n_nodes() * 3);
//  {
//    MeshBase::node_iterator
//      it = _mesh.nodes_begin(),
//      end = _mesh.nodes_end();
//
//    unsigned ctr = 0;
//    for (; it != end; ++it)
//      {
//        Node* node = *it;
//        coords[ctr++] = (*node)(0);
//        coords[ctr++] = (*node)(1);
//        coords[ctr++] = (*node)(2);
//      }
//  }
//
//  // Try to mimic the input string from the example program, although this
//  // didn't seem to make much difference in the output.
//  std::ostringstream oss;
//  oss << "rbox " << "\"" << _mesh.n_nodes() << "\"";
//  // libMesh::out << "rboxCommand2 = " << oss.str() << std::endl;
//
//  // Qhull throws exceptions if there is an error, if they go uncaught
//  // the program dies with code 1, and without any other messages.  So
//  // let's be nice and catch them!
//  Qhull qhull;
//  try
//    {
//      qhull.runQhull(/*rboxCommand2=*/oss.str().c_str(),
//                     /*pointDimension=*/3,
//                     /*pointCount=*/_mesh.n_nodes(),
//                     /*rboxPoints=*/&coords[0],
//                     /*qhullCommand2=*/"");
//    }
//  catch (QhullError& e)
//    {
//      libmesh_error_msg("Qhull threw an exception: " << e.what());
//    }

  // Print some info about the convex hull that was generated
  libMesh::out << "Number of vertices in convex hull: " << qhull.vertexList().size() << std::endl;
  libMesh::out << "Number of facets in convex hull: " << qhull.facetList().size() << std::endl;

  // Print out the vertices - this print statement doesn't compile for me
  // QhullVertexList vertices = qhull.vertexList();
  // std::cout << vertices;

  // Print out the elements - this print statement doesn't compile for me
  // QhullFacetList facets = qhull.facetList();
  // std::cout << facets;

  // Remove existing points/elements from the Mesh, let it be
  // recreated by the data in the Qhull object.
  _mesh.clear();

  // Iterate over vertices
  {
    QhullVertex
      it = qhull.beginVertex(),
      end = qhull.endVertex();

    for (; it!=end; it=it.next())
      {
        // x, y, z coordinates
        QhullPoint pt = it.point();

        // Print vertex information
        libMesh::out << "Vertex " << it.id() << ": " << pt[0] << ", " << pt[1] << ", " << pt[2] << std::endl;

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
        // Status of this facet
        libMesh::out << "isDefined()=" << it.isDefined() << std::endl;
        libMesh::out << "isGood()=" << it.isGood() << std::endl;
        libMesh::out << "isSimplicial()=" << it.isSimplicial() << std::endl;
        libMesh::out << "isTopOrient()=" << it.isTopOrient() << std::endl;
        libMesh::out << "isTriCoplanar()=" << it.isTriCoplanar() << std::endl;
        libMesh::out << "isUpperDelaunay()=" << it.isUpperDelaunay() << std::endl;

        QhullVertexSet vset = it.vertices();
        int n_vertices = vset.count();

        // Qhull facets sometimes have > 3 vertices but aren't tets.  What are they?
        Elem* elem = NULL;

        switch (n_vertices)
          {
          case 3:
            {
              elem = new Tri3;
              break;
            }

          case 4:
            {
              elem = new Tet4;
              break;
            }

          default:
            libmesh_error_msg("Facet has invalid number of vertices = " << n_vertices);
          }

        // Set the qhull ID on this Elem
        elem->set_id() = it.id();

        QhullVertexSet::const_iterator
          vset_it = vset.begin(),
          vset_end = vset.end();

        libMesh::out << "Facet " << it.id() << " has " << n_vertices << " vertices: ";

        // Iterate over vertices
        unsigned ctr=0;
        for (; vset_it != vset_end; ++vset_it)
          {
            QhullVertex vertex = *vset_it;
            libMesh::out << vertex.id() << " ";

            // Set the Elem's node pointer.  FIXME: be more careful about NULL pointers.
            elem->set_node(ctr++) = _mesh.node_ptr(vertex.id());
          }
        libMesh::out << std::endl;

        // Finally, add the element to the Mesh
        _mesh.add_elem(elem);
      }
  }

  _mesh.prepare_for_use();
}

} // namespace libMesh

#endif // LIBMESH_HAVE_QHULL_API
