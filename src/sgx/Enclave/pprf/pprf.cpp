/**
 * This program outputs a random value for electing a leader 
 * and a punctured secret value based on the secret value obtained as an input.
 * The punctured secret value is generated by convolutional encoding.
*/

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include "prvhash_core.h"
//#include "cmdline.h"
#include "viterbi.h"
#include "sgx_trts.h"

using namespace std;

class PPRF {
public:
    PPRF(){}
    uint64_t hexToDecimal(std::string); // converting from hex string to decimal value (uint64_t)
    std::vector<char> hexToBinary(std::string); // converting from hex string to Binary array (vector<char>)
    std::string prf(uint64_t *, uint64_t *, uint64_t *); // pseudo-random function
    std::string puncturing(std::string); // puncturing the bit string by convolutional coding
    std::string depuncturing(std::string); // depuncturing the bit string by viterbi decorder
    std::string decimalToBinary(uint64_t); // converting from decimal value (uint64_t) to binary string
    std::string binaryToHex(std::string); // converting from binary string to hex string
    std::vector<std::string> splitBinaryVector(std::string&, int); 
    std::string bitFlip(const std::string&); // random bit flips
    std::string align(std::string);
private:
    int keySize = 0;
    bool debug = false;
};



/* pseudo-random function */
std::string PPRF::prf(uint64_t *seed, uint64_t *lcg, uint64_t *hash){        
    uint64_t ret = 0;
    uint64_t i;
    // generating a random value by prf
    for ( i = 0; i < ( 1ULL << 28); i++) {
        ret = prvhash_core64(seed, lcg, hash);
    }    

    return align(decimalToBinary(ret));

}

/* puncturing a random value bit string using convolutional erasure code + bit flips */
std::string PPRF::puncturing(std::string rvalue) {
    std::vector<std::string> splvec;
    std::vector<int> polynomials;
    int constraint = 3;
    std::ostringstream oss;
    std::string rval_str = rvalue;
    std::string enc;
    std::string punc;
    std::string result;

    polynomials.push_back(6);
    polynomials.push_back(5);
    ViterbiCodec codec(constraint, polynomials);

//    rval_str = decimalToBinary(rvalue); // converted from unit64_t to binary vector
    keySize = rval_str.size();

//    std::cout << "size: " << keySize << " key: " << rval_str << std::endl; // for debug

    splvec = splitBinaryVector(rval_str, 7);

    
    for (auto itr = splvec.begin(); itr != splvec.end(); ++itr) {
        enc = codec.Encode(*itr);
        punc = bitFlip(enc); // encoding and bit flipping
        result += punc;
        if(debug){
//            cout << "vec: " << *itr << endl; // for debug
//            cout << " enc: " << enc;  // for debug
            //cout << " hex: " << binaryToHex(codec.Encode(*itr));
//            cout << " punc: " << punc << endl; // for debug
        }
    }

    //std::cout << "punctured: " << codec.Encode(rval_str) << std::endl;

    return result;

}

/* depuncturing a bit string by viterbi decoder */
std::string PPRF::depuncturing(std::string bits) {
    std::vector<std::string> splvec;
    std::vector<int> polynomials;
    int constraint = 3;
    std::ostringstream oss;
    std::string rval_str = bits;
    std::string dec;
    std::string punc;
    std::string result;

    polynomials.push_back(6);
    polynomials.push_back(5);
    ViterbiCodec codec(constraint, polynomials);

    keySize = rval_str.size();

    splvec = splitBinaryVector(rval_str, 18);

    for (auto itr = splvec.begin(); itr != splvec.end(); ++itr) {
        dec = codec.Decode(*itr);
        result += dec;
        if(debug){
//            cout << "vec: " << *itr << endl; // for debug
//            cout << " dec: " << dec;  // for debug
        }
    } 

    return result;

}

/* random bit flips */
std::string PPRF::bitFlip(const std::string& inbits){
    string bits = inbits;
    uint32_t rdm;
    int insize = bits.size();
    int pos, offset;
    sgx_read_rand((unsigned char *) &rdm, 4);
    pos = rdm % insize;
    offset = (rdm % insize/3) * 2;

    if (bits[pos] == '0'){
        bits[pos] = '1';
    } else {
        bits[pos] = '0';
    }

    if (bits[(pos + offset) % insize] == '0') {
        bits[(pos + offset) % insize] = '1';
    } else {
        bits[(pos + offset) % insize] = '0';
    }

    return bits;

}

