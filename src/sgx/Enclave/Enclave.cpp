#include "Enclave_t.h"
#include <sgx_trts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
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


int ecall_election(const int *num_nodes) //TODO: seckey should not be received here
{	
	std::string result;
	char * ret;
	SSLE_Obfuscation ssleobf;

	ssleobf.initialize(num_nodes);

	ssleobf.electLeader();

	result = ssleobf.getPunctKey();
	memcpy(ret, result.c_str(), result.length());

	ocall_print(ret);
	return 0;
}