#include "assetLoaderUtility.h"
#include <algorithm>
#include <cctype>



namespace emberAssetLoader
{
	namespace Utility
	{
		void TrimLeadingWhitespace(std::string& str)
		{
			str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch)
			{ return !std::isspace(ch); }));
		}
	}
}