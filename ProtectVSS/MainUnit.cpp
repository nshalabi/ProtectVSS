//
// +---------+----------------------------------------------------------------+
// | AUTHOR  | NADER SHALLABI                                                 |
// +---------+----------------------------------------------------------------+
// | CONTACT | nader@nosecurecode.com                                         |
// +--------------------------------------------------------------------------+
// | This sample code is free for use, redistributionand /or                  |
// | modification without any explicit permission from the author.            |
// |                                                                          |
// | This sample code is distributed in the hope that it will be useful,      |
// | but WITHOUT ANY WARRANTY, implied or explicit.                           |
// +--------------------------------------------------------------------------+
//

#include <ntifs.h>
#include <ntddk.h>

// Defines
#define PREFIXTAG "VSSPROTECT::"
#define PVSS_TAG 'PVSS'

// Prototypes
DRIVER_UNLOAD DriverUnload;
void          OnProcessNotify(_Inout_ PEPROCESS Process, _In_ HANDLE ProcessId, _Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo);

// Entry point
extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) 
{ 
	UNREFERENCED_PARAMETER(RegistryPath);

	auto status = STATUS_SUCCESS;

	// register for process notifications
	status = PsSetCreateProcessNotifyRoutineEx(OnProcessNotify, FALSE);
	if (!NT_SUCCESS(status))
	{
		KdPrint((PREFIXTAG "Failed to register a process callback notification (0x%08X) \n ", status));
	}

	DriverObject->DriverUnload = DriverUnload;
	
	return status;
}

// Driver unloading
void DriverUnload(PDRIVER_OBJECT DriverObject) 
{
	UNREFERENCED_PARAMETER(DriverObject);

	// unregister process notifications 
	NTSTATUS status = PsSetCreateProcessNotifyRoutineEx(OnProcessNotify, TRUE);
	if (!NT_SUCCESS(status)) {
		KdPrint((PREFIXTAG "failed to unregister process callback (0x%08X)\n", status));
	}
} 


// Helper function to look for vssadmin process
NTSTATUS CheckForVssAdminProcess(_In_ PCUNICODE_STRING pstrConstCommandLine)
{
	NTSTATUS Status = STATUS_UNSUCCESSFUL;
	UNICODE_STRING strCommandLine;

	KdPrint((PREFIXTAG "Entering...\n"));

	if (!pstrConstCommandLine || !pstrConstCommandLine->Buffer) 
	{
		KdPrint((PREFIXTAG "No Command line was provided\n"));

		return Status;
	}
	else 
	{
		KdPrint((PREFIXTAG "Checking for vssadmin process...\n"));
	}

	// Before copy, make sure we are ignoring case
	Status = RtlUpcaseUnicodeString(&strCommandLine, pstrConstCommandLine, TRUE);
	if (Status != STATUS_SUCCESS)
	{
		KdPrint((PREFIXTAG "Error converting buffer to uppercase\n"));
		return Status;
	}

	// Reset status
	Status = STATUS_UNSUCCESSFUL;

	//////////////////////////////////////////////////////////
	// THIS CAN BE MODIFIED TO ASSUME SEVERAL ALTERNATIONS  //
	//////////////////////////////////////////////////////////
	// We do not want to block vssadmin, we are mostly 
	// concerned with "vssadmin delete shdaows" command

	if (
		(NULL != wcsstr(strCommandLine.Buffer, L"VSSADMIN")) && 
		(NULL != wcsstr(strCommandLine.Buffer, L"DELETE"))
		)
	{
		KdPrint((PREFIXTAG "Match FOUND...\n"));
		Status = STATUS_SUCCESS;
	}
	else
	{
		KdPrint((PREFIXTAG "NO Match found...\n"));
	}

	// Cleanup
	RtlFreeUnicodeString(&strCommandLine);

	return Status;
}

// Code to terminate parent process
NTSTATUS TerminateParentProcess(ULONG ProcessID)
{
	NTSTATUS          Status = STATUS_SUCCESS;
	HANDLE            hProcess;
	OBJECT_ATTRIBUTES ObjectAttributes;
	CLIENT_ID         ClientId;

	KdPrint(("Entering TerminateParentProcess", ProcessID));

	InitializeObjectAttributes(&ObjectAttributes, NULL, OBJ_INHERIT, NULL, NULL);

	ClientId.UniqueProcess = (HANDLE) ProcessID;
	ClientId.UniqueThread = NULL;

	Status = ZwOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &ObjectAttributes, &ClientId);
	if (NT_SUCCESS(Status))
	{
		Status = ZwTerminateProcess(hProcess, 0);
		if (!NT_SUCCESS(Status))
		{
			KdPrint(("ZwTerminateProcess failed with status : %08X\n", Status));
		}
		else
		{
			Status = ZwClose(hProcess);
			if (!NT_SUCCESS(Status))
			{
				KdPrint(("ZwClose failed with status : %08X\n", Status));
			}
		}
	}
	else
	{
		KdPrint(("ZwOpenProcess failed with status : %08X\n", Status));
		Status = STATUS_UNSUCCESSFUL;
	}

	return Status;
}

// Process notification
void OnProcessNotify(PEPROCESS Process, HANDLE ProcessId, PPS_CREATE_NOTIFY_INFO CreateInfo)
{
	UNREFERENCED_PARAMETER(Process);

	NTSTATUS Status;

	if (CreateInfo) 
	{
		// process create 

		auto CmdLength = CreateInfo->CommandLine->Length;

		if (CmdLength > 0)
		{
			// Check if this is targetting VSS
			if (CreateInfo->CommandLine != NULL)
			{
				KdPrint((PREFIXTAG "Process (PID : %ul) created (CMD : %wZ)\n", HandleToULong(ProcessId), CreateInfo->CommandLine));

				Status = CheckForVssAdminProcess(CreateInfo->CommandLine);

				if (Status == STATUS_SUCCESS)
				{
					KdPrint((PREFIXTAG "Process (PID 0x%p) REJECTED (STATUS_ACCESS_DENIED)\n", (PVOID)ProcessId));

					// Deny access to vssadmin process
					CreateInfo->CreationStatus = STATUS_ACCESS_DENIED;

					// Terminate the parent process
					
					KdPrint((PREFIXTAG "Terminating parent process (PID 0x%p)...\n", CreateInfo->ParentProcessId));

					TerminateParentProcess(HandleToULong(CreateInfo->ParentProcessId));
				}
			}
		}
	}
	else
	{ 
		// Else, this would be a notification about a process exit event
	}
}