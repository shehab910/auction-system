#include <iostream>
#include <string>
#include <conio.h>
#include <Windows.h>
#include <map>
#include <vector>
#include <ctime>
#define YEAR (1900+newtime.tm_year)
#define MONTH (1 + newtime.tm_mon)
#define DAY (newtime.tm_mday)
using namespace std;
const int numOfMembers = 100;
const int numOfItems = 500;
time_t now = time(0);
struct tm newtime;

struct Date
{
	int day = 0, month = 0, year = 2020;
};
struct Memeber
{
	bool isExist = false;
	int ID = 0; // member id is the member index + 1000
	string email;
	string name;
	string password;
	string deliveryAddress;
	string phoneNum;
	int points = 0;
}mem[numOfMembers];

struct Item
{
	int indexOfHighestBidder = -1;
	int ownerIndex = -1;
	int previousOwnerIndex = -1;
	int num = 0; // item's number is its index + 1000
	int highestBid = 0; //initialized to starting bid price
	string category;
	string name;
	Date dateOfAuction;
	Date dateOfAuctionEnd;
	bool status = false;
	bool newItemRecieved = false;
	bool isExist = false;
}items[numOfItems];

void Register();
int Login();
int FindEmptyIndex(void);
bool CheckRating(int currMem);
void SellNew(int currMem);
void All();
void Bid(int);
string Password();
void MyForSaleList(int);
void Search(char searchChoice);
void Transfare(int i);
void ShowItemInfo(int i);
void Update(int i);
bool VerifyPhoneNo(string phone);
bool VerifyEmail(string email);
void ShowMonthlyReport(void);

