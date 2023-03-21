/**
 * This program is for experiments of SSLE without SGX.
 *
 */

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include "Node.h"
#include "pprf.h"
//#include "commitment.h"
#include "error_print.h"
#include "Enclave_u.h"
#include <sgx_urts.h>
#include "error_print.h"

Node registration(std::string);
void ocall_print(const char *);

sgx_enclave_id_t global_eid = 0; // enclave initialization

/* Enclave initialization */
int initialize_enclave()
{
    std::string launch_token_path = "enclave.token";
    std::string enclave_name = "enclave.signed.so";
    const char *token_path = launch_token_path.c_str();

    sgx_launch_token_t token = {0};
    sgx_status_t status = SGX_ERROR_UNEXPECTED;
    int updated = 0;

    /*==============================================================*
     * Step 1: Obtain enclave launch token                          *
     *==============================================================*/

    /* If exist, load the enclave launch token */
    FILE *fp = fopen(token_path, "rb");

    /* If token doesn't exist, create the token */
    if (fp == NULL && (fp = fopen(token_path, "wb")) == NULL)
    {
        /* Storing token is not necessary, so file I/O errors here
         * is not fatal
         */
        std::cerr << "Warning: Failed to create/open the launch token file ";
        std::cerr << "\"" << launch_token_path << "\"." << std::endl;
    }

    if (fp != NULL)
    {
        /* read the token from saved file */
        size_t read_num = fread(token, 1, sizeof(sgx_launch_token_t), fp);

        /* if token is invalid, clear the buffer */
        if (read_num != 0 && read_num != sizeof(sgx_launch_token_t))
        {
            memset(&token, 0x0, sizeof(sgx_launch_token_t));

            /* As aforementioned, if token doesn't exist or is corrupted,
             * zero-flushed new token will be used for launch.
             * So token error is not fatal.
             */
            std::cerr << "Warning: Invalid launch token read from ";
            std::cerr << "\"" << launch_token_path << "\"." << std::endl;
        }
    }

    /*==============================================================*
     * Step 2: Initialize enclave by calling sgx_create_enclave     *
     *==============================================================*/

    status = sgx_create_enclave(enclave_name.c_str(), SGX_DEBUG_FLAG, &token,
                                &updated, &global_eid, NULL);

    if (status != SGX_SUCCESS)
    {
        /* Defined at error_print.cpp */
        sgx_error_print(status);

        if (fp != NULL)
        {
            fclose(fp);
        }

        return -1;
    }
    /*==============================================================*
     * Step 3: Save the launch token if it is updated               *
     *==============================================================*/

    /* If there is no update with token, skip save */
    if (updated == 0 || fp == NULL)
    {
        if (fp != NULL)
        {
            fclose(fp);
        }

        return 0;
    }

    /* reopen with write mode and save token */
    fp = freopen(token_path, "wb", fp);
    if (fp == NULL)
        return 0;

    size_t write_num = fwrite(token, 1, sizeof(sgx_launch_token_t), fp);

    if (write_num != sizeof(sgx_launch_token_t))
    {
        std::cerr << "Warning: Failed to save launch token to ";
        std::cerr << "\"" << launch_token_path << "\"." << std::endl;
    }

    fclose(fp);

    return 0;
}

