
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
	
	AssignSection();
};

void CDualYarn::AssignSection()
{
	CYarn::AssignSection(CYarnSectionConstant(CSectionEllipse(1, 0.5)));
	AssignSectionOuter(CYarnSectionConstant(CSectionEllipse(1.5, 1)));
}

void CDualYarn::AssignSectionOuter(const CYarnSection &YarnSection)
{
	if (m_pYarnSectionOuter != &YarnSection)
		m_pYarnSectionOuter = YarnSection;

	// When a new section is assigned the yarn needs to be rebuilt
	m_iNeedsBuilding = ALL;
}

bool CDualYarn::BuildSections() const
{
	CYarn::BuildSections();

	TGLOG("Building yarn sections");
	if (m_SlaveNodes.empty())
	{
		TGERROR("Unable to build sections, no slave nodes created");
		assert(false);
		return false;
	}
	if (!m_pYarnSectionOuter)
	{
		// Assign default circular section with diameter 1 tenth of the length of the yarn
/*		double dDiameter = GetRawYarnLength()/10;
		AssignSection(CYarnSectionConstant(CSectionEllipse(dDiameter, dDiameter)));*/
		TGERROR("Unable to build sections, no yarn section specified");
		assert(false);
		return false;
	}

	YARN_POSITION_INFORMATION YarnPositionInfo;
	YarnPositionInfo.SectionLengths = m_SectionLengths;

	bool bFirst = true;
	m_AABB = pair<XYZ, XYZ>(XYZ(), XYZ());
	//	vector<bool> SectionFirst;
	//	SectionFirst.resize(m_MasterNodes.size()-1, true);
	//	int iSectionMin, iSectionMax;

	vector<XY> Section;
	vector<XYZ>::const_iterator itPoint;
	vector<CSlaveNode>::iterator itSlaveNode;
	XYZ PrevPos = m_SlaveNodes[0].GetPosition();
	for (itSlaveNode = m_SlaveNodes.begin(); itSlaveNode != m_SlaveNodes.end(); ++itSlaveNode)
	{
		/*		iSectionMin = iSectionMax = itSlaveNode->GetIndex();

				if (itSlaveNode!=m_SlaveNodes.begin() && (itSlaveNode-1)->GetIndex() != iSectionMin)
					--iSectionMin;
				if ((itSlaveNode+1)!=m_SlaveNodes.end() && (itSlaveNode+1)->GetIndex() != iSectionMax)
					++iSectionMax;*/

		YarnPositionInfo.dSectionPosition = itSlaveNode->GetT();
		YarnPositionInfo.iSection = itSlaveNode->GetIndex();

		Section = m_pYarnSectionOuter->GetSection(YarnPositionInfo, m_iNumSectionPoints);
		itSlaveNode->UpdateSectionPointsOuter(&Section);

		PrevPos = itSlaveNode->GetPosition();

		for (itPoint = itSlaveNode->GetSectionPointsOuter().begin(); itPoint != itSlaveNode->GetSectionPointsOuter().end(); ++itPoint)
		{
			if (bFirst)
			{
				m_AABB.first = m_AABB.second = *itPoint;
				bFirst = false;
			}
			m_AABB.first = Min(m_AABB.first, *itPoint);
			m_AABB.second = Max(m_AABB.second, *itPoint);
			/*			for (iIndex = iSectionMin; iIndex <= iSectionMax; ++iIndex)
						{
							if (SectionFirst[iIndex])
							{
								m_SectionAABBs[iIndex].first = m_SectionAABBs[iIndex].second = *itPoint;
								SectionFirst[iIndex] = false;
							}
							m_SectionAABBs[iIndex].first = Min(m_SectionAABBs[iIndex].first, *itPoint);
							m_SectionAABBs[iIndex].second = Max(m_SectionAABBs[iIndex].second, *itPoint);
						}*/
		}
	}

	CreateSectionAABBs();
	/*	const double TOL = 1e-9;
		m_AABB.first.x -= TOL;
		m_AABB.first.y -= TOL;
		m_AABB.first.z -= TOL;
		m_AABB.second.x += TOL;
		m_AABB.second.y += TOL;
		m_AABB.second.z += TOL;

		vector<pair<XYZ, XYZ> >::iterator itSectionAABB;
		for (itSectionAABB = m_SectionAABBs.begin(); itSectionAABB != m_SectionAABBs.end(); ++itSectionAABB)
		{
			itSectionAABB->first.x -= TOL;
			itSectionAABB->first.y -= TOL;
			itSectionAABB->first.z -= TOL;
			itSectionAABB->second.x += TOL;
			itSectionAABB->second.y += TOL;
			itSectionAABB->second.z += TOL;
		}*/

		// Surface points are built
	m_iNeedsBuilding &= ALL ^ SURFACE;

	return true;

}

