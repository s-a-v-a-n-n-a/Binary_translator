// #include "Instructions_realizations.h"
#include "To_x86-64/Translation.h"


int main()
{
	translation_into_x86_64("binary.xex", "output");

	system("chmod +x ./output");

	return 0;
}
