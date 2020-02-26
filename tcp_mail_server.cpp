#include <iostream>
#include <fstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <cstring>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;


const int SIZE=1024;


int strToInt(string s2){

	string s=s2;

	for (int i=0;i<s.length();i++){
		if (s[i]>57 || s[i]<48)
			return -1;
	}


	int j=s.length()-1;
	int num=0;
	int pow=1;

	while(j>=0){
		num+=(s[j]-48)*pow;
		pow*=10;
		j--;

	}

	return num;

}

void reverse(string &s){

	string s2="";
	for (int i=s.length()-1;i>=0;i--){

		s2+=s[i];

	}

	s=s2;

}

string intToStr(int x2){

	string s="";
	if (x2==0){
		s+='0';
		return s;
	}

	int x=x2;

	char ch;
	while (x){

		ch=x%10+48;
		s+=ch;
		x/=10;

	}

	reverse(s);

	return s;

}

int numOfSpaces(string s){

	int count=0;
	for (int i=0;i<s.length();i++){

		if (s[i]==' '){
			count++;
		}

	}

	return count;

}

void delay(){
	for (int i=0;i<20000000;i++);
}

void tokenize(string s,string arr[]){

	int spaces = numOfSpaces(s);
	int j=0;

	for (int i=0;i<spaces+1;i++){

		while (s[j]!=' ' && s[j]!='\0' && s[j]!='\n'){

			arr[i]+=s[j];
			j++;

		}

		j++;
	}
}

bool isValidEmail(string s){


	if (s[0]>=48 && s[0]<=57)
		return false;

	if (numOfSpaces(s))
		return false;

	int indexOfRate=0;
	while (s[indexOfRate] != '@'){

		indexOfRate++;

		if (indexOfRate==s.length())
			break;

	}

	if (!(indexOfRate==s.length()-10 || indexOfRate==s.length()-12) )
		return false;
		
	string domain="";
	
	for (int i=indexOfRate+1;i<s.length();i++){
		domain+=s[i];
		
	}
	
	if (domain != "yahoo.com" && domain!="gmail.com" && domain!="hotmail.com")
		return false;
	
	return true;
}

struct message{

	string subject;
	string body;
	string senderName;
	string recipientName;
	int id;

	message(){

        subject="";
        body="";
        senderName="";
        recipientName="";
        id=0;

	}

};

struct myClient{

	string password;
	string email;
	/*message *inbox;
	message *sent;
	int inboxCount;
	int sentCount;*/

	//struct sockaddr_in clientAddr;

};





class mainServer{

	private:

		myClient *clients;
		int numOfClients;
		int skfd;
		int newskfd;
		struct sockaddr_in serverAddr;
		struct sockaddr_in clientAddr;
		socklen_t addrSize;


	public :
		mainServer(){   // the server is started  and this sockets etc are called via constructoras soon as the server is started 

			clients=NULL;
			numOfClients=0;
			skfd= socket(AF_INET, SOCK_STREAM,0);

			serverAddr.sin_family=AF_INET;
			serverAddr.sin_port=htons(7891);
			serverAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
			memset(serverAddr.sin_zero,'\0',sizeof(serverAddr.sin_zero));
			bind(skfd,(struct sockaddr *)&serverAddr,sizeof(serverAddr));
			listen(skfd,5);
			addrSize=sizeof(clientAddr);


		}


		void wait4Client(){

			newskfd=accept(skfd,(struct sockaddr *)& clientAddr,&addrSize);

			//pid_t number=fork();

		}


		void sendToClient(string s){

			char *buffer=new char[s.length()];

			for (int i=0;i<s.length();i++){


				buffer[i]=s[i];

			}


			send(newskfd,buffer,s.length(),0);




		}

		bool isValidRcpt(string email){

			for (int i=0;i<numOfClients;i++){
				if (email == clients[i].email)
					return true;
			}

			return false;

		}

		string receiveFromClient(){

			char buffer[1024];
			string s;

			memset(&buffer, '\0', SIZE);
			recv(newskfd,buffer,SIZE,0);

			int i=0;
			while (buffer[i]!='\0'){

				s+=buffer[i];
				i++;

			}

			return s;

		}