bool CDualYarn::BuildSectionMeshes() const
{

	CYarn::BuildSectionMeshes();

	TGLOG("Building yarn section meshes");
	if (m_SlaveNodes.empty())
	{
		TGERROR("Unable to build section meshes, no slave nodes created");
		assert(false);
		return false;
	}
	if (!m_pYarnSectionOuter)
	{
		TGERROR("Unable to build section meshes, no yarn section specified");
		assert(false);
		return false;
	}

	if (m_pYarnSectionOuter->GetForceMeshLayers())
	{
		m_pYarnSectionOuter->SetSectionMeshLayersEqual(m_iNumSectionPoints);
	}

	YARN_POSITION_INFORMATION YarnPositionInfo;
	YarnPositionInfo.SectionLengths = m_SectionLengths;

	vector<XY> Section;
	CMesh Mesh;
	vector<CSlaveNode>::iterator itSlaveNode;
	XYZ PrevPos = m_SlaveNodes[0].GetPosition();

	for (itSlaveNode = m_SlaveNodes.begin(); itSlaveNode != m_SlaveNodes.end(); ++itSlaveNode)
	{
		YarnPositionInfo.dSectionPosition = itSlaveNode->GetT();
		YarnPositionInfo.iSection = itSlaveNode->GetIndex();

		Mesh = m_pYarnSectionOuter->GetSectionMeshOuter(YarnPositionInfo, m_iNumSectionPoints, m_bEquiSpacedSectionMesh);
		if (Mesh.GetNumNodes() == 0)
			return false;
		itSlaveNode->UpdateSectionMeshOuter(&Mesh);

		PrevPos = itSlaveNode->GetPosition();
	}

	// Volume mesh points are built
	m_iNeedsBuilding &= ALL ^ VOLUME;
	return true;
}

