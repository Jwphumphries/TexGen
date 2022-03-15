
#include "PrecompiledHeaders.h"
#include "DualYarn.h"
#include "SectionEllipse.h"
#include "SectionOuter.h"
//#include "Section.h"

using namespace TexGen;

CDualYarn::CDualYarn(void)
{
	YarnType = "DualYarn";
	AssignDefaults();

}
/*
CDualYarn::CDualYarn(TiXmlElement &Element)
{

}
*/
CDualYarn::~CDualYarn(void)
{

}

void CDualYarn::AssignDefaults()
{
	
	
};

void CDualYarn::AssignSection(const CYarnSection &YarnSection)
{
	CYarn::AssignSection(YarnSection);

}

bool CDualYarn::BuildSections() const
{
	CYarn::BuildSections();

	return true;
}





