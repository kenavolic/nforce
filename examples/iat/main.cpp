// Copyright 2019 Ken Avolic <kenavolic@none.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

#include <Windows.h>

#include "nforce/expr.h"
#include "nforce/lexer.h"
#include "nforce/parser.h"

using namespace n4;

//
// Basic example used to apply filter to dll iat inspection results
//

namespace {
// data model to store iat analysis results
struct entry {
  std::string module;
  std::string name;
};

using entry_list = std::vector<entry>;

// generic rule
class regex_rule {
  std::string const &_ctxt;
  std::regex _regx;

public:
  regex_rule(std::string const &reg, std::string const &ctxt)
      : _regx{reg}, _ctxt{ctxt} {}

  bool do_handle(std::string const &str) const {
    return std::regex_match(str, _regx);
  }

  bool interpret(std::string const &str) const {
    std::regex reg{_regx};
    std::smatch matches;

    if (!std::regex_search(str, matches, reg) || matches.size() != 2) {
      throw std::runtime_error("bad module rule");
    }

    try {
      reg = matches[1].str();
    } catch (std::regex_error const &) {
      throw std::runtime_error("bad module rule");
    }

    return std::regex_match(_ctxt, reg);
  }
};

// build iat
auto rva2offset(unsigned long rva, PIMAGE_NT_HEADERS nt) {
  auto sec = IMAGE_FIRST_SECTION(nt);
  for (auto i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++sec) {
    // right section
    if (rva > sec->VirtualAddress &&
        rva < (sec->VirtualAddress + sec->Misc.VirtualSize)) {
      break;
    }
  }

  return (rva - sec->VirtualAddress + sec->PointerToRawData);
}

entry_list build(std::string const &bin_path) {
  std::ifstream f{bin_path, std::ios_base::binary};

  if (!f) {
    throw std::runtime_error("bad binary path");
  }

  std::vector<byte> buffer{std::istreambuf_iterator<char>{f},
                           std::istreambuf_iterator<char>{}};

  auto dos_h = reinterpret_cast<PIMAGE_DOS_HEADER>(buffer.data());
  if (IMAGE_DOS_SIGNATURE != dos_h->e_magic) {
    throw std::runtime_error("bad binary header");
  }

  auto nt_h =
      reinterpret_cast<PIMAGE_NT_HEADERS>(buffer.data() + dos_h->e_lfanew);
  if (IMAGE_NT_SIGNATURE != nt_h->Signature) {
    throw std::runtime_error("bad binary header");
  }

  entry_list iat;
  if (0 !=
      nt_h->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size) {
    auto desc = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(
        buffer.data() +
        rva2offset(
            nt_h->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]
                .VirtualAddress,
            nt_h));

    for (; desc->Name; ++desc) {
      auto thunk = reinterpret_cast<uintptr_t *>(
          (buffer.data() +
           rva2offset((desc->OriginalFirstThunk ? desc->OriginalFirstThunk
                                                : desc->FirstThunk),
                      nt_h)));

      for (; *thunk; ++thunk) {
        if (!IMAGE_SNAP_BY_ORDINAL(*thunk)) {
          iat.push_back({reinterpret_cast<char *>(buffer.data() +
                                                  rva2offset(desc->Name, nt_h)),
                         reinterpret_cast<char *>(
                             &((reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(
                                    buffer.data() + rva2offset(*thunk, nt_h)))
                                   ->Name))});
        }
      }
    }
  }

  return iat;
}

// apply rule
auto filter(entry_list const &raw, std::string const &filter) {
  // create an entry that will serves as reference in rules
  entry eref;
  auto mod_rule = regex_rule{"mod=(.*)", eref.module};
  auto name_rule = regex_rule{"name=(.*)", eref.name};

  lexer lexer{filter};
  parser parser{
      lexer,
      std::vector<parser::rule_handler>{
          {[&rule = mod_rule](auto const &str) { return rule.do_handle(str); },
           [&rule = mod_rule](auto const &str) { return rule.interpret(str); }},
          {[&rule = name_rule](auto const &str) { return rule.do_handle(str); },
           [&rule = name_rule](auto const &str) {
             return rule.interpret(str);
           }}}};

  auto expr = parser.build();

  entry_list filtered;
  std::copy_if(std::cbegin(raw), std::cend(raw), std::back_inserter(filtered),
               [&](auto const &r) {
                 eref.module = r.module;
                 eref.name = r.name;
                 return expr->interpret();
               });

  return filtered;
}

auto sort_iat(entry_list &&list) {
  auto sorted = std::move(list);
  std::sort(std::begin(sorted), std::end(sorted), [](auto r1, auto r2) {
    return (r1.module < r2.module) ||
           ((r1.module == r2.module) && (r1.name < r2.name));
  });

  return sorted;
}

// display filtered result
void display(entry_list const &list) {
  std::cout << "\n----------IAT----------" << std::endl;
  for (const auto &e : list) {
    std::cout << "[entry] " << e.module << "::" << e.name << std::endl;
  }
}
} // namespace

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "[-][iat] usage: iat bin_path" << std::endl;
    return 1;
  }

  std::cout << "---------- Welcome to iat explorer ----------" << std::endl;
  std::cout << "usage:" << std::endl;
  std::cout << " - enabled rules are: mod=.* and name=.*" << std::endl;
  std::cout << " - example: 'mod=KERN.*' & !'name=Get.*'" << std::endl;
  std::cout << "---------------------------------------------" << std::endl;

  entry_list raw_iat;
  try {
    raw_iat = sort_iat(build(argv[0]));
  } catch (const std::exception &e) {
    std::cerr << "[-][iat] dll loading failed with error : " << e.what()
              << std::endl;
    return 1;
  }

  const std::string query = "\nenter a filter, f for full iat or q to quit: ";
  for (std::string in = (std::cout << query, "");
       std::getline(std::cin, in) && in != "q"; std::cout << query) {
    try {
      if (in == "f") {
        display(raw_iat);
      } else {
        display(sort_iat(filter(raw_iat, in)));
      }
    } catch (const std::exception &e) {
      std::cerr << "[-][iat] failed with error : " << e.what() << std::endl;
    }
  }

  return 0;
}