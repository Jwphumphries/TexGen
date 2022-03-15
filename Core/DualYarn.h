
#pragma once
#include "Yarn.h"
#include "YarnSection.h"
#include "Mesh.h"

namespace TexGen
{
	using namespace std;

	class CLASS_DECLSPEC CDualYarn : public CYarn
	{
	public:
		CDualYarn(void);
		//CDualYarn(TiXmlElement &Element);
		~CDualYarn(void);

		//string GetYarnType() const { return "DualYarn"; }


		void AssignSection(const CYarnSection &YarnSection);
		void AssignDefaults();

		CObjectContainer<CYarnSection> m_pYarnSectionOuter;

		bool BuildSections() const;

	

	};

} // namespace TexGen
