#include "Enclave_t.h"
#include <sgx_trts.h>
#include <stdio.h>
//#include <iostream>
//#include <vector>
//#include "Node.h"
//#include "pprf/pprf.h"
#include "ssle/SSLE_Obfuscation.h"

int ecall_test(const char *message, size_t message_len)
{
	ocall_print(message);

	return 31337;
}


string ecall_election(const int *num_nodes) //TODO: seckey should not be received here
{
	SSLE_Obfuscation ssleobf;

	ssleobf.initialize(num_nodes);

	ssleobf.electLeader();

	return ssleobf.getPunctKey();
}