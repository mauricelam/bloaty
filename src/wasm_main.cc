#include <emscripten.h>
#include <emscripten/bind.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "bloaty.h"

using namespace emscripten;

extern "C" {

/**
 * Entry point for Bloaty from JavaScript.
 * Mimics the command-line interface.
 *
 * @param argc Number of arguments
 * @param argv Array of argument strings
 * @return String output from Bloaty, or error message
 */
EMSCRIPTEN_KEEPALIVE
const char* bloaty_run(int argc, char** argv) {
  bloaty::Options options;
  bloaty::OutputOptions output_options;
  std::string error;

  if (!bloaty::ParseOptions(false, &argc, &argv, &options, &output_options, &error)) {
    if (!error.empty()) {
      static std::string err_out;
      err_out = "bloaty: " + error;
      return err_out.c_str();
    } else {
      return ""; // --help or similar
    }
  }

  bloaty::RollupOutput output;
  bloaty::MmapInputFileFactory mmap_factory;
  try {
    if (!bloaty::BloatyMain(options, mmap_factory, &output, &error)) {
      static std::string err_out;
      if (!error.empty()) {
        err_out = "bloaty: " + error;
      } else {
        err_out = "bloaty: unknown error";
      }
      return err_out.c_str();
    }
  } catch (const bloaty::Error& e) {
    static std::string err_out;
    err_out = "bloaty: " + std::string(e.what());
    return err_out.c_str();
  } catch (const std::exception& e) {
    static std::string err_out;
    err_out = "bloaty: " + std::string(e.what());
    return err_out.c_str();
  }

  static std::string result;
  if (!options.dump_raw_map()) {
    std::stringstream ss;
    output.Print(output_options, &ss);
    result = ss.str();
  } else {
    result = "Raw map dump not supported in WASM yet";
  }
  return result.c_str();
}

}

/**
 * Convenience wrapper for easier use from JS when using embind.
 */
std::string run_bloaty(std::vector<std::string> args) {
  std::vector<char*> argv;
  for (const auto& arg : args) {
    argv.push_back(const_cast<char*>(arg.c_str()));
  }
  return bloaty_run(argv.size(), argv.data());
}

EMSCRIPTEN_BINDINGS(bloaty) {
  register_vector<std::string>("StringVector");
  function("run_bloaty", &run_bloaty);
}
