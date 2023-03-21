#include "Enclave_t.h"
#include <sgx_trts.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include "Node.h"
#include "pprf/pprf.h"
#include "ssle/SSLE_Obfuscation.h"

int ecall_test(const char *message, size_t message_len)
{
	ocall_print(message);

	return 31337;
}


uint64_t ecall_election(uint64_t sec, std::vector<Node> nodes)
{
	SSLE_Obfuscation ssleobf;

	ssleobf.initialize(sec, nodes);

	ssleobf.electLeader();

	return ssleobf.getPunctKey();
}