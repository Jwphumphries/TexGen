
#include "PrecompiledHeaders.h"
#include "DualYarn.h"
#include "SectionEllipse.h"

using namespace TexGen;

CDualYarn::CDualYarn(void)
{
	AssignSection();
}
/*
CDualYarn::CDualYarn(TiXmlElement &Element)
{

}
*/
CDualYarn::~CDualYarn(void)
{

}

void CDualYarn::AssignSection()
{
	CYarn::AssignSection(CYarnSectionConstant(CSectionEllipse(2, 2)));
}


