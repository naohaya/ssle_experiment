#include "stdlib.h"
#include "sgx_trts.h"

class SSLE
{
    public:
        virtual void initialize();

        virtual void electLeader();

        virtual static int verifyLeader();
};
