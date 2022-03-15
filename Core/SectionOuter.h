#pragma once
#include "Section.h"

namespace TexGen
{
	using namespace std;

	/// Outer Scection for Dual yarn
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


		// Accessor methods
		double GetWidth() const { return m_dWidth; }
		double GetHeight() const { return m_dHeight; }
		void SetWidth(double dWidth) { m_dWidth = dWidth; }


	protected:
		double m_dWidth, m_dHeight;



	};

};	// namespace TexGen