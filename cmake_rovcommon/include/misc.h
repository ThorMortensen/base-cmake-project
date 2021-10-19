//
// Created by thm on 07/06/2021.
//

#pragma once
#include <random>
#include <memory>

namespace rov {


  static std::random_device rd; //Will be used to obtain a seed for the random number engine
  static std::mt19937 gen(rd());//Standard mersenne_twister_engine seeded with rd()


  inline std::string getRandomStr(const size_t len) {

    std::string tmp_s;
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    //  srand( (unsigned) time(NULL) * getpid());
    std::uniform_int_distribution<> distribStr(0, (sizeof(alphanum) - 1));

    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i)
      tmp_s += alphanum[distribStr(gen)];
    return tmp_s;
  }

  template<typename T>
  inline T getRandom(T from, T to) {
    std::uniform_int_distribution<> distrib(from, to);
    return distrib(gen);
  }


}// namespace rov