bool CDualYarn::AddSurfaceToMesh(CMesh &Mesh, CMesh &OuterMesh, bool bAddEndCaps) const
{

	CYarn::AddSurfaceToMesh(Mesh);

	TGLOG("Adding outer yarn surface to mesh");
	// If the yarn needs building then build it before creating the mesh
	if (!BuildYarnIfNeeded(SURFACE))
		return false;

	// Check the number of section points on all the nodes is the same
	int iNumPoints = -1;
	int iNumNodes = (int)m_SlaveNodes.size();
	vector<CSlaveNode>::iterator itNode;
	for (itNode = m_SlaveNodes.begin(); itNode != m_SlaveNodes.end(); ++itNode)
	{
		if (iNumPoints == -1)
		{
			iNumPoints = (int)itNode->GetSectionPointsOuter().size();
		}
		else if (iNumPoints != (int)itNode->GetSectionPointsOuter().size())
		{
			// Cannot create surface mesh if the number of section points is not the same for all nodes
			TGERROR("Unable to create surface mesh, number of section points is not the same for all slave nodes");
			assert(false);
			return false;
		}
	}

	// Add inner nodes to the mesh
	vector<XYZ>::const_iterator itSectionPoint;
	int iFirstNode = (int)OuterMesh.GetNumNodes();
	for (itNode = m_SlaveNodes.begin(); itNode != m_SlaveNodes.end(); ++itNode)
	{
		for (itSectionPoint = itNode->GetSectionPointsOuter().begin(); itSectionPoint != itNode->GetSectionPointsOuter().end(); ++itSectionPoint)
		{
			OuterMesh.AddNode(*itSectionPoint);
		}
	}

		// Create the surface mesh elements
	int i, j, u, v;
	for (i = 0; i < iNumNodes - 1; ++i)
	{
		for (j = 0; j < iNumPoints; ++j)
		{
			vector<int> Indices;
			// Add quad (if triangles are needed they can be converted to triangles later)
			u = i;   v = j;   if (v == iNumPoints) v = 0; Indices.push_back(iFirstNode + v + u * iNumPoints);
			u = i + 1; v = j;   if (v == iNumPoints) v = 0; Indices.push_back(iFirstNode + v + u * iNumPoints);
			u = i + 1; v = j + 1; if (v == iNumPoints) v = 0; Indices.push_back(iFirstNode + v + u * iNumPoints);
			u = i;   v = j + 1; if (v == iNumPoints) v = 0; Indices.push_back(iFirstNode + v + u * iNumPoints);
			OuterMesh.AddElement(CMesh::QUAD, Indices);
		}

	}

	if (bAddEndCaps)
		AddEndCapsToMeshOuter(OuterMesh);

	return true;
}

void CDualYarn::AddEndCapsToMeshOuter(CMesh &Mesh) const
{
	if (m_SlaveNodes.empty())
		return;

	const CSlaveNode &StartNode = m_SlaveNodes[0];
	const CSlaveNode &EndNode = m_SlaveNodes[m_SlaveNodes.size() - 1];

	if (StartNode.GetPosition() == EndNode.GetPosition())
		return;

	CMesh StartMesh, EndMesh;

	StartMesh = CSectionMeshTriangulate::GetSimpleMeshOuter(StartNode.Get2DSectionPoints(),StartNode.Get2DSectionPointsOuter());
	EndMesh = CSectionMeshTriangulate::GetSimpleMeshOuter(EndNode.Get2DSectionPoints(),EndNode.Get2DSectionPointsOuter());
	
	XYZ StartSide = CrossProduct(StartNode.GetTangent(), StartNode.GetUp());
	XYZ EndSide = CrossProduct(EndNode.GetTangent(), EndNode.GetUp());

	StartMesh.Rotate(WXYZ(StartSide, StartNode.GetUp(), -StartNode.GetTangent()));
	EndMesh.Rotate(WXYZ(EndSide, EndNode.GetUp(), -EndNode.GetTangent()));

	EndMesh.FlipNormals();
	
	Mesh.InsertMesh(StartMesh, StartNode.GetPosition());
	Mesh.InsertMesh(EndMesh, EndNode.GetPosition());
}

