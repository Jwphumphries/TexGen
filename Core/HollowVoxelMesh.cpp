/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2024 Joseph Humphries

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
#include "HollowVoxelMesh.h"
#include "TexGen.h"
#include "DomainPrism.h"
#include <iterator>
//#define SHINY_PROFILER TRUE


using namespace TexGen;

CHollowVoxelMesh::CHollowVoxelMesh(string Type)
	:CVoxelMesh(Type)
{

}

CHollowVoxelMesh::~CHollowVoxelMesh(void)
{

}

bool CHollowVoxelMesh::CalculateVoxelSizes(CTextile &Textile)
{
	XYZ DomSize;

	CDomainPrism* Domain = Textile.GetDomain()->GetPrismDomain();

	// Get the XYZ size of each axis of the domain, 
	// taking into account that they may be rotated depending on the orientation of the domain yarn specified
	Domain->GetPolygonLimits(m_StartPoint, m_RotatedVoxSize);

	m_RotatedVoxSize[0] /= m_XVoxels;
	m_RotatedVoxSize[1] /= m_YVoxels;
	m_RotatedVoxSize[2] /= m_ZVoxels;

	// Create map of which elements in the xy bounding box are in the prism cross-section - assumes constant cross-section
	GetElementMap(Textile);

	return true;
}

void CHollowVoxelMesh::SaveVoxelMesh(CTextile &Textile, string OutputFilename, vector<XYZ> &P0Arr, vector<XYZ> &P1Arr, vector<int> &XVoxNumArr, int YVoxNum, int ZVoxNum,
	bool bOutputMatrix, bool bOutputYarns, int iBoundaryConditions, int iElementType, int FileType)
{
	//PROFILE_SHARED_DEFINE(ProfileTest)
	//PROFILE_FUNC()

	const CDomain* pDomain = Textile.GetDomain();
	if (!pDomain)
	{
		TGERROR("Unable to create ABAQUS input file: No domain specified");
		return;
	}
	//PROFILE_SHARED_BEGIN(ProfileTest);
	m_P0Arr = P0Arr;
	m_P1Arr = P1Arr;
	m_XVoxNumArr = XVoxNumArr;

	m_NumSections = XVoxNumArr.size();
	m_YVoxels = YVoxNum;
	m_ZVoxels = ZVoxNum;
	m_XVoxels = 0;

	int i;

	for (i = 0; i < m_NumSections; i++)
	{
		m_XVoxels += m_XVoxNumArr[i];
	}


	TGLOG("Calculating voxel sizes");
	if (!CalculateVoxelSizes(Textile))
	{
		TGERROR("Unable to create ABAQUS input file: Error calculating voxel sizes");
		return;
	}
	TGLOG("Replacing spaces in filename with underscore for ABAQUS compatibility");
	OutputFilename = ReplaceFilenameSpaces(OutputFilename);
	//GetYarnGridIntersections(Textile);
	if (FileType == INP_EXPORT)
	{
		CTimer timer;
		timer.start("Timing SaveToAbaqus");
		SaveToAbaqus(OutputFilename, Textile, bOutputMatrix, bOutputYarns, iBoundaryConditions, iElementType);
		timer.check("End of SaveToAbaqus");
		timer.stop();
	}
	else
		SaveVoxelMeshToVTK(OutputFilename, Textile);

	m_ElementsInfo.clear(); // Clear point_info data as otherwise retains memory space until create another voxel mesh or exit program

   // PROFILE_END();
   // PROFILER_UPDATE();
   // PROFILER_OUTPUT("ProfileOutput.txt");
	//SaveToSCIRun( OutputFilename, Textile );
}



