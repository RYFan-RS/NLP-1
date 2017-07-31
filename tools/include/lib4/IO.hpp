/*
  ATLAN C++ Library: IO.hpp

  ATLAN C++ Library is a library for personal studying.
  No licence is annouced.
  You can use or change any part of it.
  When you using it, inform the author is hoped but not required.
  
  Author: atlantise522@gmail.com
*/
#ifndef __ACL__IO_HPP__
#define __ACL__IO_HPP__
#include <iterator>
#include <stdlib.h>
#include <string.h>
#include "String.hpp"
#include "Vector.hpp"

namespace acl{

class IO{
public:
	static void overwrite(acl::String output, std::ostream& ofs = std::cerr){
		static int lastLength = 0;
		
		//reset the lastLength
		if(output.empty()){
			lastLength = 0;
			return;
		}
		
		//clear last output
		for(int pos=0; pos<lastLength; pos++){
			ofs << "\b";
		}
		
		//write new output
		ofs << output;
		
		//update the lastLength
		std::vector<std::string> v = output.toCharactor();
		lastLength = 0;
		for(size_t i=0; i<v.size(); i++){
			if(v[i].length() > 1){
				lastLength += 2;
			}else{
				lastLength++;
			}
		}
	}

	static std::string STDIN(const Vector<String>& messages){
		std::string input;
		char charIn[2560];

		while(1){
			std::copy(messages.begin(),--messages.end(),std::ostream_iterator<String>(std::cout,"\n"));
			std::cout<<messages[messages.size()-1];

			if(fgets(charIn, 2560, stdin) == NULL) continue;
			
			for(size_t i=0; i<strlen(charIn); i++){
				if(charIn[i] != 0x0a && charIn[i] != 0x0d){
					input += charIn[i];
				}
			}
			if(input.empty()){
				continue;
			}else if(input == "q"){
				exit(0);
			}else{
				break;
			}
		}

		return input;
	}
	
	static std::string STDIN(const String& message = "INPUT>"){
		Vector<String> messages;
		
		messages.push_back(message);

		return STDIN(messages);
	}
};

}//namespace acl

#endif //__ACL__IO_HPP__
