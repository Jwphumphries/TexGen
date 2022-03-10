
#pragma once
#include "Yarn.h"

namespace TexGen
{
	using namespace std;

	class CLASS_DECLSPEC CDualYarn : public CYarn
	{
	public:
		CDualYarn(void);
		//CDualYarn(TiXmlElement &Element);
		~CDualYarn(void);

		void AssignSection();

		

	};

} // namespace TexGen
