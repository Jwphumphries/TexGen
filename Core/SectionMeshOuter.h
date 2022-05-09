#pragma once
#include "SectionMesh.h"


namespace TexGen
{
	using namespace std;

	/// Create a rectangular mesh, the number of layers can be specified or set as -1 for automatic determination
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

		bool CreateMesh(const vector<XY> &Section) const;
		//int CalculateNumberofLayers(const vector<XY> &Section) const;
		bool CreateSingleLayerMesh(const vector<XY> &Section) const;

		int m_iNumLayers;
		bool m_bTriangleCorners;
	};

}	// namespace TexGen