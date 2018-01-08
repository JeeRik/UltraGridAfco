/* 
 * File:   sandbox.cpp
 * Author: maara
 *
 * Created on February 6, 2017, 11:16 AM
 */
#include <algorithm>
#include <cstring>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <iterator>
#include <ctime>
#include <thread>
#include <queue>
#include <iomanip>

#include "SocketHandler.h"
#include "StringSocket.h"
#include "Tools.h"
#include "ComHandler.h"
#include "Main.h"
#include "UgAdapter.h"
#include "BlockWarnBuffer.h"

using namespace std;

int SERVER_PORT = 12345;

template <typename T> std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
  if ( !v.empty() ) {
    out << '[';
    std::copy (v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
    out << "\b\b]";
  }
  return out;
}

void start(int argc, char** argv) {
    (void) argc;
    (void) argv;

    ComHandler com = ComHandler(12347, 12345, 12346, "localhost");
    if (com.init()) {
        std::cerr << "Failed to initialize ComHandler, exiting" << std::endl;
        exit(1);
    }

    UgAdapter ug(com, {"uv"});

    InMessage msg;
    Main main(com);

    main.main();
}

int main(int argc, char** argv) {
    (void) argc;
    (void) argv;

    start(argc, argv);

}