void CHollowVoxelMesh::OutputNodes(ostream &Output, CTextile &Textile, int Filetype)
{
	int x, y, z, SectionNum;
	int iNodeIndex = 1;
	int *pNodeIndex = &iNodeIndex;
	vector<XYZ> CentrePoints;
	vector<POINT_INFO> RowInfo;
	XYZ StartPoint = m_StartPoint;
	XY Centroid;
	XYZ CentroidPoint;

	vector<XYZ> CentroidPoints;

	// Variables 
	double X;
	double Za;
	double Zb;

	double DeltaX;
	double DeltaZa;
	double DeltaZb;

	double m;
	double c;
	double zValue;

	XYZ P0a;
	XYZ P0b;

	XYZ P1a;
	XYZ P1b;

	XYZ Pna;
	XYZ Pnb;

	XYZ dirAA;
	XYZ dirBB;
	XYZ dirAB;

	XYZ Pnab;

	// Point creation is based on diagrams in note book
	for (z = 0; z <= m_ZVoxels; ++z)
	{


		for (y = 0; y <= m_YVoxels; ++y)
		{
			XYZ YStartPoint;
			YStartPoint = m_StartPoint + m_RotatedVoxSize[1] * y;

			for (SectionNum = 0; SectionNum < m_NumSections; ++SectionNum)
			{
				XYZ Point;
				XYZ Point2;
				XYZ Point3;
				XYZ Point4;

				P0a = m_P0Arr[SectionNum];
				P1a = m_P1Arr[SectionNum];


				if (SectionNum < (m_NumSections - 1))
				{
					P0b = m_P0Arr[SectionNum + 1];
					P1b = m_P1Arr[SectionNum + 1];
				}
				else if (SectionNum == (m_NumSections - 1))
				{
					P0b = m_P0Arr[0];
					P1b = m_P1Arr[0];
				}

				
				




				if (SectionNum == 0)
				{
					Za = GetLength(P1a - P0a);
					Zb = GetLength(P1b - P0b);



					DeltaZa = Za / m_ZVoxels;
					DeltaZb = Zb / m_ZVoxels;

					// Calculating direction vectors AA and BB
					dirAA = P1a - P0a;
					dirBB = P1b - P0b;

					Normalise(dirAA);
					Normalise(dirBB);



					// equations of point Pna and Pnb
					Pna = P0a + z * DeltaZa * dirAA;

					StartPoint = YStartPoint + Pna;    //+ Pna;
					Point = StartPoint;  // +m_RotatedVoxSize[0] * x;

					if (Filetype == INP_EXPORT)
					{
						Output << iNodeIndex << ", ";
						Output << Point << "\n";
					}
					else if (Filetype == VTU_EXPORT)
						m_Mesh.AddNode(Point);
					++iNodeIndex;
				}

				if (SectionNum >= 0 && SectionNum < m_NumSections - 1)
				{
					for (x = 1; x <= m_XVoxNumArr[SectionNum]; x++)
					{

						Za = GetLength(P1a - P0a);
						Zb = GetLength(P1b - P0b);



						DeltaZa = Za / m_ZVoxels;
						DeltaZb = Zb / m_ZVoxels;

						// Calculating direction vectors AA and BB
						dirAA = P1a - P0a;
						dirBB = P1b - P0b;

						Normalise(dirAA);
						Normalise(dirBB);



						// equations of point Pna and Pnb
						Pna = P0a + z * DeltaZa * dirAA;
						Pnb = P0b + z * DeltaZb * dirBB;


						// Calculating everything AB
						X = GetLength(Pnb - Pna);
						DeltaX = X / m_XVoxNumArr[SectionNum];
						dirAB = Pnb - Pna;
						Normalise(dirAB);




						Pnab = Pna + x * DeltaX * dirAB;

						//m = ((P0b.z + DeltaZb * z) - (P0a.z + DeltaZa * z)) / (P0b.x - P0a.x);

						//c = P0a.z + DeltaZa * z - m * P0a.x;

						//zValue = m * (P0a.x + DeltaX * x) + c;

						StartPoint = YStartPoint;
						//StartPoint.z = zValue;

						Point = StartPoint + Pnab;

						if (Filetype == INP_EXPORT)
						{
							Output << iNodeIndex << ", ";
							Output << Point << "\n";
						}
						else if (Filetype == VTU_EXPORT)
							m_Mesh.AddNode(Point);


						// Points for centroid

						//Point 2
						Pnab = Pna + (x - 1) * DeltaX * dirAB;
						StartPoint = YStartPoint;
						Point2 = StartPoint + Pnab;

						//Point 3

						// equations of point Pna and Pnb
						Pna = P0a + (z + 1) * DeltaZa * dirAA;
						Pnb = P0b + (z + 1) * DeltaZb * dirBB;


						// Calculating everything AB
						X = GetLength(Pnb - Pna);
						DeltaX = X / m_XVoxNumArr[SectionNum];
						dirAB = Pnb - Pna;
						Normalise(dirAB);


						Pnab = Pna + (x - 1) * DeltaX * dirAB;

						StartPoint = YStartPoint;

						Point3 = StartPoint + Pnab;

						//Point4

						// equations of point Pna and Pnb
						Pna = P0a + (z + 1) * DeltaZa * dirAA;
						Pnb = P0b + (z + 1) * DeltaZb * dirBB;


						// Calculating everything AB
						X = GetLength(Pnb - Pna);
						DeltaX = X / m_XVoxNumArr[SectionNum];
						dirAB = Pnb - Pna;
						Normalise(dirAB);


						Pnab = Pna + x * DeltaX * dirAB;

						StartPoint = YStartPoint;

						Point4 = StartPoint + Pnab;

						// Finding centre points of accociated elements by finding the centroid
						if (x > 0 && x <= m_XVoxNumArr[SectionNum] && y < m_YVoxels && z < m_ZVoxels)
						{

							CentroidPoints.clear();

							CentroidPoints.push_back(Point);
							CentroidPoints.push_back(Point2);
							CentroidPoints.push_back(Point3);
							CentroidPoints.push_back(Point4);

							Centroid = GetCentroidXZ(&CentroidPoints.front(), 4);

							CentroidPoint = XYZ(Centroid.x, m_StartPoint.y + m_RotatedVoxSize[1].y * (y + 0.5), Centroid.y);

							CentrePoints.push_back(CentroidPoint);

						}
						++iNodeIndex;
					}

				}
				else if (SectionNum == (m_NumSections - 1))
				{
					for (x = 1; x <= m_XVoxNumArr[SectionNum]; x++)
					{

						Za = GetLength(P1a - P0a);
						Zb = GetLength(P1b - P0b);



						DeltaZa = Za / m_ZVoxels;
						DeltaZb = Zb / m_ZVoxels;

						// Calculating direction vectors AA and BB
						dirAA = P1a - P0a;
						dirBB = P1b - P0b;

						Normalise(dirAA);
						Normalise(dirBB);



						// equations of point Pna and Pnb
						Pna = P0a + z * DeltaZa * dirAA;
						Pnb = P0b + z * DeltaZb * dirBB;


						// Calculating everything AB
						X = GetLength(Pnb - Pna);
						DeltaX = X / m_XVoxNumArr[SectionNum];
						dirAB = Pnb - Pna;
						Normalise(dirAB);




						Pnab = Pna + x * DeltaX * dirAB;

						//m = ((P0b.z + DeltaZb * z) - (P0a.z + DeltaZa * z)) / (P0b.x - P0a.x);

						//c = P0a.z + DeltaZa * z - m * P0a.x;

						//zValue = m * (P0a.x + DeltaX * x) + c;

						StartPoint = YStartPoint;
						//StartPoint.z = zValue;

						Point = StartPoint + Pnab;


						if (x != m_XVoxNumArr[SectionNum])
						{


							if (Filetype == INP_EXPORT)
							{
								Output << iNodeIndex << ", ";
								Output << Point << "\n";
							}
							else if (Filetype == VTU_EXPORT)
								m_Mesh.AddNode(Point);

							++iNodeIndex;
						}


						// Points for centroid

						//Point 2
						Pnab = Pna + (x - 1) * DeltaX * dirAB;
						StartPoint = YStartPoint;
						Point2 = StartPoint + Pnab;

						//Point 3

						// equations of point Pna and Pnb
						Pna = P0a + (z + 1) * DeltaZa * dirAA;
						Pnb = P0b + (z + 1) * DeltaZb * dirBB;


						// Calculating everything AB
						X = GetLength(Pnb - Pna);
						DeltaX = X / m_XVoxNumArr[SectionNum];
						dirAB = Pnb - Pna;
						Normalise(dirAB);


						Pnab = Pna + (x - 1) * DeltaX * dirAB;

						StartPoint = YStartPoint;

						Point3 = StartPoint + Pnab;

						//Point4

						// equations of point Pna and Pnb
						Pna = P0a + (z + 1) * DeltaZa * dirAA;
						Pnb = P0b + (z + 1) * DeltaZb * dirBB;


						// Calculating everything AB
						X = GetLength(Pnb - Pna);
						DeltaX = X / m_XVoxNumArr[SectionNum];
						dirAB = Pnb - Pna;
						Normalise(dirAB);


						Pnab = Pna + x * DeltaX * dirAB;

						StartPoint = YStartPoint;

						Point4 = StartPoint + Pnab;

						// Finding centre points of accociated elements by finding the centroid
						if (x > 0 && x <= m_XVoxNumArr[SectionNum] && y < m_YVoxels && z < m_ZVoxels)
						{

							CentroidPoints.clear();

							CentroidPoints.push_back(Point);
							CentroidPoints.push_back(Point2);
							CentroidPoints.push_back(Point3);
							CentroidPoints.push_back(Point4);

							Centroid = GetCentroidXZ(&CentroidPoints.front(), 4);

							CentroidPoint = XYZ(Centroid.x, m_StartPoint.y + m_RotatedVoxSize[1].y * (y + 0.5), Centroid.y);

							CentrePoints.push_back(CentroidPoint);

						}
						//++iNodeIndex;
					}

				}


				
			}

		}
		RowInfo.clear();   // Changed to do layer at a time instead of row to optimise
		Textile.GetPointInformation(CentrePoints, RowInfo);
		m_ElementsInfo.insert(m_ElementsInfo.end(), RowInfo.begin(), RowInfo.end());
		CentrePoints.clear();
	}
}


