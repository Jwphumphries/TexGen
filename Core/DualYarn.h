
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

	
		void AssignSection();
		void AssignSectionOuter(const CYarnSection &YarnSection);
		void AssignDefaults();
		void AddEndCapsToMeshOuter(CMesh &Mesh) const;

		CObjectContainer<CYarnSection> m_pYarnSectionOuter;

		bool AddSurfaceToMesh(CMesh &Mesh, CMesh &OuterMesh, bool bAddEndCaps = true) const;
		bool BuildSections() const;

	

	};

} // namespace TexGen
