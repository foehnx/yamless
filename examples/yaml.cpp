#include "yamless/yaml.hpp"

#include <catch2/catch.hpp>
#include <exception>
#include <iostream>

using namespace yamless;

TEST_CASE("Read a YAML stream", "[yaml]") {
  const std::string s{
    "int: 2   # a comment\n"
    "node:    # another comment \n"
    "  int: 1\n"
    "  float: 1.0\n"
    "  # an indented comment\n"
    "  double: 2.0\n"
    "  bool: true\n"
    "  string1: \"string1\"\n"
    "  string2: \'string2\'\n"
    "  string3: string3\n"
    "  string4: \'string4\"\n"
    "  string5: \"string5\'\n"
    "# completely stupid comment followed by empty line\n"
    "\n"
    "  list: [0, 1, 2]\n"
    "  multi_list: [ [1 ,2], [1.0, -2.0, 3.0], [[\"a\", \"b\"], [\'cd\', "
    "\'efg\"]]]\n"
    "  other_list: [{braces}, (parentheses), [mixed}]\n"
    "  complex: (1.0, 2.0)\n"
    "  nested:\n"
    "    nested_value: 3.0\n"
    "    nested_list:\n"
    "      - bla: 1\n"
    "        blu: 3\n"
    "      - bla: 2\n"
    "        blu: 4\n"};

  Yaml yaml(s);
  CHECK(!yaml["doesnotexist"].isValid());
  CHECK(!yaml["doesnotexist"].isDefined());
  CHECK(yaml["doesnotexist"].size() == 0);
  CHECK(yaml["int"].as<int>() == 2);
  CHECK(yaml["int"].size() == 1ul);
  CHECK(yaml["node"].size() == 14);
  CHECK(yaml["node"]["int"].as<int>() == 1);
  CHECK(yaml["node"]["float"].as<float>() == 1.0f);
  CHECK(yaml["node"]["double"].as<double>() == 2.0);
  CHECK(yaml["node"]["bool"].as<bool>() == true);
  CHECK(yaml["node"]["string1"].as<std::string>() == "string1");
  CHECK(yaml["node"]["string2"].as<std::string>() == "string2");
  CHECK(yaml["node"]["string3"].as<std::string>() == "string3");
  CHECK(yaml["node"]["string4"].as<std::string>() == "string4");
  CHECK(yaml["node"]["string5"].as<std::string>() == "string5");
  CHECK(yaml["node"]["list"].size() == 3);
  CHECK(yaml["node"]["list"][0].as<int>() == 0);
  CHECK(yaml["node"]["list"][1].as<int>() == 1);
  CHECK(yaml["node"]["list"][2].as<int>() == 2);
  CHECK(yaml["node"]["multi_list"].size() == 3);
  CHECK(yaml["node"]["multi_list"][0].size() == 2);
  CHECK(yaml["node"]["multi_list"][0][0].as<int>() == 1);
  CHECK(yaml["node"]["multi_list"][0][1].as<int>() == 2);
  CHECK(yaml["node"]["multi_list"][1].size() == 3);
  CHECK(yaml["node"]["multi_list"][1][0].as<int>() == 1);
  CHECK(yaml["node"]["multi_list"][1][1].as<int>() == -2);
  CHECK(yaml["node"]["multi_list"][1][2].as<int>() == 3);
  CHECK(yaml["node"]["multi_list"][2].size() == 2);
  CHECK(yaml["node"]["multi_list"][2][0].size() == 2);
  CHECK(yaml["node"]["multi_list"][2][0][0].as<std::string>() == "a");
  CHECK(yaml["node"]["multi_list"][2][0][1].as<std::string>() == "b");
  CHECK(yaml["node"]["multi_list"][2][1].size() == 2);
  CHECK(yaml["node"]["multi_list"][2][1][0].as<std::string>() == "cd");
  CHECK(yaml["node"]["multi_list"][2][1][1].as<std::string>() == "efg");
  CHECK(yaml["node"]["other_list"].size() == 3);
  CHECK(yaml["node"]["other_list"][0].size() == 1);
  CHECK(yaml["node"]["other_list"][1].size() == 1);
  CHECK(yaml["node"]["other_list"][2].size() == 1);
  CHECK(yaml["node"]["other_list"][0].as<std::string>() == "braces");
  CHECK(yaml["node"]["other_list"][1].as<std::string>() == "parentheses");
  CHECK(yaml["node"]["other_list"][2].as<std::string>() == "mixed");
  CHECK(yaml["node"]["complex"].size() == 2);
  CHECK(yaml["node"]["complex"].as<std::complex<double>>() ==
        std::complex<double>(1.0, 2.0));
  CHECK(yaml["node"]["nested"]["nested_value"].as<double>() == 3.0);
  CHECK(yaml["node"]["nested"]["nested_list"].size() == 2);
  CHECK(yaml["node"]["nested"]["nested_list"][0]["bla"].as<int>() == 1);
  CHECK(yaml["node"]["nested"]["nested_list"][0]["blu"].as<int>() == 3);
  CHECK(yaml["node"]["nested"]["nested_list"][1]["bla"].as<int>() == 2);
  CHECK(yaml["node"]["nested"]["nested_list"][1]["blu"].as<int>() == 4);
}


