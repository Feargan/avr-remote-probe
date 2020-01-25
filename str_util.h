#ifndef STR_UTIL_H_
#define STR_UTIL_H_

#include <string.h>

void str_replace(char *Str, const char *Search, const char *Replace)
{
	char* p, *temp=Str;
	uint8_t ReplaceSize = strlen(Replace);
	uint8_t StrSize = strlen(Str);
	uint8_t SearchSize = strlen_P(Search);
	while ((p = strstr_P(temp, Search)))
	{
		memmove(p + ReplaceSize, p+SearchSize, StrSize-(p-Str)-SearchSize+1);
		memcpy(p, Replace, ReplaceSize);
		temp = p;
		temp += ReplaceSize;
		StrSize += ReplaceSize - SearchSize;
	}
}

#endif /* STR_UTIL_H_ */