/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under both the BSD-style license (found in the
 * LICENSE file in the root directory of this source tree) and the GPLv2 (found
 * in the COPYING file in the root directory of this source tree).
 */
#include "Options.h"
// #include "util.h"
#include "utils/ScopeGuard.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <thread>
#include <vector>
#include <mutex>  
#include <condition_variable> 


namespace pzstd {

namespace {
unsigned defaultNumThreads() {
#ifdef PZSTD_NUM_THREADS
  return PZSTD_NUM_THREADS;
#else
  return std::thread::hardware_concurrency();
#endif
}

const std::string kZstdExtension = ".zst";
constexpr char kStdIn[] = "-";
constexpr char kStdOut[] = "-";
constexpr unsigned kDefaultCompressionLevel = 3;
constexpr unsigned kMaxNonUltraCompressionLevel = 19;

#ifdef _WIN32
const char nullOutput[] = "nul";
#else
const char nullOutput[] = "/dev/null";
#endif

// void notSupported(const char *option) {
//   std::fprintf(stderr, "Operation not supported: %s\n", option);
// }

} // anonymous namespace

Options::Options()
    : numThreads(defaultNumThreads()), maxWindowLog(23),
      compressionLevel(kDefaultCompressionLevel), decompress(false),
      overwrite(false), keepSource(true), writeMode(WriteMode::Auto),
      checksum(true), verbosity(2) {}

Options::Status Options::parse(Options_Args args) {

  bool ultra = false;   //TODO: 保留使用

  if(args.numThreads <= 0)
    numThreads = defaultNumThreads();
  else
    numThreads = args.numThreads;

  maxWindowLog = args.maxWindowLog;
  compressionLevel = args.compressionLevel;
  decompress = args.decompress;
  

  overwrite = args.overwrite;
  keepSource = args.keepSource;
  
  switch (args.writeMode)
  {
    case 0:
      writeMode = WriteMode::Regular;
      break;
    
    case 1:
      writeMode = WriteMode::Auto;
      break;

    case 2:
      writeMode = WriteMode::Sparse;
      break;

    default:
      break;
  }
  
  checksum = args.checksum;
  verbosity = args.verbosity;
  inputBuffer = args.input_buffer;
  input_len = args.input_len;
  outputFile = args.outputFile;
  id = args.id;
  isLast = args.isLast;
  data_type = args.data_type;

  {
    unsigned maxCLevel =
        ultra ? ZSTD_maxCLevel() : kMaxNonUltraCompressionLevel;
    if (compressionLevel > maxCLevel || compressionLevel == 0) {
      std::fprintf(stderr, "Invalid compression level %u.\n", compressionLevel);
      return Status::Failure;
    }
  }

  // Check that numThreads is set
  if (numThreads == 0) {
    std::fprintf(stderr, "Invalid arguments: # of threads not specified "
                         "and unable to determine hardware concurrency.\n");
    return Status::Failure;
  }

  return Status::Success;
}

}