		void closeConnection(){
			close(newskfd);


		}


		void addClient(string email,string password){

			ofstream outfile("clients.txt",ios::app);
			outfile <<email<<" "<<password<<endl;
			outfile.close();


			myClient *temp=new myClient[numOfClients];

			for (int i=0;i<numOfClients;i++){

				temp[i]=clients[i];

			}

			if (numOfClients)
				delete []clients;

			clients=new myClient[numOfClients+1];
			clients[numOfClients].email=email;
			clients[numOfClients].password=password;
			for (int i=0;i<numOfClients;i++){

				clients[i]=temp[i];

			}


			numOfClients++;

		}

		void makeClientFiles(string email){

			string s="mkdir -p "+email;
			system(s.c_str());

			string file=email+"/"+"inbox.txt";
			ofstream outfile(file.c_str());
			outfile.close();

			file=email+"/"+"sent.txt";
			ofstream outfile2(file.c_str());
			outfile2.close();


		}



		void insertClient(string email,string password){

			myClient *temp=new myClient[numOfClients];

			for (int i=0;i<numOfClients;i++){

				temp[i]=clients[i];

			}

			if (numOfClients)
				delete []clients;

			clients=new myClient[numOfClients+1];
			clients[numOfClients].email=email;
			clients[numOfClients].password=password;
			for (int i=0;i<numOfClients;i++){

				clients[i]=temp[i];

			}


			numOfClients++;


		}


		void loadClientsData(){

			ifstream fin("clients.txt");

			string line="";

			getline(fin,line);

			while (!fin.eof()){


				int spaces=numOfSpaces(line);

				string arr[spaces+1];

				tokenize(line,arr);


				insertClient(arr[0],arr[1]);

				getline(fin,line);

			}
		}


		bool authenticate(string email,string pswd){


			for (int i=0;i<numOfClients;i++){

				if (email == clients[i].email && pswd == clients[i].password){

					return true;

				}

			}

			return false;

		}

		bool isDuplicateEmail(string e){

			for (int i=0;i<numOfClients;i++){
				if (clients[i].email == e){
					return true;
				}
			}

			return false;

		}

		void insertToFiles(message m){

			string file=m.recipientName+"/"+"inbox.txt";
			ofstream outfile(file.c_str(),ios::app);

			outfile<<"From : "<<m.senderName<<endl;
			outfile<<"Subject : "<<m.subject<<endl;
			outfile<<"Message : "<<m.body;
			outfile<<'.'<<endl;

			outfile.close();


			file=m.senderName+"/"+"sent.txt";
			ofstream outfile2(file.c_str(),ios::app);

			outfile2<<"To : "<<m.recipientName<<endl;
			outfile2<<"Subject : "<<m.subject<<endl;
			outfile2<<"Message : "<<m.body;
			outfile2<<'.'<<endl;

			outfile2.close();


		}


};

