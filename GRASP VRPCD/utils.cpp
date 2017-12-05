#include "utils.h"

vector<string> split(string line, char delimiter) {
  vector<string> new_vector;
  stringstream new_string(line); 
  string token;
  
  while(getline(new_string, token, delimiter)) {
    new_vector.push_back(token);
  }
  
  return new_vector;
}

vector<int> get_int_vector(string line) {

	line = trim(line);
	vector<int> new_vector;
	stringstream new_string(line); 
	string token;

	while(getline(new_string, token, ' ')) {
		if(token.compare(" ") == -1 ){
			new_vector.push_back(-1);
		}
		else{
			new_vector.push_back(atoi(token.c_str()));
		}
	}

	return new_vector;
}

string trim_left(const string& str)
{
  const string pattern = " \f\n\r\t\v";
  return str.substr(str.find_first_not_of(pattern));
}

string trim_right(const string& str)
{
  const string pattern = " \f\n\r\t\v";
  return str.substr(0,str.find_last_not_of(pattern) + 1);
}

string trim(const string& str)
{
  return trim_left(trim_right(str));
}