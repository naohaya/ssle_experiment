#include <iostream>
#include <bitset>
#include <string>
#include <sstream>
//#include <boost/dynamic_bitset.hpp> /* corresponding to binStringtoDynamicBitset() */

/* convert uint32_t into string */
std::string binaryToString(uint32_t bs) {
  std::stringstream ss;
  for (int i = 0; i < 32; i++) {
    ss << ((bs & (1 << (32 - i - 1))) ? 1 : 0);
  }
  return ss.str();
}

/* convert string into uint32_t */
uint32_t stringToUint32(std::string binaryString) {
  std::bitset<32> binaryNum(binaryString); // convert string into bitset
  uint32_t uint32Num = static_cast<uint32_t>(binaryNum.to_ulong()); // convert bitset into uint32_t

  return uint32Num;
}

// uint32_tを2進数のstringに変換する関数
std::string uint32ToBinString(uint32_t num) {
  std::bitset<32> binaryNum(num); // uint32_tをbitsetに変換する
  std::string binaryString = binaryNum.to_string(); // bitsetをstringに変換する

  return binaryString;
}


/* convert binary string into hex string 
* Note: output the binary from (length) bits from the tail.
*/ 
std::string binaryToHex(std::string binaryString, int bitLength) {
  std::bitset<32> binaryNum(binaryString); // stringをbitsetに変換する
  uint32_t uint32Num = static_cast<uint32_t>(binaryNum.to_ulong()); // bitsetをuint32_tに変換する
  uint32Num = uint32Num << (32 - bitLength); // bitLengthだけの長さに拡張する
  uint32Num = uint32Num >> (32 - bitLength); // bitLengthだけの長さに切り詰める

  std::stringstream ss;
  ss << std::hex << uint32Num; // uint32_tを16進数のstringに変換する
  std::string hexString = ss.str();

  return hexString;
}

/* convert hex string into binary string 
* Note: output the binary from (length) bits from the tail.
*/ 
std::string hexToBinary(std::string hexString, int bitLength) {
  uint32_t uint32Num = std::stoul(hexString, nullptr, 16); // 16進数のstringをuint32_tに変換する
  std::bitset<32> binaryNum(uint32Num); // uint32_tをbitsetに変換する

  std::string binaryString = binaryNum.to_string(); // bitsetをstringに変換する
  binaryString = binaryString.substr(binaryString.size() - bitLength); // bitLengthだけの長さに切り詰める

  return binaryString;
}

/* convert binary string into dynamic_bitset */
// without using dynamic_bitset
std::bitset<64> binStringToBitset(const std::string& binaryString)
{
    std::bitset<64> bitset;
    int index = 0;
    for (auto it = binaryString.crbegin(); it != binaryString.crend(); ++it)
    {
        if (*it == '1')
        {
            bitset.set(index);
        }
        ++index;
    }
    return bitset;
}

//using dynamic_bitset
/*
boost::dynamic_bitset<> binStringToDynamicBitset(std::string binaryString) {
  boost::dynamic_bitset<> dynamicBitset(binaryString); // stringをdynamic_bitsetに変換する

  return dynamicBitset;
}
*/