bool CDualYarn::AddVolumeToMesh(CMesh &Mesh, CMesh &OuterMesh) const
{
	CYarn::AddVolumeToMesh(Mesh);

	TGLOG("Adding yarn volume to mesh");
	// Build the yarn with section meshes if needed
	if (!BuildYarnIfNeeded(VOLUME))
		return false;

	// Check the section meshes are compatible
	bool bFirst = true;
	CMesh ReferenceMesh;
	//    int iNumNodes = (int)m_SlaveNodes.size();
	vector<CSlaveNode>::iterator itNode;
	for (itNode = m_SlaveNodes.begin(); itNode != m_SlaveNodes.end(); ++itNode)
	{
		if (bFirst)
		{
			ReferenceMesh = itNode->GetSectionMeshOuter();
			bFirst = false;
		}
		else
		{
			// Check mesh is compatible
			bool bCompatible = ReferenceMesh.GetNumNodes() == itNode->GetSectionMeshOuter().GetNumNodes();
			int i;
			for (i = 0; i < CMesh::NUM_ELEMENT_TYPES; ++i)
			{
				if (!bCompatible)
					break;
				if (ReferenceMesh.GetIndices((CMesh::ELEMENT_TYPE)i) != itNode->GetSectionMeshOuter().GetIndices((CMesh::ELEMENT_TYPE)i))
					bCompatible = false;
			}
			if (!bCompatible)
			{
				// Cannot create surface mesh if the section meshes are not compatible
				TGERROR("Unable to create volume mesh, not all section meshes are compatible");
				assert(false);
				return false;
			}
		}
	}

	// Add nodes and elements to the mesh
	list<int>::const_iterator itIndex;
	int iPrevIndex = -1;
	int iIndex;
	int aiTriangles[3];
	int aiQuads[4];
	for (itNode = m_SlaveNodes.begin(); itNode != m_SlaveNodes.end(); ++itNode)
	{
		const CMesh &SectionMesh = itNode->GetSectionMeshOuter();
		iIndex = OuterMesh.InsertNodes(SectionMesh);
		const list<int> &PolygonIndices = SectionMesh.GetIndices(CMesh::POLYGON);
		for (itIndex = PolygonIndices.begin(); itIndex != PolygonIndices.end(); ++itIndex)
		{
			OuterMesh.GetIndices(CMesh::POLYGON).push_back(*(itIndex)+iIndex);
		}

		if (iPrevIndex != -1)
		{
			const list<int> &TriIndices = SectionMesh.GetIndices(CMesh::TRI);
			for (itIndex = TriIndices.begin(); itIndex != TriIndices.end(); )
			{
				aiTriangles[0] = *(itIndex++);
				aiTriangles[1] = *(itIndex++);
				aiTriangles[2] = *(itIndex++);

				// Element node ordering as defined in VTK
				OuterMesh.GetIndices(CMesh::WEDGE).push_back(aiTriangles[0] + iPrevIndex);
				OuterMesh.GetIndices(CMesh::WEDGE).push_back(aiTriangles[1] + iPrevIndex);
				OuterMesh.GetIndices(CMesh::WEDGE).push_back(aiTriangles[2] + iPrevIndex);

				OuterMesh.GetIndices(CMesh::WEDGE).push_back(aiTriangles[0] + iIndex);
				OuterMesh.GetIndices(CMesh::WEDGE).push_back(aiTriangles[1] + iIndex);
				OuterMesh.GetIndices(CMesh::WEDGE).push_back(aiTriangles[2] + iIndex);
			}
			const list<int> &QuadIndices = SectionMesh.GetIndices(CMesh::QUAD);
			for (itIndex = QuadIndices.begin(); itIndex != QuadIndices.end(); )
			{
				aiQuads[0] = *(itIndex++);
				aiQuads[1] = *(itIndex++);
				aiQuads[2] = *(itIndex++);
				aiQuads[3] = *(itIndex++);

				// Element node ordering as defined in VTK
				OuterMesh.GetIndices(CMesh::HEX).push_back(aiQuads[0] + iIndex);
				OuterMesh.GetIndices(CMesh::HEX).push_back(aiQuads[1] + iIndex);
				OuterMesh.GetIndices(CMesh::HEX).push_back(aiQuads[2] + iIndex);
				OuterMesh.GetIndices(CMesh::HEX).push_back(aiQuads[3] + iIndex);

				OuterMesh.GetIndices(CMesh::HEX).push_back(aiQuads[0] + iPrevIndex);
				OuterMesh.GetIndices(CMesh::HEX).push_back(aiQuads[1] + iPrevIndex);
				OuterMesh.GetIndices(CMesh::HEX).push_back(aiQuads[2] + iPrevIndex);
				OuterMesh.GetIndices(CMesh::HEX).push_back(aiQuads[3] + iPrevIndex);
			}
		}
		iPrevIndex = iIndex;
	}
	return true;
}