void CHollowVoxelMesh::OutputNodesQuad(ostream &Output, CTextile &Textile, int Filetype)
{
	int x, y, z, SectionNum;
	int iNodeIndex = 1;
	int *pNodeIndex = &iNodeIndex;
	vector<XYZ> CentrePoints;
	vector<POINT_INFO> RowInfo;
	XYZ StartPoint = m_StartPoint;
	XY Centroid;
	XYZ CentroidPoint;

	vector<XYZ> CentroidPoints;

	// Variables 
	double X;
	double Za;
	double Zb;

	double DeltaX;
	double DeltaZa;
	double DeltaZb;

	double m;
	double c;
	double zValue;

	XYZ P0a;
	XYZ P0b;

	XYZ P1a;
	XYZ P1b;

	XYZ Pna;
	XYZ Pnb;

	XYZ dirAA;
	XYZ dirBB;
	XYZ dirAB;

	XYZ Pnab;

	// Point creation is based on diagrams in note book
	for (z = 0; z <= m_ZVoxels; ++z)
	{


		for (y = 0; y <= m_YVoxels; ++y)
		{
			XYZ YStartPoint;
			YStartPoint = m_StartPoint + m_RotatedVoxSize[1] * y;

			for (SectionNum = 0; SectionNum < m_NumSections; ++SectionNum)
			{
				XYZ Point;
				XYZ Point2;
				XYZ Point3;
				XYZ Point4;

				P0a = m_P0Arr[SectionNum];
				P0b = m_P0Arr[SectionNum + 1];

				P1a = m_P1Arr[SectionNum];
				P1b = m_P1Arr[SectionNum + 1];




				if (SectionNum == 0)
				{
					Za = GetLength(P1a - P0a);
					Zb = GetLength(P1b - P0b);

					DeltaZa = Za / m_ZVoxels;
					DeltaZb = Zb / m_ZVoxels;

					// Calculating direction vectors AA and BB
					dirAA = P1a - P0a;
					dirBB = P1b - P0b;

					Normalise(dirAA);
					Normalise(dirBB);



					// equations of point Pna and Pnb
					Pna = P0a + z * DeltaZa * dirAA;

					StartPoint = YStartPoint + Pna;
					Point = StartPoint;  // +m_RotatedVoxSize[0] * x;

					if (Filetype == INP_EXPORT)
					{
						Output << iNodeIndex << ", ";
						Output << Point << "\n";
					}
					else if (Filetype == VTU_EXPORT)
						m_Mesh.AddNode(Point);
					++iNodeIndex;
				}


				for (x = 1; x <= m_XVoxNumArr[SectionNum]; x++)
				{

					Za = GetLength(P1a - P0a);
					Zb = GetLength(P1b - P0b);

					DeltaZa = Za / m_ZVoxels;
					DeltaZb = Zb / m_ZVoxels;

					// Calculating direction vectors AA and BB
					dirAA = P1a - P0a;
					dirBB = P1b - P0b;

					Normalise(dirAA);
					Normalise(dirBB);



					// equations of point Pna and Pnb
					Pna = P0a + z * DeltaZa * dirAA;
					Pnb = P0b + z * DeltaZb * dirBB;


					// Calculating everything AB
					X = GetLength(Pnb - Pna);
					DeltaX = X / m_XVoxNumArr[SectionNum];
					dirAB = Pnb - Pna;
					Normalise(dirAB);




					Pnab = Pna + x * DeltaX * dirAB;

					//m = ((P0b.z + DeltaZb * z) - (P0a.z + DeltaZa * z)) / (P0b.x - P0a.x);

					//c = P0a.z + DeltaZa * z - m * P0a.x;

					//zValue = m * (P0a.x + DeltaX * x) + c;

					StartPoint = YStartPoint;
					//StartPoint.z = zValue;

					Point = StartPoint + Pnab;

					if (Filetype == INP_EXPORT)
					{
						Output << iNodeIndex << ", ";
						Output << Point << "\n";
					}
					else if (Filetype == VTU_EXPORT)
						m_Mesh.AddNode(Point);


					// Points for centroid

					//Point 2
					Pnab = Pna + (x - 1) * DeltaX * dirAB;
					StartPoint = YStartPoint;
					Point2 = StartPoint + Pnab;

					//Point 3

					// equations of point Pna and Pnb
					Pna = P0a + (z + 1) * DeltaZa * dirAA;
					Pnb = P0b + (z + 1) * DeltaZb * dirBB;


					// Calculating everything AB
					X = GetLength(Pnb - Pna);
					DeltaX = X / m_XVoxNumArr[SectionNum];
					dirAB = Pnb - Pna;
					Normalise(dirAB);


					Pnab = Pna + (x - 1) * DeltaX * dirAB;

					StartPoint = YStartPoint;

					Point3 = StartPoint + Pnab;

					//Point4

					// equations of point Pna and Pnb
					Pna = P0a + (z + 1) * DeltaZa * dirAA;
					Pnb = P0b + (z + 1) * DeltaZb * dirBB;


					// Calculating everything AB
					X = GetLength(Pnb - Pna);
					DeltaX = X / m_XVoxNumArr[SectionNum];
					dirAB = Pnb - Pna;
					Normalise(dirAB);


					Pnab = Pna + x * DeltaX * dirAB;

					StartPoint = YStartPoint;

					Point4 = StartPoint + Pnab;

					// Finding centre points of accociated elements by finding the centroid
					if (x > 0 && x <= m_XVoxNumArr[SectionNum] && y < m_YVoxels && z < m_ZVoxels)
					{

						CentroidPoints.clear();

						CentroidPoints.push_back(Point);
						CentroidPoints.push_back(Point2);
						CentroidPoints.push_back(Point3);
						CentroidPoints.push_back(Point4);

						Centroid = GetCentroidXZ(&CentroidPoints.front(), 4);

						CentroidPoint = XYZ(Centroid.x, m_StartPoint.y + m_RotatedVoxSize[1].y * (y + 0.5), Centroid.y);

						CentrePoints.push_back(CentroidPoint);

					}
					++iNodeIndex;
				}
			}

		}
		RowInfo.clear();   // Changed to do layer at a time instead of row to optimise
		Textile.GetPointInformation(CentrePoints, RowInfo);
		m_ElementsInfo.insert(m_ElementsInfo.end(), RowInfo.begin(), RowInfo.end());
		CentrePoints.clear();
	}


	// created the nodes associated with diagram one in my note book
	for (z = 0; z <= m_ZVoxels; ++z)
	{


		for (y = 0; y <= m_YVoxels; ++y)
		{
			XYZ YStartPoint;
			YStartPoint = m_StartPoint + m_RotatedVoxSize[1] * y;

			for (SectionNum = 0; SectionNum < m_NumSections; ++SectionNum)
			{
				XYZ Point;
				XYZ Point2;
				XYZ Point3;
				XYZ Point4;

				P0a = m_P0Arr[SectionNum];
				P0b = m_P0Arr[SectionNum + 1];

				P1a = m_P1Arr[SectionNum];
				P1b = m_P1Arr[SectionNum + 1];





				for (x = 0; x < m_XVoxNumArr[SectionNum]; x++)
				{

					Za = GetLength(P1a - P0a);
					Zb = GetLength(P1b - P0b);



					DeltaZa = Za / m_ZVoxels;
					DeltaZb = Zb / m_ZVoxels;

					// Calculating direction vectors AA and BB
					dirAA = P1a - P0a;
					dirBB = P1b - P0b;

					Normalise(dirAA);
					Normalise(dirBB);



					// equations of point Pna and Pnb
					Pna = P0a + z * DeltaZa * dirAA;
					Pnb = P0b + z * DeltaZb * dirBB;


					// Calculating everything AB
					X = GetLength(Pnb - Pna);
					DeltaX = X / m_XVoxNumArr[SectionNum];
					dirAB = Pnb - Pna;
					Normalise(dirAB);




					Pnab = Pna + (x + 0.5) * DeltaX * dirAB;

					//m = ((P0b.z + DeltaZb * z) - (P0a.z + DeltaZa * z)) / (P0b.x - P0a.x);

					//c = P0a.z + DeltaZa * z - m * P0a.x;

					//zValue = m * (P0a.x + DeltaX * x) + c;

					StartPoint = YStartPoint;
					//StartPoint.z = zValue;

					Point = StartPoint + Pnab;

					if (Filetype == INP_EXPORT)
					{
						Output << iNodeIndex << ", ";
						Output << Point << "\n";
					}
					else if (Filetype == VTU_EXPORT)
						m_Mesh.AddNode(Point);

					++iNodeIndex;
				}
			}
		}
	}

	// created the nodes associated with diagram two in my note book
	for (z = 0; z <= m_ZVoxels; ++z)
	{


		for (y = 0; y < m_YVoxels; ++y)
		{
			XYZ YStartPoint;
			YStartPoint = m_StartPoint + m_RotatedVoxSize[1] * (y + 0.5);

			for (SectionNum = 0; SectionNum < m_NumSections; ++SectionNum)
			{
				XYZ Point;
				XYZ Point2;
				XYZ Point3;
				XYZ Point4;

				P0a = m_P0Arr[SectionNum];
				P0b = m_P0Arr[SectionNum + 1];

				P1a = m_P1Arr[SectionNum];
				P1b = m_P1Arr[SectionNum + 1];

				Za = GetLength(P1a - P0a);
				Zb = GetLength(P1b - P0b);




				if (SectionNum == 0)
				{
					DeltaZa = Za / m_ZVoxels;
					DeltaZb = Zb / m_ZVoxels;

					// Calculating direction vectors AA and BB
					dirAA = P1a - P0a;
					dirBB = P1b - P0b;

					Normalise(dirAA);
					Normalise(dirBB);



					// equations of point Pna and Pnb
					Pna = P0a + z * DeltaZa * dirAA;
					StartPoint = YStartPoint + Pna;
					Point = StartPoint;  // +m_RotatedVoxSize[0] * x;

					if (Filetype == INP_EXPORT)
					{
						Output << iNodeIndex << ", ";
						Output << Point << "\n";
					}
					else if (Filetype == VTU_EXPORT)
						m_Mesh.AddNode(Point);
					++iNodeIndex;
				}



				for (x = 1; x <= m_XVoxNumArr[SectionNum]; x++)
				{
					// Could be more efficient in outer loop

					DeltaZa = Za / m_ZVoxels;
					DeltaZb = Zb / m_ZVoxels;

					// Calculating direction vectors AA and BB
					dirAA = P1a - P0a;
					dirBB = P1b - P0b;

					Normalise(dirAA);
					Normalise(dirBB);



					// equations of point Pna and Pnb
					Pna = P0a + z * DeltaZa * dirAA;
					Pnb = P0b + z * DeltaZb * dirBB;


					// Calculating everything AB
					X = GetLength(Pnb - Pna);
					DeltaX = X / m_XVoxNumArr[SectionNum];
					dirAB = Pnb - Pna;
					Normalise(dirAB);




					Pnab = Pna + x * DeltaX * dirAB;

					//m = ((P0b.z + DeltaZb * z) - (P0a.z + DeltaZa * z)) / (P0b.x - P0a.x);

					//c = P0a.z + DeltaZa * z - m * P0a.x;

					//zValue = m * (P0a.x + DeltaX * x) + c;

					StartPoint = YStartPoint;
					//StartPoint.z = zValue;

					Point = StartPoint + Pnab;

					if (Filetype == INP_EXPORT)
					{
						Output << iNodeIndex << ", ";
						Output << Point << "\n";
					}
					else if (Filetype == VTU_EXPORT)
						m_Mesh.AddNode(Point);

					++iNodeIndex;
				}
			}
		}
	}

	for (z = 0; z < m_ZVoxels; ++z)
	{


		for (y = 0; y <= m_YVoxels; ++y)
		{
			XYZ YStartPoint;
			YStartPoint = m_StartPoint + m_RotatedVoxSize[1] * y;

			for (SectionNum = 0; SectionNum < m_NumSections; ++SectionNum)
			{
				XYZ Point;
				XYZ Point2;
				XYZ Point3;
				XYZ Point4;

				P0a = m_P0Arr[SectionNum];
				P0b = m_P0Arr[SectionNum + 1];

				P1a = m_P1Arr[SectionNum];
				P1b = m_P1Arr[SectionNum + 1];



				if (SectionNum == 0)
				{
					Za = GetLength(P1a - P0a);
					Zb = GetLength(P1b - P0b);



					DeltaZa = Za / m_ZVoxels;
					DeltaZb = Zb / m_ZVoxels;

					// Calculating direction vectors AA and BB
					dirAA = P1a - P0a;
					dirBB = P1b - P0b;

					Normalise(dirAA);
					Normalise(dirBB);



					// equations of point Pna and Pnb
					Pna = P0a + (z + 0.5) * DeltaZa * dirAA;
					StartPoint = YStartPoint + Pna;
					Point = StartPoint;  // +m_RotatedVoxSize[0] * x;

					if (Filetype == INP_EXPORT)
					{
						Output << iNodeIndex << ", ";
						Output << Point << "\n";
					}
					else if (Filetype == VTU_EXPORT)
						m_Mesh.AddNode(Point);
					++iNodeIndex;
				}



				for (x = 1; x <= m_XVoxNumArr[SectionNum]; x++)
				{
					// Could be more efficient in outer loop


					Za = GetLength(P1a - P0a);
					Zb = GetLength(P1b - P0b);



					DeltaZa = Za / m_ZVoxels;
					DeltaZb = Zb / m_ZVoxels;

					// Calculating direction vectors AA and BB
					dirAA = P1a - P0a;
					dirBB = P1b - P0b;

					Normalise(dirAA);
					Normalise(dirBB);



					// equations of point Pna and Pnb
					Pna = P0a + (z + 0.5) * DeltaZa * dirAA;
					Pnb = P0b + (z + 0.5) * DeltaZb * dirBB;


					// Calculating everything AB
					X = GetLength(Pnb - Pna);
					DeltaX = X / m_XVoxNumArr[SectionNum];
					dirAB = Pnb - Pna;
					Normalise(dirAB);


					Pnab = Pna + x * DeltaX * dirAB;

					//m = ((P0b.z + DeltaZb * z) - (P0a.z + DeltaZa * z)) / (P0b.x - P0a.x);

					//c = P0a.z + DeltaZa * z - m * P0a.x;

					//zValue = m * (P0a.x + DeltaX * x) + c;

					StartPoint = YStartPoint;
					//StartPoint.z = zValue;

					Point = StartPoint + Pnab;

					if (Filetype == INP_EXPORT)
					{
						Output << iNodeIndex << ", ";
						Output << Point << "\n";
					}
					else if (Filetype == VTU_EXPORT)
						m_Mesh.AddNode(Point);

					++iNodeIndex;
				}
			}
		}
	}
}
/*
void CTaperedVoxelMesh::OutputNodesQuad(ostream &Output, CTextile &Textile, int Filetype)
{
	int x, y, z;
	int iNodeIndex = 1;
	vector<XYZ> CentrePoints;
	vector<POINT_INFO> RowInfo;
	XYZ StartPoint = m_StartPoint;

	for (z = 0; z <= m_ZVoxels; ++z)
	{
		StartPoint = m_StartPoint + m_RotatedVoxSize[2] * z;

		for (y = 0; y <= m_YVoxels; ++y)
		{
			XYZ YStartPoint;
			YStartPoint = StartPoint + m_RotatedVoxSize[1] * y;

			for (x = 0; x <= m_XVoxels; ++x)
			{
				XYZ Point;
				Point = YStartPoint + m_RotatedVoxSize[0] * x;

				if (Filetype == INP_EXPORT)
				{
					Output << iNodeIndex << ", ";
					Output << Point << "\n";
				}
				else if (Filetype == VTU_EXPORT)
					m_Mesh.AddNode(Point);

				if (x < m_XVoxels && y < m_YVoxels && z < m_ZVoxels)
				{
					if (m_ElementMap.at(make_pair(x, z)))  // Only store centre points for elements within prism
					{
						Point.x += 0.5*m_RotatedVoxSize[0].x;
						Point.x += 0.5*m_RotatedVoxSize[1].x;
						Point.x += 0.5*m_RotatedVoxSize[2].x;
						Point.y += 0.5*m_RotatedVoxSize[0].y;
						Point.y += 0.5*m_RotatedVoxSize[1].y;
						Point.y += 0.5*m_RotatedVoxSize[2].y;
						Point.z += 0.5*m_RotatedVoxSize[0].z;
						Point.z += 0.5*m_RotatedVoxSize[1].z;
						Point.z += 0.5*m_RotatedVoxSize[2].z;
						CentrePoints.push_back(Point);
					}
				}
				++iNodeIndex;
			}

		}
		RowInfo.clear();   // Changed to do layer at a time instead of row to optimise
		Textile.GetPointInformation(CentrePoints, RowInfo);
		m_ElementsInfo.insert(m_ElementsInfo.end(), RowInfo.begin(), RowInfo.end());
		CentrePoints.clear();
	}

	// created the nodes associated with diagram one in my note book
	for (z = 0; z <= m_ZVoxels; ++z)
	{
		StartPoint = m_StartPoint + m_RotatedVoxSize[2] * z;

		for (y = 0; y <= m_YVoxels; ++y)
		{
			XYZ YStartPoint;
			YStartPoint = StartPoint + m_RotatedVoxSize[1] * y;

			for (x = 0; x < m_XVoxels; ++x)
			{
				XYZ Point;
				Point = YStartPoint + m_RotatedVoxSize[0] * (x + 0.5);

				if (Filetype == INP_EXPORT)
				{
					Output << iNodeIndex << ", ";
					Output << Point << "\n";
				}
				else if (Filetype == VTU_EXPORT)
					m_Mesh.AddNode(Point);


				++iNodeIndex;
			}

		}

	}

	// created the nodes associated with diagram two in my note book
	for (z = 0; z <= m_ZVoxels; ++z)
	{
		StartPoint = m_StartPoint + m_RotatedVoxSize[2] * z;

		for (y = 0; y < m_YVoxels; ++y)
		{
			XYZ YStartPoint;
			YStartPoint = StartPoint + m_RotatedVoxSize[1] * (y + 0.5);

			for (x = 0; x <= m_XVoxels; ++x)
			{
				XYZ Point;
				Point = YStartPoint + m_RotatedVoxSize[0] * x;

				if (Filetype == INP_EXPORT)
				{
					Output << iNodeIndex << ", ";
					Output << Point << "\n";
				}
				else if (Filetype == VTU_EXPORT)
					m_Mesh.AddNode(Point);


				++iNodeIndex;
			}

		}

	}


	// created the nodes associated with diagram three in my note book
	for (z = 0; z < m_ZVoxels; ++z)
	{
		StartPoint = m_StartPoint + m_RotatedVoxSize[2] * (z + 0.5);

		for (y = 0; y <= m_YVoxels; ++y)
		{
			XYZ YStartPoint;
			YStartPoint = StartPoint + m_RotatedVoxSize[1] * y;

			for (x = 0; x <= m_XVoxels; ++x)
			{
				XYZ Point;
				Point = YStartPoint + m_RotatedVoxSize[0] * x;

				if (Filetype == INP_EXPORT)
				{
					Output << iNodeIndex << ", ";
					Output << Point << "\n";
				}
				else if (Filetype == VTU_EXPORT)
					m_Mesh.AddNode(Point);


				++iNodeIndex;
			}

		}

	}




}
*/
void CHollowVoxelMesh::GetElementMap(CTextile &Textile)
{
	m_ElementMap.clear();
	m_NumElements = 0;
	CDomainPrism* Domain = Textile.GetDomain()->GetPrismDomain();
	vector<XY> PrismPoints = Domain->GetPoints();

	XY Point, Min, Max;
	GetMinMaxXY(PrismPoints, Min, Max);
	double XSize = (Max.x - Min.x) / m_XVoxels;
	double ZSize = (Max.y - Min.y) / m_ZVoxels;  // y in 2D polygon translates to z coordinate in 3D

	Point.y = Min.y + 0.5*ZSize;
	for (int j = 0; j < m_ZVoxels; ++j)
	{
		Point.x = Min.x + 0.5*XSize;
		for (int i = 0; i < m_XVoxels; ++i)
		{
			if (PointInside(Point, PrismPoints))
			{
				m_ElementMap[make_pair(i, j)] = true;
				m_NumElements++;
			}
			else
				m_ElementMap[make_pair(i, j)] = false;
			Point.x += XSize;
		}
		Point.y += ZSize;
	}
}


