using namespace std;
class PPRF {
public:
uint64_t hexToDecimal(string); // converting from hex string to decimal value (uint64_t)
vector<char> hexToBinary(string); // converting from hex string to Binary array (vector<char>)
string prf(uint64_t *, uint64_t *, uint64_t *); // pseudo-random function
string puncturing(string); // puncturing the value by convolutional coding
string depuncturing(string); // depuncturing a bit string by viterbi decoder
string decimalToBinary(uint64_t); // converting from decimal value (uint64_t) to binary string
string binaryToHex(string); // converting from binary string to hex string
vector<string> splitBinaryVector(string&, int); 
string bitFlip(const string&); // random bit flips
};