TEST_CASE("Ill formed YAML parsing", "[yaml]") {
  const std::string ill_formed1{
    "fine_int: 5\n"
    "good_string: \"soo good\"\n"
    " this is not so good .... ::: {}\n"
    "this_could_be_fine_again: 42"};
  CHECK_THROWS([&] {
    try {
      Yaml yaml(ill_formed1);
    } catch (std::exception& e) {
      std::cout << e.what();
      throw e;
    }
  }());

  const std::string ill_formed2{
    "fine_int: 5\n"
    "good_string: \"soo good\"\n"
    "too_many_opening_brackets_fail: {{1.0, 2.0}\n"
    "this_could_be_fine_again: 42"};
  CHECK_THROWS([&] {
    try {
      Yaml yaml(ill_formed2);
    } catch (std::exception& e) {
      std::cout << e.what();
      throw e;
    }
  }());

  const std::string ill_formed3{
    "fine_int: 5\n"
    "good_string: \"soo good\"\n"
    "too_many_closing_brackets_fail: {1.0, 2.0}}\n"
    "this_could_be_fine_again: 42"};
  CHECK_THROWS([&] {
    try {
      Yaml yaml(ill_formed3);
    } catch (std::exception& e) {
      std::cout << e.what();
      throw e;
    }
  }());
}

TEST_CASE("Bool parsing", "[yaml]") {
  const std::string bools{
    "true_1: 1\n"
    "false_0: 0\n"
    "true_t: t\n"
    "false_f: f\n"
    "true_T: T\n"
    "false_F: F\n"
    "true: true\n"
    "false: false\n"
    "true_case: TruE\n"
    "false_case: fALse\n"
    "true_quote: \"true\"\n"
    "false_quote: \"false\"\n"
    "true_sentence: \"if it has true and false, the first is returned\"\n"};
  Yaml yaml{bools};
  std::cout << yaml;
  CHECK(yaml["true_1"].as<bool>() == true);
  CHECK(yaml["false_0"].as<bool>() == false);
  CHECK(yaml["true_t"].as<bool>() == true);
  CHECK(yaml["false_f"].as<bool>() == false);
  CHECK(yaml["true_T"].as<bool>() == true);
  CHECK(yaml["false_F"].as<bool>() == false);
  CHECK(yaml["true"].as<bool>() == true);
  CHECK(yaml["false"].as<bool>() == false);
  CHECK(yaml["true_case"].as<bool>() == true);
  CHECK(yaml["false_case"].as<bool>() == false);
  CHECK(yaml["true_quote"].as<bool>() == true);
  CHECK(yaml["false_quote"].as<bool>() == false);
  CHECK(yaml["true_sentence"].as<bool>() == true);
}