#include <stdio.h>
#include <wchar.h>

#include "krt.h"

int main(int argc, char **argv)
{
	wprintf(L"%s", L"Hello from KVM.\n");
	getwchar();
	return 0;
}