/*

int CTaperedVoxelMesh::OutputHexElements(ostream &Output, bool bOutputMatrix, bool bOutputYarn, int Filetype)
{
	int numx = m_XVoxels + 1;
	int numy = m_YVoxels + 1;
	int x, y, z;
	vector<POINT_INFO>::iterator itElementInfo = m_ElementsInfo.begin();
	int iElementNumber = 1;

	vector<POINT_INFO> NewElementInfo;

	if (Filetype == SCIRUN_EXPORT)
		Output << m_NumElements * m_YVoxels;

	for (z = 0; z < m_ZVoxels; ++z)
	{
		for (y = 0; y < m_YVoxels; ++y)
		{
			for (x = 0; x < m_XVoxels; ++x)
			{
				if (m_ElementMap[make_pair(x, z)])  // Only export elements within domain prism outline
				{
					if ((itElementInfo->iYarnIndex == -1 && bOutputMatrix)
						|| (itElementInfo->iYarnIndex >= 0 && bOutputYarn))
					{
						if (Filetype == INP_EXPORT)
						{
							Output << iElementNumber << ", ";
							Output << (x + 1) + y * numx + z * numx*numy + 1 << ", " << (x + 1) + (y + 1)*numx + z * numx*numy + 1 << ", ";
							Output << x + (y + 1)*numx + z * numx*numy + 1 << ", " << x + y * numx + z * numx*numy + 1 << ", ";
							Output << (x + 1) + y * numx + (z + 1)*numx*numy + 1 << ", " << (x + 1) + (y + 1)*numx + (z + 1)*numx*numy + 1 << ", ";
							Output << x + (y + 1)*numx + (z + 1)*numx*numy + 1 << ", " << x + y * numx + (z + 1)*numx*numy + 1 << "\n";
						}
						else if (Filetype == SCIRUN_EXPORT)
						{
							Output << x + y * numx + z * numx*numy + 1 << ", " << (x + 1) + y * numx + z * numx*numy + 1 << ", ";
							Output << x + y * numx + (z + 1)*numx*numy + 1 << ", " << (x + 1) + y * numx + (z + 1)*numx*numy + 1 << ", ";
							Output << x + (y + 1)*numx + z * numx*numy + 1 << ", " << (x + 1) + (y + 1)*numx + z * numx*numy + 1 << ", ";
							Output << x + (y + 1)*numx + (z + 1)*numx*numy + 1 << ", " << (x + 1) + (y + 1)*numx + (z + 1)*numx*numy + 1 << "\n";
						}
						else  // VTU export
						{
							vector<int> Indices;
							Indices.push_back(x + y * numx + z * numx*numy);
							Indices.push_back((x + 1) + y * numx + z * numx*numy);
							Indices.push_back((x + 1) + y * numx + (z + 1)*numx*numy);
							Indices.push_back(x + y * numx + (z + 1)*numx*numy);
							Indices.push_back(x + (y + 1)*numx + z * numx*numy);
							Indices.push_back((x + 1) + (y + 1)*numx + z * numx*numy);
							Indices.push_back((x + 1) + (y + 1)*numx + (z + 1)*numx*numy);
							Indices.push_back(x + (y + 1)*numx + (z + 1)*numx*numy);
							m_Mesh.AddElement(CMesh::HEX, Indices);
						}
						++iElementNumber;
						if (bOutputYarn && !bOutputMatrix) // Just saving yarn so need to make element array with just yarn info
						{
							NewElementInfo.push_back(*itElementInfo);
						}
					}
					++itElementInfo;  // Only saved element info for elements within domain outline
				}
			}
		}
	}


	if (bOutputYarn && !bOutputMatrix)
	{
		m_ElementsInfo.clear();
		m_ElementsInfo = NewElementInfo;
	}
	return (iElementNumber - 1);
}

*/