void handleClient(mainServer s){

		
		string clientChoiceStr=s.receiveFromClient();

		int clientChoice = strToInt(clientChoiceStr);

		if (clientChoice == 1){

			string email,pswd;

			email=s.receiveFromClient();
			pswd=s.receiveFromClient();

			if (s.authenticate(email,pswd)){

				string str="250";
				s.sendToClient(str);

				//now receive menu choice from user
				int clientMenuChoice = strToInt(s.receiveFromClient());

				if (clientMenuChoice == 1){

					message msg;

					msg.senderName=email;
					msg.recipientName=s.receiveFromClient();


					if (s.isValidRcpt(msg.recipientName)){

						s.sendToClient("250");
						msg.subject=s.receiveFromClient();

						//delay();

						string line=s.receiveFromClient();
						while (line!="."){

							msg.body+=line;
							msg.body+='\n';

							line = s.receiveFromClient();

						}

						s.sendToClient("Message accepted for delievery !!!");

						s.insertToFiles(msg);


					}
					else{

						s.sendToClient("Recipient Mail Not Valid!!!!");

					}
				}

				else if (clientMenuChoice == 2){     //show clients inbox

				    message *inbox;
				    int msgCount=0;

				    string file=email+"/"+"inbox.txt";
				    ifstream ifile;
				    ifile.open(file.c_str());
				    message m;
				    string line;

				   while(!ifile.eof()){

				        getline(ifile,line);
				        if (line[0]=='.')
				            msgCount+=1;

				    }
				    s.sendToClient(intToStr(msgCount));
				    ifile.close();

				     delay();

				    ifstream ifile2(file.c_str());
				    for (int i=0;i<msgCount;i++){

				            m.senderName="";
				            m.subject="";

				            m.id=i;

				            getline(ifile2,m.senderName);
				            getline(ifile2,m.subject);

				            getline(ifile2,line);

				            m.body="";

				            while (line[0]!='.'){

				                m.body+=line;
				                m.body+='\n';
				                getline(ifile2,line);

				            }

				            message *temp=new message[i];

				            for (int j=0;j<i;j++){

				                temp[j]=inbox[j];

				            }

				            if (i){
				                delete []inbox;
				            }

				            inbox=new message[i+1];
				            inbox[i]=m;

				            for (int j=0;j<i;j++){

				                inbox[j]=temp[j];

				            }

				            s.sendToClient(intToStr(inbox[i].id)+") "+inbox[i].senderName+" | "+inbox[i].subject);
				            //delay();
				    }

				    ifile2.close();

				    int msgNumber=strToInt(s.receiveFromClient());

				    s.sendToClient(inbox[msgNumber].senderName+"\n"+inbox[msgNumber].subject+"\n"+inbox[msgNumber].body);

				}

				else if (clientMenuChoice == 3){     //show clients sent box

				    message *inbox;
				    int msgCount=0;

				    //s.sendToClient(email);
				    string file=email+"/"+"sent.txt";
				    ifstream ifile;
				    ifile.open(file.c_str());
				    message m;
				    string line;

				    while(!ifile.eof()){

				        getline(ifile,line);
				        if (line[0]=='.')
				            msgCount+=1;

				    }
				    s.sendToClient(intToStr(msgCount));
				    ifile.close();

				     delay();

				    ifstream ifile2(file.c_str());
				    for (int i=0;i<msgCount;i++){

				            m.recipientName="";
				            m.subject="";

				            m.id=i;

				            getline(ifile2,m.recipientName);
				            getline(ifile2,m.subject);

				            getline(ifile2,line);

				            m.body="";

				            while (line[0]!='.'){

				                m.body+=line;
				                m.body+='\n';
				                getline(ifile2,line);

				            }

				            message *temp=new message[i];

				            for (int j=0;j<i;j++){

				                temp[j]=inbox[j];

				            }

				            if (i){
				                delete []inbox;
				            }

				            inbox=new message[i+1];
				            inbox[i]=m;

				            for (int j=0;j<i;j++){

				                inbox[j]=temp[j];

				            }

				            s.sendToClient(intToStr(inbox[i].id)+") "+inbox[i].recipientName+" | "+inbox[i].subject);
				            //delay();
				    }

				    ifile2.close();

				    int msgNumber=strToInt(s.receiveFromClient());

				    s.sendToClient(inbox[msgNumber].recipientName+"\n"+inbox[msgNumber].subject+"\n"
				    		+inbox[msgNumber].body);

				}


			}
			else{
				s.sendToClient("Log In Failed !!!");
			}


		}

		else if (clientChoice == 2){

			string email,pswd,pswd2;

			email = s.receiveFromClient();
			pswd = s.receiveFromClient();
			pswd2=s.receiveFromClient();

			if (isValidEmail(email) && pswd==pswd2 && numOfSpaces(pswd)==0  && !s.isDuplicateEmail(email) ){

				s.addClient(email,pswd);
				s.makeClientFiles(email);

				s.sendToClient("Sign Up Successful !!!");

			}
			else{

				s.sendToClient("Sign Up Fail !!!");

			}

		}
}

int main() {


	mainServer s;

	s.loadClientsData();  // first 

	while(1){

		s.wait4Client();
		pid_t number = fork();  // making child processes for talking with multiple clients

		if (number==0)
			handleClient(s);
		else
			s.closeConnection();

	}

	return 0;

}