/* converting from string to unit64_t */
uint64_t PPRF::hexToDecimal(std::string sarg) {
    uint64_t ret = 0;
    uint64_t base = 1;
    int len = sarg.length();

    for (int i = len - 1; i >= 0; i--) {
        if (sarg[i] >= '0' && sarg[i] <= '9') {
            ret += (sarg[i] - '0') * base;
        } else if (sarg[i] >= 'a' && sarg[i] <= 'f') {
            ret += (sarg[i] - 'a' + 10) * base;
        } else if (sarg[i] >= 'A' && sarg[i] <= 'F') {
            ret += (sarg[i] - 'A' + 10) * base;
        }
        base *= 16;
    }

    return ret;
}

/* converting from hex string to binary vector */
std::vector<char> PPRF::hexToBinary (std::string inHex){
    char tmp[256], out[256];
    long b = strtol(tmp, NULL, 16);
    int i = 0;
    std::vector<char> v;

    while(b) {
        out[i++] = b%2 + '0';
        b /= 2;
    }

    while(i--) {
        v.push_back(out[i]);
    }

    return v;
}

/* converting from binary vector to hex string */
std::string PPRF::binaryToHex(std::string inBin) {
    std::string hexStr = "";
    int len = inBin.length();

    // Add leading zeros if necessary to make string length a multiple of 4
    while (len % 4 != 0) {
        inBin = "0" + inBin;
        len++;
    }

    // Convert binary to hex by processing 4 bits at a time
    for (int i = 0; i < len; i += 4) {
        uint8_t nibble = 0;
        for (int j = 0; j < 4; j++) {
            nibble = (nibble << 1) + (inBin[i+j] - '0');
        }
        if (nibble < 10) {
            hexStr += (char)(nibble + '0');
        } else {
            hexStr += (char)(nibble - 10 + 'a');
        }
    }

    return hexStr;
}

/* split vector in to a small vector of a certain length */
std::vector<std::string> PPRF::splitBinaryVector (std::string& vec, int num){

    vector<string> result;
 
    for (size_t i = 0; i < vec.size(); i += num) {
        result.push_back(vec.substr(i, num));
    }
 
    //padding with 0
    if (result[result.size() - 1].size() < num) {
        for(int remaining = num - result[result.size() - 1].size(); remaining < 0; remaining--) {
            result[result.size() - 1] += '0';
        }

    }

    return result;

}

std::string PPRF::decimalToBinary(uint64_t n) {
 
    std::string result;
    while (n != 0){
        result += n % 2 == 0 ? '0' : '1' ;
        n /= 2;
    }
    reverse(result.begin(), result.end());
    return result;
}

std::string PPRF::align(const std::string bits){
    std::string rval_str = bits;
    std::string result;
    int keySize = rval_str.size();
    int remain = keySize % 7; // currently key size should be a multiples of 7
    result = rval_str.erase(keySize - remain);

    if(debug){
//        std::cout << "input size: " << keySize << " key: " << rval_str << std::endl; // for debug
//        std::cout << "output size: "<< result.size() << "key: " << result << std::endl; // for debug
    }

    return result;

}

/* Test code for using the class PPRF */
/*
int main(int argc, char* argv[]){
//    const uint64_t defaultSeed = 0x93064E905C127FE5;
    const uint64_t defaultSeed = 0xF3D6FE905C127FE5;
    const uint64_t defaultHash = 0xA4BDE5C4A05E6256;
    const uint64_t defaultlcg = 0;
    int keySize = 0;

    uint64_t seed = defaultSeed; // secret 
    uint64_t lcg = defaultlcg;
    uint64_t hash = defaultHash; // key 
    string v = "";
    string result;
    string depunct;

    PPRF pr = PPRF();
    
    //std::string in_seed;
    //std::string in_hash;

    // for analyzing command line arguments 
    cmdline::parser cla;
    cla.add<string>("secret", 's', "secret value", false);
    cla.add<string>("key", 'k', "key value", false);
    cla.add("help", 0, "print usage");

    if(!cla.parse(argc, argv) || cla.exist("help")) {
        std::cout << cla.error_full() << cla.usage();
        return 0;
    }

    if (cla.exist("secret")) {
        //in_seed = argv[2]; //p.get<std::string>("secret"); 
        seed = pr.hexToDecimal(argv[2]);
    }

    if(cla.exist("key")) {
        // hash = hexToDecimal(p.get<std::string>("key")); 
        //in_hash = argv[4];
        hash = pr.hexToDecimal(argv[4]);
    }



    cout << "secret: " << seed << endl;

    v = pr.prf(&seed, &lcg, &hash);

    cout << "key (decimal): " << v << endl; 

    result = pr.puncturing(v);

    cout << "punctured key: " << result << endl;

    depunct = pr.depuncturing(result);

    cout << "depunctured key: " << depunct << endl;

}
*/