int main()
{
	string choice;
	int currMem = -1;
	
	while (true)
	{
		//time
		time_t now = time(0);
		localtime_s(&newtime, &now);
		cout << "\n\t\t\tMain Menu\n==========================================================\n";
		cout << "Current member: ";
		if (currMem < 0)
		{
			cout << "none.. please login\n";
		}
		else
		{
			cout << mem[currMem].name << endl;
			cout << "You can Buy or Sell now !\n";
		}
		cout << "Type:\n";
		if (currMem < 0)
		{
			cout << "register OR R | to Register a new member\n"
				<< "login OR L | to login to an existing member account\n";
			cin >> choice;
			if (choice == "register" || choice == "R")
			{
				Register();
			}
			else if (choice == "login" || choice == "L")
			{
				bool validRating = false;
				currMem = Login();
				while (!validRating)
				{
					validRating = CheckRating(currMem);

				}
			}
		}
		else
		{
			cout << "logout OR E | to logout\n"
				<< "new OR N | to post a new item for bidding\n"
				<< "all OR A | to show all items currently avaliable for bidding\n"
				<< "filter OR F | to search items by category or price or both\n"
				<< "bid OR B | to bid on an item (make sure you remember the item's number)\n"
				<< "MFSL OR mfsl | to show the items you own currently for sale (My For Sale List)\n"
				<< "report OR R | to show monthly report of sold items\n";
			cin >> choice;
			if (choice == "new" || choice == "N")
			{
				SellNew(currMem);
			}
			else if (choice == "all" || choice == "A")
			{
				cout << "Please remember the item's number that you want to bid on it\n";
				cout << "\n\t\t\tItem list\n==========================================================\n";
				Sleep(1000);
				All();
			}
			else if (choice == "filter" || choice == "F")
			{
				char filterChoice;
				cout << "type\n"
					<< "C | to search by category\n"
					<< "P | write the max price you are willing to pay (all highest bids will be less than that number\n"
					<< "B | to search by both\n";
				cin >> filterChoice;
				cout << "\n\t\t\tItem list\n==========================================================\n";
				Search(filterChoice);
			}
			else if (choice == "MFSL" || choice == "mfsl")
			{
				cout << "\n\t\t\tItem list\n==========================================================\n";
				MyForSaleList(currMem);
			}
			else if (choice == "bid" || choice == "B")
			{
				Bid(currMem);
			}
			else if (choice == "report" || choice == "R")
			{
				ShowMonthlyReport();
			}
			else if (choice == "logout" || choice == "E")
			{
				currMem = -1;
			}
		}
	}
	return 0;
}
string Password()
{
	string pass, checkPass;
	pass.erase();
	checkPass.erase();
	char ch, check_ch;
	bool isSpecial = true, isNum = true;
	cout << "Enter your password \n(Your password must contain at least 1 special character and 1 number)\n"
		<< "Password will not be shown for security\n";
	bool isMatch = true;
	do
	{
		if (!isMatch)
		{
			cout << "\nTwo passwords are different or no password was typed\n"
				<< "try again..\n";
		}
		do
		{
			pass.erase();
			checkPass.erase();
			if (!(isSpecial && isNum))
			{
				cout << endl << "Password is not valid" << endl;
				cout << "Enter your password \n(Your password must contain at least 1 special character and 1 number)\n"
					<< "Password will not be shown for security\n";
			}
			isSpecial = false, isNum = false;
			while (true)
			{
				ch = _getch();
				if (ch == '\r')
				{
					break;
				}
				pass += ch;
				if (ch == '$' || ch == '%' || ch == '_' || ch == '#' || ch == '@' || ch == '!' || ch == '&' || ch == '*')
				{
					isSpecial = true;
				}
				if (ch >= 48 && ch <= 57)
				{
					isNum = true;
				}
			}
		} while (!(isSpecial && isNum));
		cout << "Enter the same password again\n";
		while (true)
		{
			check_ch = _getch();
			if (check_ch == '\r')
			{
				break;
			}
			checkPass += check_ch;
		}
		// check if the passwords dont match or left blank
		if (pass != checkPass || pass == "\0")
		{
			isMatch = false;
		}
		else
		{
			isMatch = true;
		}
	} while (!isMatch);
	return pass;
}
void Transfare(int i)
{
	//transfare item from owner to highest bidder
	if (items[i].indexOfHighestBidder > -1)
	{
		items[i].previousOwnerIndex = items[i].ownerIndex;
		items[i].ownerIndex = items[i].indexOfHighestBidder;
		items[i].indexOfHighestBidder = -1;
		items[i].newItemRecieved = true;
	}
}
bool CheckRating(int currMem)
{
	bool isShown = false;
	for (int i = 0; i < numOfItems; i++)
	{
		if (items[i].ownerIndex == currMem && items[i].newItemRecieved == true)
		{
			if (!isShown)
			{
				cout << "Please rate the seller of the below item(s)\n";
				Sleep(2000);
				isShown = true;
			}
			ShowItemInfo(i);
			cout << "Enter your rating (from 1 to 5)\n";
			char rating;
			cin >> rating;
			if (rating == '3' || rating == '4' || rating == '5')
			{
				mem[items[i].previousOwnerIndex].points++; 
			}
			else if (!(rating == '1' || rating == '2'))
			{
				cout << "Invalid input try again\n";
				return false;
			}
			items[i].newItemRecieved = false;
		}
	}
	return true;
}
void Update(int i)
{
	//checks if the current item (i) auction is outdated 
	//and transfare items if the auction ended
	if (items[i].dateOfAuctionEnd.month <= MONTH - 1)
	{
		items[i].isExist = false;
	}
	if (items[i].dateOfAuctionEnd.year < YEAR)
	{
		items[i].status = false;
		Transfare(i);
	}
	else if (items[i].dateOfAuctionEnd.month < MONTH)
	{
		items[i].status = false;
		Transfare(i);
	}
	else if (items[i].dateOfAuctionEnd.day < DAY)
	{
		items[i].status = false;
		Transfare(i);
	}
}
void ShowItemInfo(int i)
{
	//prints the info of the item
	cout << "Item's name: " << items[i].name << "\tItem's Number: " << items[i].num << "\tItem's category: " << items[i].category << endl;
	cout << "Highest bid: " << items[i].highestBid << "\tEnd date: "
		<< items[i].dateOfAuctionEnd.day << '/' << items[i].dateOfAuctionEnd.month << '/' << items[i].dateOfAuctionEnd.year << endl;
	cout << "Seller's points: " << mem[items[i].ownerIndex].points << endl;

}
void Search(char filterChoice)
{
		string categorySearch;
		int priceSearch;
		switch (filterChoice)
		{
			case 'C':
				cout << "Enter the catergory:";
				cin >> categorySearch;
				break;
			case 'P':
				cout << "Enter the max price: ";
				cin >> priceSearch;
				break;
			case 'B':
				cout << "Enter the catergory:";
				cin >> categorySearch;
				cout << "Enter the max price: ";
				cin >> priceSearch;
				break;
			default:
			{
				cout << "Invalid input.. try again\n";
				Sleep(1000);
				return;
			}
		}
		for (int i = 0, itemsSize = numOfItems; i < itemsSize; i++)
		{
			Update(i);
			if (items[i].status)
			{
				if (filterChoice == 'C')
				{
					if (items[i].category == categorySearch)
					{
						ShowItemInfo(i);
					}
				}
				else if (filterChoice == 'P')
				{
					if (items[i].highestBid < priceSearch)
					{
						ShowItemInfo(i);
					}
				}
				else if (filterChoice == 'B')
				{
					if (items[i].highestBid < priceSearch && items[i].category == categorySearch)
					{
						ShowItemInfo(i);
					}
				}
			}
		}
		return;
}
bool VerifyPhoneNo(string phone)
{
	if (phone[0] == '0' && phone[1] == '1' && phone.size() == 11)
	{
		return true;
	}
	return false;
}
bool VerifyEmail(string email)
{
	bool foundAt = false, foundDot = false, foundNoSpecialChar = false;
	for (int i = 0, z = email.size(); i < z; i++)
	{
		if (email[i] == '.')
		{
			foundDot = true;
		}
		if (email[i] == '@')
		{
			foundAt = true;
		}
		if ((email[i] >= 'A' && email[i] <= 'Z') || (email[i] >= 'a' && email[i] <= 'z') || (email[i] >= '0' && email[i] <= '9') || email[i] == '.' || email[i] == '_' || email[i] == '-')
		{
			foundNoSpecialChar = true;
		}
	}
	if (foundAt && foundDot && foundNoSpecialChar)
	{
		return true;
	}
	return false;
}
void Register()
{
	int currMem = 0;
	string currInput,confPass;
	//searching for an empty index in the memebers array
	for (int i = 0; i < numOfMembers; i++)
	{
		if (!mem[i].isExist)
		{
			currMem = i;
			break;
		}
	}
	cout << "\t\t\tRegister\n========================================================\n";
	// prompting the user for info
	cout << "Enter your name: ";
	cin.ignore();
	getline(cin, currInput);
	mem[currMem].name = currInput;
	cout << "Enter your email: ";
	getline(cin, currInput);
	if (!VerifyEmail(currInput))
	{
		cout << "Wrong email format, try again\n";
		return;
	}
	mem[currMem].email = currInput;
	cout << "Enter your phone number: ";
	getline(cin, currInput);
	if (!VerifyPhoneNo(currInput))
	{
		cout << "Wrong phone number format, try again\n";
		return;
	}
	mem[currMem].phoneNum = currInput;
	cout << "Enter your delievery address: ";
	getline(cin, currInput);
	mem[currMem].deliveryAddress = currInput;
	mem[currMem].password = Password();
	mem[currMem].ID = currMem + 1000;
	//making sure the input is true
	cout << "Please check the data you enterd:\n"
		<< "Your Name: "
		<< mem[currMem].name
		<< endl
		<< "Your email: "
		<< mem[currMem].email
		<< endl
		<< "Your Phone number: "
		<< mem[currMem].phoneNum
		<< endl
		<< "Your address: "
		<< mem[currMem].deliveryAddress
		<< endl
		<< "Your ID: "
		<< mem[currMem].ID
		<< endl;
	cout << "To confirm and save your info type 'y', to delete type 'd'\n";
	char save;
	cin >> save;
	if (save == 'y' || save == 'Y')
	{
		mem[currMem].isExist = true;
		cout << "Info saved.. you can login now\n";
		Sleep(1500);
	}
	else
	{
		cout << "Info not saved.. try again\n";
		Sleep(1500);
	}
	return;
}
int Login()
{
	string email, pass;
	char check_ch;
	bool isvalid = true;
	int userIndex;
	cout << "Please enter your email and password\nEmail: ";
	cin >> email;
	cout << "Password: ";
	while (true)
	{
		check_ch = _getch();
		if (check_ch == '\r')
		{
			break;
		}
		pass += check_ch;
	}
	for (int i = 0; i < numOfMembers; i++)
	{
		if (mem[i].email == email)
		{
			userIndex = i;
			break;
		}
	}
	if (pass == mem[userIndex].password)
	{
		return userIndex;
	}
	cout << "email or password not found.. please register or try again\n";
	Sleep(1500);
	return -1;
}
int FindEmptyIndex(void)
{
	for (int i = 0; i < numOfItems; i++)
	{
		if (items[i].isExist == false)
		{
			return i;
		}
	}
	return -1;
}
void SellNew(int currMem)
{
	int currItemIndex = FindEmptyIndex();
	if (currItemIndex == -1)
	{
		cout << "memory is full..\n please wait till some of the auctions end and try again\n";
		Sleep(2000);
		return;
	}
	char check;
	int item_num = (currItemIndex + 1000);
	Item temp_item;
	temp_item.status = true;
	temp_item.isExist = true;
	temp_item.num = item_num;
	temp_item.ownerIndex = currMem;
	cout << "Enter item's name: ";
	cin >> temp_item.name;
	cout << "Enter item's category: ";
	cin >> temp_item.category;
	cout << "Enter item's start bidding price: ";
	cin >> temp_item.highestBid;
	cout << "Enter item's start bidding date (DD MM YY): ";
	cin >> temp_item.dateOfAuction.day >> temp_item.dateOfAuction.month >> temp_item.dateOfAuction.year;
	cout << "Enter item's end bidding date (DD MM YY): ";
	cin >> temp_item.dateOfAuctionEnd.day >> temp_item.dateOfAuctionEnd.month >> temp_item.dateOfAuctionEnd.year;
	cout << "Check your data and make sure it is correct.. Type 'y' to save or 'n' to ignore\n";
	cin >> check;
	if (check == 'y' || check == 'Y')
	{
		items[currItemIndex] = temp_item;
		cout << "Info saved your item will be for sale on the start date\n";
		Sleep(1500);
		return;
	}
	else
	{
		cout << "Item not saved.. try again\n";
		Sleep(1500);
		return;
	}
}
void All()
{
	int listSize = numOfItems;
	for (int i = 0; i < listSize; i++)
	{
		//updating the out of date bids
		Update(i);
		//checking if the item is available
		if (items[i].status)
		{
			ShowItemInfo(i);
		}
	}
}
void Bid(int currMem)
{
	int index, bid = 0;
	char check = 0;
	cout << "Please type the item's number: ";
	cin >> index;
	index -= 1000;
	Update(index);
	if (!items[index].status)
	{
		cout << "This item is not available try again\n";
		Sleep(1500);
		return;
	}
	ShowItemInfo(index);
	cout << "If you wish to continue enter your bid, else enter 0\n"
		<< "You should bid higher than the highest bid\n";
	cin >> bid;
	if (bid == 0)
	{
		return;
	}
	else if (bid < 0 || bid <= items[index].highestBid)
	{
		cout << "Invalid input.. try again\n";
		Sleep(1000);
		return;
	}
	items[index].indexOfHighestBidder = currMem;
	items[index].highestBid = bid;
	cout << "Success.. Make sure to check periodicaly to see if someone outbidded you!\n"
		<< "If the seller agreed to your price or the auction end and your are the highest bidder\n"
		<<"the item will be added to your list\n";
	Sleep(3000);
	return;
}
void MyForSaleList(int currMem)
{
	for (int i = 0, itemsSize = numOfItems; i < itemsSize; i++)
	{
		Update(i);
		if (items[i].status && items[i].ownerIndex == currMem)
		{
			ShowItemInfo(i);
		}
	}
}
void ShowMonthlyReport(void)
{
	cout << "\n\t\t\tSold In This Month\n==========================================================\n";
	for (int i = 0; i < numOfItems; i++)
	{
		if (items[i].isExist == true && items[i].status == false)
		{
			ShowItemInfo(i);
			cout << "New Owner: " << mem[items[i].ownerIndex].name << endl;
			cout << "Old Owner: " << mem[items[i].previousOwnerIndex].name << endl;
		}
	}
}
//resources
//https://www.tutorialspoint.com/cplusplus/cpp_date_time.htm
//http://www.cplusplus.com/reference/string/string/
//https://stackoverflow.com/questions/35258285/how-to-use-localtime-s-with-a-pointer-in-c
//https://bit.ly/2WuUxG9
//https://stackoverflow.com/questions/12691316/getline-does-not-work-if-used-after-some-inputs