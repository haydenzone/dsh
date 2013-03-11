/**
 * @file helper.h
 */
#ifndef __HELPER_H_
#define __HELPER_H_

#include<string>
#include<sstream>
#include<vector>
using namespace std;
bool isinteger( string number );
int stringToInt( string number);
void trimLeadingCharacter( string & str, char chr );
vector<string> split( string str, char divide );
void filterBlanks( vector<string> &strs);
void parse_redirect( vector<string> & command_array, bool & rert_stdin, bool & rert_stdout, string & stdout_file, string & stdin_file);
void parse_remote_redirect( vector<string> & command_array, bool & rert_stdin, bool & rert_stdout, string & stdin_ip,string & stdin_port, string & stdout_port);

#endif
