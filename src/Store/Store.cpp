#include "Store.h"

namespace ToolFramework {

Store::Store(){}

void Store::Initialise(std::stringstream inputstream){
	std::string line;
	while(getline(inputstream, line)){
		if(line.empty()) continue;
		if(line[0]=='#') continue;
		std::string key;
		std::string value;
		std::stringstream stream(line);
		if(stream>>key>>value) m_variables[key]=value;
	}
}

bool Store::Initialise(std::string filename){
  
  std::ifstream file(filename.c_str());
  std::string line;
  
  if(file.is_open()){
    
    while (getline(file,line)){
      if (line.size()>0){
	if (line.at(0)=='#')continue;
	std::string key="";
	std::string value="";
	std::stringstream stream(line);
	stream>>key>>value;
	std::string tmp;
	stream>>tmp;
	value='"'+value;
	  
	while(tmp.length() && tmp[0]!='#'){
	  value+=" "+tmp;
	  tmp="";
	  stream>>tmp;
	}
	value+="\"";

	if(value!="") m_variables[key]=value;
      }
      
    }
    file.close();
  }
  else{
    std::cout<<"\033[38;5;196m WARNING!!!: Config file "<<filename<<" does not exist no config loaded \033[0m"<<std::endl;
    return false;
  }
  
  return true;
}

void Store::Print(){
  
  for (std::map<std::string,std::string>::iterator it=m_variables.begin(); it!=m_variables.end(); ++it){
    
    std::cout<< it->first << " => " << it->second <<std::endl;
    
  }
  
}


void Store::Delete(){

  m_variables.clear();


}


void Store::JsonParser(std::string input){ 

  int type=0;
  std::string key="";
  std::string value="";
  bool term = false;

  for(std::string::size_type i = 0; i < input.size(); ++i) {
    
     if(type==2){
         // scanning a key
         if(value.size()==0){
            // not yet found start of key, might be string or otherwise
            if(input[i]==':' || input[i]==' '){
                continue; // still not found the start, keep looking
            } else if(input[i]=='\"'){
                // string value, set our scan to stop at a terminating '"'
                term=true;
            } else {
                // not a string, set our scan to stop at a terminating ','
                value+=input[i];  // this isn't a terminator, so add to value
            }
         } else {
             // we're adding chars. check for terminator
             if( (term && input[i]=='\"') || (!term && input[i]==',') || (!term && input[i]=='}') ){
                 // terminator found, add to internal map and reset
                 type=0;
                 size_t sz=value.size();
                 while(value[sz-1]==' ') --sz; // trim
                 value.resize(sz);
                 m_variables[key] = value;
                 key="";
                 value="";
                 term=false;
             } else {
                // just a char to add to value
                value+=input[i];
             }
         }
     }
     else if(input[i]=='\"') type++;
     else if(type==1)key+=input[i];
     else if(type==3)value+=input[i];
     else if(type==4){
       type=0;
       m_variables[key]=value;
       key="";
       value="";
       term=false;
     }
     
      
      /*
    if(input[i]!=',' &&  input[i]!='{' && input[i]!='}' && input[i]!='\"' && input[i]!=':' && input[i]!=',')pair<<input[i];
    else if(input[i]==':')pair<<" ";
    else if(input[i]==',') {
      std::cout<<" i = "<<i<<" pair = "<<pair<<std::endl;
    
      pair>>key>>value;
      */  
      //pair.clear();

      //}
  }

}
  


bool Store::Has(std::string key){

  return (m_variables.count(key)!=0);

}


std::vector<std::string> Store::Keys(){

  std::vector<std::string> ret;

  for(std::map<std::string, std::string>::iterator it= m_variables.begin(); it!= m_variables.end(); it++){

    ret.push_back(it->first);

  }

  return ret;

}


bool Store::Get(std::string name, std::string &out){
  if(m_variables.count(name)>0){ 
    out=StringStrip(m_variables[name]);
    return true;
  }
  return false;
}

bool Store::Get(std::string name, bool &out){
  if(m_variables.count(name)>0){
    std::string tmp=StringStrip(m_variables[name]);
    if(tmp=="true") out=true;
    else if(tmp=="false") out=false;
    else if(tmp=="" || tmp=="0") out=false;
    else out=true;
    return true;
    
  }
  return false;

}

bool Store::Get(std::string name, Store &out){
  if(m_variables.count(name)>0 && StringStrip(m_variables[name])[0]=='{'){
    out.JsonParser(StringStrip(m_variables[name]));
      return true;
 }
 return false;
  
}

void Store::Set(std::string name, std::string in){
  std::stringstream stream;
  stream<<"\""<<in<<"\"";
  m_variables[name]=stream.str();
}

void Store::Set(std::string name, const char* in){
  std::stringstream stream;
  stream<<"\""<<in<<"\"";
  m_variables[name]=stream.str();
}

void Store::Set(std::string name,std::vector<std::string> in){
  std::stringstream stream;
  std::string tmp="[";
  for(unsigned int i=0; i<in.size(); i++){
    stream<<"\""<<in.at(i)<<"\"";
    tmp+=stream.str();
    if(i!=in.size()-1)tmp+=',';
    stream.str("");
    stream.clear();
  }
  tmp+=']';
  m_variables[name]=tmp;
  
}

std::string Store::StringStrip(std::string in){

  if(in.length() && in[0]=='"' && in[in.length()-1]=='"') return in.substr(1,in.length()-2);
  return in;

}

bool Store::Destring(std::string key){

  if(!m_variables.count(key)) return false;
  m_variables[key]=StringStrip(m_variables[key]);
  return true;

}

std::ostream& operator<<(std::ostream& stream, const Store& s){
  stream<<"{";
  bool first=true;
  for(auto it=s.m_variables.begin(); it!=s.m_variables.end(); ++it){
      if (!first) stream<<", ";
      stream<<"\""<<it->first<<"\":"<< it->second;
      first=false;
  }
  stream<<"}";
  return stream;
}
  
}
