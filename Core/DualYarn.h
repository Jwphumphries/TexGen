/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2022 Joseph Humphries

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
=============================================================================*/

#pragma once
#include "Yarn.h"
#include "YarnSection.h"
#include "Mesh.h"

namespace TexGen
{
	using namespace std;
	/// class used to create a representation of a core spun yarn
	class CLASS_DECLSPEC CDualYarn : public CYarn
	{
	public:
		CDualYarn(void);
		//CDualYarn(TiXmlElement &Element);
		~CDualYarn(void);

		//string GetYarnType() const { return "DualYarn"; }

		/// Assigns section to the inner and outer section of the dual yarn
		void AssignSection();

		/// Assigns the section to the outer section of the dual yarn
		void AssignSectionOuter(const CYarnSection &YarnSection);

		/// Used to overide the assignment of properties from CYarn when CDualYarn is called
		void AssignDefaults();

		/// Adds end caps to the outer yarn surface mesh
		void AddEndCapsToMeshOuter(CMesh &Mesh) const;

		/// Used to store the outer sections of the yarn
		CObjectContainer<CYarnSection> m_pYarnSectionOuter;

		/// Creates surface mesh for inner section using CYarn:: AddSurfaceToMesh then creates surface mesh for 
		/// the outer section according to Fig 7 of 'Geometric Modelling Core Spun Yarns in TexGen' and adds it to outersection surface mesh object
		bool AddSurfaceToMesh(CMesh &Mesh, CMesh &OuterMesh, bool bAddEndCaps = true) const;

		///Builds the inner yarn sections and outer yarn sections
		bool BuildSections() const;

		/// Builds section meshes for inner and outer sections
		bool BuildSectionMeshes() const;

		/// Creates volume mech for inner and outer sections of the dual yarn (WIP)
		bool AddVolumeToMesh(CMesh &Mesh, CMesh &OuterMesh) const;
	

	};

} // namespace TexGen