int CHollowVoxelMesh::OutputHexElements(ostream &Output, bool bOutputMatrix, bool bOutputYarn, int Filetype)
{
	int numx = m_XVoxels;  // + 1;
	int numy = m_YVoxels + 1;
	int x, y, z;
	vector<POINT_INFO>::iterator itElementInfo = m_ElementsInfo.begin();
	int iElementNumber = 1;

	vector<POINT_INFO> NewElementInfo;

	if (Filetype == SCIRUN_EXPORT)
		Output << m_XVoxels * m_YVoxels*m_ZVoxels << "\n";

	for (z = 0; z < m_ZVoxels; ++z)
	{
		for (y = 0; y < m_YVoxels; ++y)
		{
			for (x = 0; x < m_XVoxels; ++x)
			{
				if ((itElementInfo->iYarnIndex == -1 && bOutputMatrix)
					|| (itElementInfo->iYarnIndex >= 0 && bOutputYarn))
				{
					if (Filetype == INP_EXPORT)
					{
						Output << iElementNumber << ", ";
						if (x < m_XVoxels - 1)
						{
							Output << (x + 1) + y * numx + z * numx*numy + 1 << ", " << (x + 1) + (y + 1)*numx + z * numx*numy + 1 << ", ";
							Output << x + (y + 1)*numx + z * numx*numy + 1 << ", " << x + y * numx + z * numx*numy + 1 << ", ";
							Output << (x + 1) + y * numx + (z + 1)*numx*numy + 1 << ", " << (x + 1) + (y + 1)*numx + (z + 1)*numx*numy + 1 << ", ";
							Output << x + (y + 1)*numx + (z + 1)*numx*numy + 1 << ", " << x + y * numx + (z + 1)*numx*numy + 1 << "\n";
						}
						else if (x == m_XVoxels - 1)
						{
							Output << 0 + y * numx + z * numx*numy + 1 << ", " << 0 + (y + 1)*numx + z * numx*numy + 1 << ", ";
							Output << x + (y + 1)*numx + z * numx*numy + 1 << ", " << x + y * numx + z * numx*numy + 1 << ", ";
							Output << 0 + y * numx + (z + 1)*numx*numy + 1 << ", " << 0 + (y + 1)*numx + (z + 1)*numx*numy + 1 << ", ";
							Output << x + (y + 1)*numx + (z + 1)*numx*numy + 1 << ", " << x + y * numx + (z + 1)*numx*numy + 1 << "\n";
						}
					}
					else if (Filetype == SCIRUN_EXPORT)
					{
						Output << x + y * numx + z * numx*numy + 1 << ", " << (x + 1) + y * numx + z * numx*numy + 1 << ", ";
						Output << x + y * numx + (z + 1)*numx*numy + 1 << ", " << (x + 1) + y * numx + (z + 1)*numx*numy + 1 << ", ";
						Output << x + (y + 1)*numx + z * numx*numy + 1 << ", " << (x + 1) + (y + 1)*numx + z * numx*numy + 1 << ", ";
						Output << x + (y + 1)*numx + (z + 1)*numx*numy + 1 << ", " << (x + 1) + (y + 1)*numx + (z + 1)*numx*numy + 1 << "\n";
					}
					else  // VTU export
					{
						vector<int> Indices;
						Indices.push_back(x + y * numx + z * numx*numy);
						Indices.push_back((x + 1) + y * numx + z * numx*numy);
						Indices.push_back((x + 1) + y * numx + (z + 1)*numx*numy);
						Indices.push_back(x + y * numx + (z + 1)*numx*numy);
						Indices.push_back(x + (y + 1)*numx + z * numx*numy);
						Indices.push_back((x + 1) + (y + 1)*numx + z * numx*numy);
						Indices.push_back((x + 1) + (y + 1)*numx + (z + 1)*numx*numy);
						Indices.push_back(x + (y + 1)*numx + (z + 1)*numx*numy);
						m_Mesh.AddElement(CMesh::HEX, Indices);
					}
					++iElementNumber;
					if (bOutputYarn && !bOutputMatrix) // Just saving yarn so need to make element array with just yarn info
					{
						NewElementInfo.push_back(*itElementInfo);
					}

				}
				++itElementInfo;
			}
		}
	}


	if (bOutputYarn && !bOutputMatrix)
	{
		m_ElementsInfo.clear();
		m_ElementsInfo = NewElementInfo;
	}
	return (iElementNumber - 1);
}



