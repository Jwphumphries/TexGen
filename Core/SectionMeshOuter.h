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
#include "SectionMesh.h"


namespace TexGen
{
	using namespace std;

	/// Create a rectangular mesh for the outer sections of the dual yarn
	class CLASS_DECLSPEC CSectionMeshOuter : public CSectionMesh
	{
	public:
		CSectionMeshOuter(int iNumLayers = -1, bool bTriangleCorners = true);
		//CSectionMeshOuter(TiXmlElement &Element);
		~CSectionMeshOuter(void);

		CSectionMesh* Copy() const { return new CSectionMeshOuter(*this); }

		string GetType() const { return "CSectionMeshOuter"; }

		//void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		void SetNumLayers(int iNum);

		int GetNumLayers() const { return m_iNumLayers; }

		int CalculateNumberofLayers(const vector<XY> &Section) const;

		/// To be used to creat the section mesh for each section according to Fig 8. in 'Geometric Modelling Core Spun Yarns in TexGen'
		bool CreateMesh(const vector<XY> &Section) const;

		bool CreateSingleLayerMesh(const vector<XY> &Section) const;

		int m_iNumLayers;

		bool m_bTriangleCorners;
	};

}	// namespace TexGen