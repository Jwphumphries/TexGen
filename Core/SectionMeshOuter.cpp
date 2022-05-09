#include "PrecompiledHeaders.h"
#include "SectionMeshOuter.h"

using namespace TexGen;

CSectionMeshOuter::CSectionMeshOuter(int iNumLayers, bool bTriangleCorners)
	: m_iNumLayers(iNumLayers)
	, m_bTriangleCorners(bTriangleCorners)
{
	if (iNumLayers != -1 && iNumLayers % 2 != 0)
	{
		assert(false);
		TGERROR("Warning: Rectangular mesh specified with an odd number of layers: " << iNumLayers);
	}
}

CSectionMeshOuter::~CSectionMeshOuter(void)
{
}



bool CSectionMeshOuter::CreateMesh(const vector<XY> &Section) const
{
	int iNumLayers = m_iNumLayers;
	if (m_iNumLayers == -1)
		iNumLayers = 3;
	
	int i, j;
	int iPointIndex, iNumPoints = (int)Section.size();

	
	
	// creating fake inner section
	XY Q;
	vector<XY> InnerSection;
	double theta;
	for (i = 0; i < iNumPoints; ++i)
	{
		theta = i * 2*PI / iNumPoints;
		Q.x = 0.5*cos(theta);
		Q.y = 0.5*sin(theta);
		InnerSection.push_back(Q);
	}

	m_Mesh.Clear();
	m_Mesh.SetNumNodes((iNumLayers + 1)*(iNumPoints));
	
	XY P;
	XY dir; // direction vector
	double mag; //magnitude of direction vector
	
	for (i = 0; i < iNumPoints; ++i)
	{
		for (j =0; j < iNumLayers + 1; ++j)
		{
			dir = Section[i] - InnerSection[i];
			mag = sqrt(dir.x*dir.x + dir.y* dir.y);
			P = InnerSection[i] + (j * mag / (iNumLayers + 1))*Section[i];

			m_Mesh.SetNode(j+(iNumLayers+1)*i , XYZ(P.x, P.y, 0));
		}
	}

	for (i = 0; i < iNumPoints; ++i)
	{
		for (j = 0; j < iNumLayers; ++j)
		{
			m_Mesh.GetIndices(CMesh::QUAD).push_back(j+(iNumLayers+1)*i);
			m_Mesh.GetIndices(CMesh::QUAD).push_back((j+1) + (iNumLayers + 1)*i);
			m_Mesh.GetIndices(CMesh::QUAD).push_back((j+1) + (iNumLayers + 1)*(i+1));
			m_Mesh.GetIndices(CMesh::QUAD).push_back(j + (iNumLayers + 1)*(i+1));
		}
	}
	

	

	
	
	

	return true;
}

int CSectionMeshOuter::CalculateNumberofLayers(const vector<XY> &Section) const
{
	// Section may either be rotated section or not start from point on x axis
	// in which case the points need rotating back otherwise test for number of layers is invalid
	XY p1, p2, dp;
	p1 = Section[0];
	p2 = Section[Section.size() / 2];
	dp = p1 - p2;
	double dAngle = atan2(dp.y, dp.x);

	vector<XY> RotatedSection;
	for (int i = 1; i <= (int)Section.size() / 4 + 1; ++i)
	{
		XY RotPoint;
		RotPoint.x = Section[i].x*cos(dAngle) + Section[i].y*sin(dAngle);
		RotPoint.y = Section[i].y*cos(dAngle) - Section[i].x*sin(dAngle);
		RotatedSection.push_back(RotPoint);
	}

	// Assuming the section is symmetric about all 4 quadrants
	// 25-9-12 Can't assume this. Both hybrid and polygon sections may result in non-symmetrical sections
	int i;
	XY P1, P2, DP;
	vector<XY>::iterator itRotatedSection;
	//for (i=1; i<=(int)RotatedSection.size()/4; ++i)
	for (i = 0; i < (int)RotatedSection.size() - 1; ++i)
	{
		P1 = RotatedSection[i];
		P2 = RotatedSection[i + 1];
		DP = P2 - P1;
		if (abs(DP.x) > abs(DP.y))
		{
			return (i + 1) * 2;  // +1 because RotatedSection starts at 2nd point in Section
		}
	}
	return i * 2;
	// Should never reach this point
	//return 0;
}

bool CSectionMeshOuter::CreateSingleLayerMesh(const vector<XY> &Section) const
{
	if (Section.size() % 2 != 0)
	{
		assert(false);
		TGERROR("Unable to create section mesh, the number of sections points specified is odd: " << Section.size());
		return false;
	}

	int iNumColumns = Section.size() / 2;

	m_Mesh.Clear();
	int i, j;
	for (i = 0; i < (int)Section.size(); ++i)
	{
		XY pt = Section[i];
		m_Mesh.AddNode(XYZ(pt.x, pt.y, 0));
	}

	// Build up a single row of elements, two triangles at either end and rectangles in the middle
	for (j = 0; j < iNumColumns; ++j)
	{
		if (j == 0)
		{
			// Triangle at the right edges
			vector<int> Indices;
			Indices.push_back(0);
			Indices.push_back(1);
			Indices.push_back(Section.size() - 1);
			m_Mesh.AddElement(CMesh::TRI, Indices);
		}
		else if (j == iNumColumns - 1)
		{
			// Triangle at the left edge
			vector<int> Indices;
			Indices.push_back(iNumColumns - 1);
			Indices.push_back(iNumColumns);
			Indices.push_back(iNumColumns + 1);
			m_Mesh.AddElement(CMesh::TRI, Indices);
		}
		else
		{
			// Rectangles in the middle section
			vector<int> Indices;
			Indices.push_back(j);
			Indices.push_back(j + 1);
			Indices.push_back(Section.size() - j - 1);
			Indices.push_back(Section.size() - j);
			m_Mesh.AddElement(CMesh::QUAD, Indices);
		}
	}

	bool bSurfaceNodes = true; // Add this to class later
	if (bSurfaceNodes)
	{
		vector<int> Indices;
		for (j = 0; j < (int)Section.size(); ++j)
		{
			Indices.push_back(j);
		}
		Indices.push_back(Indices[0]);
		m_Mesh.AddElement(CMesh::POLYGON, Indices);
	}

	return true;
}

void CSectionMeshOuter::SetNumLayers(int iNum)
{
	m_iNumLayers = iNum;
}
