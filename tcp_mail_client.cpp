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

string reverse(string s){

	string s2="";
	for (int i=s.length()-1;i>=0;i--){

		s2+=s[i];

	}

	return s2;

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

	return reverse(s);

}




class client{
	private:

		//string password,email;
		int skfd;
		struct sockaddr_in serverAddr;
		socklen_t addrSize;


	public :

		client(){

		}



		void connectToServer(){  // this code runs only once 


			skfd= socket(AF_INET, SOCK_STREAM,0);
			serverAddr.sin_family=AF_INET;
			serverAddr.sin_port=htons(7891);
			serverAddr.sin_addr.s_addr=inet_addr("127.0.0.1");

			memset(serverAddr.sin_zero,'\0',sizeof(serverAddr.sin_zero));
			addrSize=sizeof(serverAddr);
			connect(skfd,(struct sockaddr *)&serverAddr, addrSize);


		}


		void sendToServer(string s){   //sends the string as buffer to the server


			char *buffer=new char [s.length()];

			for (int i=0;i<s.length();i++){

				buffer[i]=s[i];

			}

			send(skfd , buffer , strlen(buffer) , 0);



		}

		string receiveFromServer(){ //recieves the reply from server mostly the 200 ok tc


			char buffer[SIZE];
			memset(&buffer, '\0', SIZE);
			recv(skfd,buffer,SIZE,0);

			string s;
			int i=0;
			while (buffer[i]!='\0'){
				s+=buffer[i];
				i++;

			}

			return s;

		}

};


void showHomePage(){ //the first menu shown to the client

		cout<<"Hello Dear Client !!!\n1.Log In.\n2.Sign Up.\n3.Forgot Password\nYour Choice = ";

}

int userChoice(){

	int x;
	cin>>x;

	return x;

}



void showUserMenu(){  //this is shown if users gets log in

	cout<<"1.Send Mail.\n2.Show Inbox\n3.Show SentBox\n4.log Out\nYour Choice = ";

}



void delay(){
	for (int i=0;i<200000000;i++);
}



int main() {


		client c;

		c.connectToServer(); // to connect to the server

	

	
		showHomePage(); // home page is shown at the start only once
		int choice = userChoice();  //take input from user 

		string choiceStr=intToStr(choice);  //convert the choice number into string

		c.sendToServer(choiceStr); //sending choice to server,so that server handle client according to his choice

		if (choice==1){ // choice one is for login

			string email,pswd;

			cout<<"Enter your email = ";
			cin>>email;
			c.sendToServer(email);

			cout<<"Enter your password = ";
			cin>>pswd;
			c.sendToServer(pswd); //send the password  and mail to server to get the 200 or bad response


			string logInResponse = c.receiveFromServer(); //server sends the response on the basis of mail and password

			if (strToInt(logInResponse) == 250){  // if the login is successful
			
				
				showUserMenu();
				int menuChoice = userChoice();

				c.sendToServer(intToStr(menuChoice));  //telling server what client choosed

				if (menuChoice == 1){                    //if client choosed to send mail

					string rcpt,subject,body;

					cout<<"Enter Recipient mail = ";
					cin>>rcpt;

					c.sendToServer(rcpt);
					string rcptMailResponse = c.receiveFromServer();

					if (strToInt(rcptMailResponse) == 250){  // if the recipient exists


						cout<<"Enter Subject of email (max 60 characters) = ";
						getline(cin>>ws,subject);


						c.sendToServer(subject);  // the subject of mail is appended in file at server

						string line;
						cout<<"Enter mail ending with single '.' on last line = ";

						while (line != "."){   // the body of mail

							getline(cin>>ws,line);
							c.sendToServer(line);

						}

						cout<<c.receiveFromServer()<<endl;   //once the body has been recieved by server it sends a reply to client

					}

					else{   // if the  reciepient doesnt exist

						cout<<rcptMailResponse<<endl;

					}

				}

				else if (menuChoice==2){  //show inbox is choice by logged in client


					    string sss=c.receiveFromServer();
					    int msgCount=strToInt(sss); // tells how many inbox msgs do you have

					    string line;

					    for(int i=0;i<msgCount;i++){

						line=c.receiveFromServer();
						cout<<line<<endl;

					    }

					    int msgNumber=0;
					    cout<<"Enter message number to see full message = ";msgNumber=userChoice();

					    c.sendToServer(intToStr(msgNumber));      


					    line=c.receiveFromServer();  //shows the whole msg
					    cout<<line;


				}

				else if (menuChoice==3){ // if the choice is to view sent box

					    string sss=c.receiveFromServer();
					    int msgCount=strToInt(sss);

					    string line;

					    for(int i=0;i<msgCount;i++){

						line=c.receiveFromServer();
						cout<<line<<endl;

					    }

					    int msgNumber=0;
					    cout<<"Enter message number to see full message = ";msgNumber=userChoice();

					    c.sendToServer(intToStr(msgNumber));


					    line=c.receiveFromServer();
					    cout<<line;

				}
				else{
					cout<<"Wrong Input !!!\n";
				}

			}
			else{

				cout<<logInResponse<<endl;

			}
		}

		else if (choice == 2){  //sign up

			string email,pswd,pswd2;

			cout<<"Enter your email = ";
			cin>>email;

			c.sendToServer(email);

			cout<<"Enter your password = ";
			cin>>pswd;

			c.sendToServer(pswd);

			cout<<"Enter your password again = ";
			cin>>pswd2;

			c.sendToServer(pswd2);

			cout<<c.receiveFromServer()<<endl;

		}
		else{
		
			cout<<"Wrong Input !!!\n";
		
		}
		
	


	return 0;

}