/*
int CTaperedVoxelMesh::OutputHexElementsQuad(ostream &Output, bool bOutputMatrix, bool bOutputYarn, int Filetype)
{
	int numx = m_XVoxels + 1;
	int numy = m_YVoxels + 1;
	int numz = m_ZVoxels + 1;
	int x, y, z;
	int numLinearNodes = numx * numy*numz;
	int numQuadNodes_1 = (numx - 1)*numy*numz;
	int numQuadNodes_2 = numx * (numy - 1)*numz;
	vector<POINT_INFO>::iterator itElementInfo = m_ElementsInfo.begin();
	int iElementNumber = 1;

	vector<POINT_INFO> NewElementInfo;

	if (Filetype == SCIRUN_EXPORT)
		Output << m_NumElements * m_YVoxels;

	for (z = 0; z < m_ZVoxels; ++z)
	{
		for (y = 0; y < m_YVoxels; ++y)
		{
			for (x = 0; x < m_XVoxels; ++x)
			{
				if (m_ElementMap[make_pair(x, z)])  // Only export elements within domain prism outline
				{
					if ((itElementInfo->iYarnIndex == -1 && bOutputMatrix)
						|| (itElementInfo->iYarnIndex >= 0 && bOutputYarn))
					{
						if (Filetype == INP_EXPORT)
						{
							Output << iElementNumber << ", ";
							Output << (x + 1) + y * numx + z * numx*numy + 1 << ", " << (x + 1) + (y + 1)*numx + z * numx*numy + 1 << ", ";
							Output << x + (y + 1)*numx + z * numx*numy + 1 << ", " << x + y * numx + z * numx*numy + 1 << ", ";
							Output << (x + 1) + y * numx + (z + 1)*numx*numy + 1 << ", " << (x + 1) + (y + 1)*numx + (z + 1)*numx*numy + 1 << ", ";
							Output << x + (y + 1)*numx + (z + 1)*numx*numy + 1 << ", " << x + y * numx + (z + 1)*numx*numy + 1 << ",";
							Output << numLinearNodes + numQuadNodes_1 + (x + 1) + y * numx + z * numx*(numy - 1) + 1 << ", " << numLinearNodes + x + (y + 1) * (numx - 1) + z * (numx - 1)*numy + 1 << ", ";
							Output << numLinearNodes + numQuadNodes_1 + x + y * numx + z * numx*(numy - 1) + 1 << ", " << numLinearNodes + x + y * (numx - 1) + z * (numx - 1)*numy + 1 << ", ";
							Output << numLinearNodes + numQuadNodes_1 + (x + 1) + y * numx + (z + 1) * numx*(numy - 1) + 1 << ", " << numLinearNodes + x + (y + 1) * (numx - 1) + (z + 1) * (numx - 1)*numy + 1 << ", ";
							Output << numLinearNodes + numQuadNodes_1 + x + y * numx + (z + 1) * numx*(numy - 1) + 1 << ", " << numLinearNodes + x + y * (numx - 1) + (z + 1) * (numx - 1)*numy + 1 << ", ";
							Output << numLinearNodes + numQuadNodes_1 + numQuadNodes_2 + (x + 1) + y * numx + z * numx*numy + 1 << ", " << numLinearNodes + numQuadNodes_1 + numQuadNodes_2 + (x + 1) + (y + 1)*numx + z * numx*numy + 1 << ", ";
							Output << numLinearNodes + numQuadNodes_1 + numQuadNodes_2 + x + (y + 1) * numx + z * numx*numy + 1 << ", " << numLinearNodes + numQuadNodes_1 + numQuadNodes_2 + x + y * numx + z * numx*numy + 1 << "\n ";
						}
						else if (Filetype == SCIRUN_EXPORT)
						{
							Output << x + y * numx + z * numx*numy + 1 << ", " << (x + 1) + y * numx + z * numx*numy + 1 << ", ";
							Output << x + y * numx + (z + 1)*numx*numy + 1 << ", " << (x + 1) + y * numx + (z + 1)*numx*numy + 1 << ", ";
							Output << x + (y + 1)*numx + z * numx*numy + 1 << ", " << (x + 1) + (y + 1)*numx + z * numx*numy + 1 << ", ";
							Output << x + (y + 1)*numx + (z + 1)*numx*numy + 1 << ", " << (x + 1) + (y + 1)*numx + (z + 1)*numx*numy + 1 << "\n";
						}
						else  // VTU export
						{
							vector<int> Indices;
							Indices.push_back(x + y * numx + z * numx*numy);
							Indices.push_back((x + 1) + y * numx + z * numx*numy);
							Indices.push_back((x + 1) + y * numx + (z + 1)*numx*numy);
							Indices.push_back(x + y * numx + (z + 1)*numx*numy);
							Indices.push_back(x + (y + 1)*numx + z * numx*numy);
							Indices.push_back((x + 1) + (y + 1)*numx + z * numx*numy);
							Indices.push_back((x + 1) + (y + 1)*numx + (z + 1)*numx*numy);
							Indices.push_back(x + (y + 1)*numx + (z + 1)*numx*numy);
							m_Mesh.AddElement(CMesh::HEX, Indices);
						}
						++iElementNumber;
						if (bOutputYarn && !bOutputMatrix) // Just saving yarn so need to make element array with just yarn info
						{
							NewElementInfo.push_back(*itElementInfo);
						}
					}
					++itElementInfo;  // Only saved element info for elements within domain outline
				}
			}
		}
	}


	if (bOutputYarn && !bOutputMatrix)
	{
		m_ElementsInfo.clear();
		m_ElementsInfo = NewElementInfo;
	}
	return (iElementNumber - 1);
}

*/
