#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
using namespace std;

typedef string (*handle)(smatch);
typedef map<string, handle> RuleMap;

static void usage(char **argv) {
  cout << "[Usage]: " << argv[0] << " inputfile [outputfile]" << endl;
  cout << "\tinputfile - markdown filename to parse\n";
  cout << "\touputfile - optional output html filename\n";
}

static string surroundWithTag(string text, string tag, string attributes = "") {
  return "<" + tag + attributes + ">" + text + "</" + tag + ">";
}

static string replaceMatch(smatch match, string text, string htmlText) {
  int start = text.find(match[0]);
  int end = match[0].length();
  string newText = text;
  newText.replace(start, end, htmlText);
  return newText;
}

static string handleHeader(smatch match) {
  string size = "h" + to_string(match[1].length());
  string header = match[2];

  return surroundWithTag(header, size);
}

static string handleUnorderedList(smatch match) {
  string listItem = match[1];
  return "\n" + surroundWithTag(surroundWithTag(listItem, "li"), "ul");
}

static string handleStrong(smatch match) {
  string bold = match[1];
  return surroundWithTag(bold, "strong");
}

static string handleLink(smatch match) {
  string linkText(match[1]);
  string url(match[2]);
  string attr = " href=\"" + url + "\"";
  return surroundWithTag(linkText, "a", attr);
}

static string handleInlineCode(smatch match) {
  return surroundWithTag(match[1], "code");
}

static string handleStrikethrough(smatch match) {
  return surroundWithTag(match[1], "strike");
}

static string cleanText(string text) {
  map<string, string> replacements;
  replacements.emplace("<", "&lt;");
  replacements.emplace(">", "&gt;");
  string newText = text;
  for (map<string, string>::iterator i = replacements.begin(),
                                     e = replacements.end();
       i != e; ++i) {
    size_t start;
    int length = i->first.length();
    while ((start = newText.find(i->first)) != string::npos) {
      newText.replace(start, length, i->second);
    }
  }

  return newText;
}

static string parseMarkdown(string text) {
  smatch match;
  RuleMap rules;
  rules.emplace("(#+) (.*)", &handleHeader);
  rules.emplace("\\[([^\\]]+)\\]\\(([^\\)]+)\\)", &handleLink);
  rules.emplace("\\n\\* (.*)", &handleUnorderedList);
  rules.emplace("\\*\\*(.*)\\*\\*", &handleStrong);
  rules.emplace("`(.*)`", &handleInlineCode);
  rules.emplace("~{2}(.*)~{2}", &handleStrikethrough);

  string updatedText = cleanText(text);
  for (RuleMap::iterator i = rules.begin(), e = rules.end(); i != e; ++i) {
    text = updatedText;
    while (regex_search(text, match, regex(i->first))) {
      string html = i->second(match);
      updatedText = replaceMatch(match, updatedText, html);
      text = match.suffix().str();
    }
  }

  while (updatedText.find("</ul>\n<ul>") != string::npos) {
    string unwanted = "</ul>\n<ul>";
    int start = updatedText.find(unwanted);
    int end = unwanted.length();

    updatedText.replace(start, end, "\n");
  }

  return updatedText;
}

int main(int argc, char **argv) {

  ifstream mdDoc;
  ofstream output;
  stringstream buffer;
  string plaintext;
  string htmlText;
  if (argc < 2) {
    usage(argv);
    return 1;
  } else if (argc == 3) {
    output.open(argv[2]);
  }

  mdDoc.open(argv[1]);
  if (!mdDoc.is_open())
    return 1;

  buffer << mdDoc.rdbuf();

  plaintext = buffer.str();
  htmlText = parseMarkdown(plaintext);

  if (output.is_open()) {
    output << htmlText << endl;
  }

  cout << htmlText << endl;

  return 0;
}