int main(int argc, char *argv[])
{

    std::vector<Node> nodes;
    Node thisNode;
    Node remoteNode;
    bool leader = false;
    int inconnect;
    int localport;
    int remoteport;
    std::ostringstream oss;

    if (argc == 4)
    {
        localport = std::atoi(argv[1]);
        remoteport = std::atoi(argv[2]);
        thisNode = Node("127.0.0.1", localport);
        remoteNode = Node("127.0.0.1", remoteport);

        if (std::atoi(argv[3]))
        {
            leader = true;
        }
    }
    else
    {
        std::cout << "./node <local port>, <remote port>, <flag>" << std::endl;
    }

    if (leader)
    { // what a leader does
        // preparing for information for pprf
        uint64_t secret = 0x93064E905C127FE5; // key
        uint64_t hash = 0xA4BDE5C4A05E6256;   // hash (constant)
        uint64_t lcg = 0;
        string rval;
        string punct;
        string comm;

        // register itself
        nodes.push_back(thisNode);

        /* initializing enclave */
        if (initialize_enclave() < 0)
        {
            std::cerr << "App: fatal error: Failed to initialize enclave.";
            std::cerr << std::endl;
            return -1;
        }

        /* starts ECALL */
        const char *message = "Hello Enclave.";
        size_t message_len = strlen(message);
        int retval = -9999;
        uint64_t pkey;

        std::cout << "Execute ECALL.\n"
                  << std::endl;

        sgx_status_t status = ecall_test(global_eid, &retval,
                                         message, message_len);

        sgx_status_t status2 = ecall_election(global_eid, &pkey, secret, 3); // for test

        if (status != SGX_SUCCESS)
        {
            sgx_error_print(status);

            return -1;
        }
        else
        {
            /* This function also can display succeeded message */
            sgx_error_print(status);
        }

        /* print ECALL result */
        std::cout << "\nReturned integer from ECALL is: " << retval << std::endl;
        std::cout << std::endl;



        /* Destruct the enclave */
        sgx_destroy_enclave(global_eid);

        std::cout << "Whole operations have been executed correctly." << std::endl;

        /* ends ECALL */


        thisNode.initialization();

        thisNode.listen_as_server();

        inconnect = thisNode.accept_as_server();

        // registration of participants
        thisNode.receive_message(inconnect);
        std::string str = thisNode.get_message();
        std::cout << "Leader received: " << str << std::endl;
        nodes.push_back(registration(str));
        std::cout << "Leader registered: " << nodes[nodes.size() - 1].ipaddr << ":" << nodes[nodes.size() - 1].portno << std::endl;

        // decide the next leader TODO

        // send information to participants
        str = punct;
        thisNode.send_message(inconnect, str, str.length(), 0); // send message to participants
        std::cout << "Leader sent: " << str << "(" << str.length() << ")" << std::endl;

        // send committed value
        comm = "test";
        thisNode.send_message(inconnect, comm, comm.length(), 0); // send message to participants
        std::cout << "Leader sent: " << comm << "(" << comm.length() << ")" << std::endl;

        thisNode.receive_message(inconnect);
        str = thisNode.get_message();
        std::cout << "Leader received: " << str << std::endl;

        // socket close
        thisNode.close_connection(inconnect); // close incoming connection
        thisNode.close_connection();          // close the listening port
    }
    else
    { // what a participant does
        remoteNode.initialization();

        remoteNode.connect_from_client();

        // データ送信
        std::string msg = thisNode.ipaddr + "," + std::to_string(thisNode.portno);
        remoteNode.send_message(msg, msg.length(), 0); // 送信
        std::cout << "Participant sent: " << msg << std::endl;

        // データ受信
        char r_str[162]; // receive buffer
                         //	    remoteNode.receive_message(r_str, 12, 0);
        remoteNode.receive_message();
        std::string received = remoteNode.get_message();
        std::cout << "Client get_messages(): " << received << "(" << received.length() << ")" << std::endl;

        // correction of the received punctured key
        int remain = received.length() % 18;
        received.erase(received.length() - remain, remain);
        std::cout << "corrected string: " << received << "(" << received.length() << ")" << std::endl;

        char c_str[162];
        remoteNode.receive_message();
        std::string comm = remoteNode.get_message();
        std::cout << "Participant received commitment: " << comm << std::endl;

        std::string rmsg = "hello";
        remoteNode.send_message(rmsg, rmsg.length(), 0);
        std::cout << "Participant sent: " << rmsg << std::endl;

        // socket close
        remoteNode.close_connection();
    }
    // socket close
    //	thisNode.close_connection(inconnect); // close incoming connection
    //	thisNode.close_connection(); // close the listening port
    //    if(!leader)
    //        remoteNode.close_connection();

    return 0;
}

Node registration(std::string str)
{
    int first = 0;
    char del = ',';
    int last = str.find_first_of(del);

    std::vector<std::string> result;

    while (first < str.size())
    {
        std::string subStr(str, first, last - first);

        result.push_back(subStr);

        first = last + 1;
        last = str.find_first_of(del, first);

        if (last == std::string::npos)
        {
            last = str.size();
        }
    }

    Node ret = Node(result[0], atoi(result[1].c_str()));

    return ret;
}

// Ocall implementation
/* OCALL implementations */
void ocall_print(const char *str)
{
    std::cout << "Output from OCALL: " << std::endl;
    std::cout << str << std::endl;

    return;
}
