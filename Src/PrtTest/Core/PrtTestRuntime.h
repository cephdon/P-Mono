#ifndef PRT_TEST_RUNTIME_H
#define PRT_TEST_RUNTIME_H

#include "Prt.h"

#ifdef __cplusplus
extern "C"{
#endif

PRT_API PRT_MACHINEINST * PRT_CALL_CONV
PrtMkMachineTest(
_Inout_  PRT_PROCESS			*process,
_In_  PRT_UINT32				instanceOf,
_In_  PRT_VALUE					*payload
);

PRT_API void PRT_CALL_CONV
PrtSendTest(
	_Inout_ PRT_MACHINEINST			*context,
	_In_ PRT_VALUE					*event,
	_In_ PRT_VALUE					*payload
);

void 
TestMain(PRT_PROGRAMDECL *program, PRT_UINT32 main_machine);

#ifdef __cplusplus
}
#endif
#endif