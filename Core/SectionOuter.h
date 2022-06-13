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
#include "Section.h"

namespace TexGen
{
	using namespace std;

	/// a class created to represent the outer cross-section of a core spun yarn
	class CLASS_DECLSPEC CSectionOuter : public CSection
	{
	public:
		CSectionOuter(double dWidth, double dHeight);
		//CSectionOuter(TiXmlElement &Element);
		~CSectionOuter(void);
		
		bool operator == (const CSection &CompareMe) const;
		CSection* Copy() const { return new CSectionOuter(*this); }

		//void PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const;

		string GetType() const { return "CSectionOuter"; }
		string GetDefaultName() const;

		XY GetPoint(double t) const;


		/// Accessor methods
		double GetWidth() const { return m_dWidth; }
		double GetHeight() const { return m_dHeight; }
		void SetWidth(double dWidth) { m_dWidth = dWidth; }


	protected:
		double m_dWidth, m_dHeight;



	};

};	// namespace TexGen