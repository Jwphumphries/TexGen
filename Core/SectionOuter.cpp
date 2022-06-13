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

#include "PrecompiledHeaders.h"
#include "SectionOuter.h"
using namespace TexGen;

CSectionOuter::CSectionOuter(double dWidth, double dHeight)
	: m_dWidth(dWidth)
	, m_dHeight(dHeight)
{
}

CSectionOuter::~CSectionOuter(void)
{
}

bool CSectionOuter::operator == (const CSection &CompareMe) const
{
	if (CompareMe.GetType() != GetType())
		return false;
	return m_dWidth == ((CSectionOuter*)&CompareMe)->m_dWidth &&
		m_dHeight == ((CSectionOuter*)&CompareMe)->m_dHeight;
}
/*
CSectionOuter::CSectionOuter(TiXmlElement &Element)
	: CSection(Element)
	, m_dWidth(0)
	, m_dHeight(0)
{
	Element.Attribute("Width", &m_dWidth);
	Element.Attribute("Height", &m_dHeight);
}

void CSectionOuter::PopulateTiXmlElement(TiXmlElement &Element, OUTPUT_TYPE OutputType) const
{
	CSection::PopulateTiXmlElement(Element, OutputType);
	Element.SetAttribute("Width", stringify(m_dWidth));
	Element.SetAttribute("Height", stringify(m_dHeight));
}
*/
XY CSectionOuter::GetPoint(double t) const
{
	return XY(0.5*m_dWidth*cos(t * 2 * PI), 0.5*m_dHeight*sin(t * 2 * PI));
}

string CSectionOuter::GetDefaultName() const
{
	return "Outer(W:" + stringify(m_dWidth) + ",H:" + stringify(m_dHeight) + ")";
}