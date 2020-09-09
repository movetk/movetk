/*
 * Copyright (C) 2018-2020 HERE Europe B.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 * License-Filename: LICENSE
 */


//
// Created by Mitra, Aniket on 04/01/2019.
//

#ifndef MOVETK_DOCUMENTATION_H
#define MOVETK_DOCUMENTATION_H

/*!
 * \mainpage  The Movement Toolkit
 * @imageSize{movetk_structure.jpeg,height:570px;width:800px;}
  \image html movetk_structure.jpeg
 */

/*! \page GeomBackendDesign On the design of a geometry backend agnostic library
  \section sec Design Goals
  A movement toolkit such as \b movetk relies on a geometric library.
  A number of standard open source libraries are available. The most well known
  among them being (a) \b CGAL (b) \b Boost.Geometry (c) \b GDAL.
  However, each of
  \arg \b Different \b Interfaces: these libraries  have their own interface for construction of
  geometric objects (points, lines, segments etc.).
  \arg \b Coverage \b of \b functionality: these libraries like CGAL provide a comprehensive set of geometric algorithms on these geometric
  types.
  \arg \b Dimension \b Agnostic:
  \arg \b Licensing \b Agreements: Each of these geometric libraries have licences that range from
  open access to restrictive access.

  Similarly, the choice of the geometry backend also depends on the scope of
  usage , for example, for basic geometric objects, a library like GDAL / Boost.Geometry
  should suffice.  In constrast, for advanced usage, a library like CGAL would be better
  suited.

  Therefore, one of the design decisions of movetk
  is to let the user choose their own geometry backend. This in turn requires designing
  \arg A generic interface for different geometric objects
  \arg

  @imageSize{geomety_agnostic_backend_main_design.jpeg,height:370px;width:570px;}
  \image html geomety_agnostic_backend_main_design.jpeg
  \subsection subsection1 Design Patterns

  \subsection subsection2 The Interface
  Text.
  \subsection subsection3 The CGAL Adapter
  More text.
  * \cite HHKPS01 \cite Fabri2000OnTD
  *
  Even more text \cite BB2009 \cite BBM2010
*/



#endif //MOVETK_DOCUMENTATION_H
