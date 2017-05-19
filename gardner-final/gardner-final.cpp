// Program Name:	Contact List (using a Bianry Tree)
// Chapter:			Chapter 5  - Binary Trees
// Programmer:		Michael A Gardner
// Date:			24 July 2016
// Description:		This Program inserts the data from "contacts.in" to a binarytree. After that It will display a menu asking the user
//					to manipulate the contacts binary tree.  To save the binary tree enter y after exiting.

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>

#pragma warning(disable: 4996); // This disables the 4996 Error Message!!

using namespace std;
#define MaxWordSize 20 // The max size of a contact input.

typedef struct {					// The Data Payload for each node!
	char lName[MaxWordSize + 1];	// Last Name is stored here
	char fName[MaxWordSize + 1];	// First Name is stored here
	char phone[MaxWordSize + 1];	// Phone Number is stored here
}Payload;

typedef struct node {
	Payload pl;								// pl is the where the data is stored
	struct node *left, *right, *parent;		// The Roots Branches
} Node, *NodePtr;

typedef struct {
	NodePtr root;		// This is the starting point for the tree! :)
} control;


//  **  Function Prototypes  **
char menu();											// Displays a menu to the console
NodePtr deleteContact(NodePtr cp, Payload d);			// Used to find then delete a contact specified by payload d.
control getFile();										// Gets contacts.in and places it in the tree
NodePtr newNode(Payload d);								// Creates a new node. xD
Payload getData(string tempLine);						// Gets each lines data in the contacts.in file and places the data in the correct pl char.
NodePtr findOrInsert(NodePtr curr, Payload d);			// This will Insert a new node into the binary tree in the correct order!
void printContacts(NodePtr np);							// This will print the contacts in alphabetical order to the console.
NodePtr FindMin(NodePtr root);							// THis is used to find the minimum left value after a node that is to be deleted
int combine(char currlname[], char currfname[], char targetlname[], char targetfname[]);  // Returns (-1 is less, 0 is equal, 1 is greater)
void search(NodePtr curr, Payload d);					// This Searches for a Contact.
Payload getFullName(bool needNumber);					// Ask the user for first and last name if bool is true it will also ask for phone.
void printToFile(NodePtr curr, FILE * out);				// Will Print contacts to the file specified with the data specified.
bool searchEqual(NodePtr curr, Payload d);



int main()
{
	//  ** Varables and Structs ** 
	char choice, save;
	control cp;
	Payload pl, findThis, delCon;

	// ** Create Binary Tree from contacts.in
	cp = getFile();

	// Start Menu
	do {
		choice = menu();

		switch (choice)
		{
		case '1':	// Add Contact
			pl = getFullName(true);
			if (searchEqual(cp.root, pl)) {
				cout << pl.fName << " " << pl.lName << "  " << pl.phone << " is already in the contacts!\n\n";
			}
			else findOrInsert(cp.root, pl);
			break;

		case '2':	// Search Contacts
			findThis = getFullName(false);
			search(cp.root, findThis);
			break;

		case '3':	//Print Contacts
			cout
				<< "\n\t     ** Contacts ** \n\n"
				<< "  Last \t\t First          Number\n"
				<< " ------\t\t-------        --------\n";
			printContacts(cp.root);
			cout << endl << endl;
			break;

		case '4':	// Delete a Contact
			delCon = getFullName(false);
			deleteContact(cp.root, delCon);
			break;

		case '5':  // Create or Update Contacts.txt
			FILE * out = fopen("Contacts.txt", "w");   //create the output file
			printToFile(cp.root, out);
			fclose(out);
			cout << "Contacts.txt updated!!\n\n";
			break;
		}

	} while (choice != '0');

	// Ask User if he/she would like to save the file.
	cout << "Would you like to save your changes? (y or n): ";
	cin >> save;										// Get Answer

	if (save == 'y') {
		FILE * saveCon = fopen("contacts.in", "w");		// Create the output file.
		printToFile(cp.root, saveCon);					// Update the files data.
		fclose(saveCon);								// Close the file.
		cout << "**  Save Sucessful  **\n\n";			// Tell the user the save was succesfull.
	}
	cout << "System Exit\n\n";							// Let the user know the program is exiting.
	cout << endl;
	return 0;
}

char menu()
{
	char choice;		// Choice the user makes

						//  **  Display Choices  **
	cout
		<< "Menu:\n"
		<< "1. Add New Contact\n"
		<< "2. Search Contacts\n"
		<< "3. Print Contact List\n"
		<< "4. Delete Contact\n"
		<< "5. Update Contacts.txt\n"
		<< "0. Exit\n\n"
		<< "  Make a selection: ";
	cin >> choice;		// Get Choice from user
	cout << endl;		// Newline

	return choice;		// Return users choice to the calling function.
} // End menu()


control getFile()
{

	control cp;			// Make a new Control Pointer;
	cp.root = NULL;		// Set the new Control Pointer to NULL. It is empty.

	ifstream contacts;						// Create a new stream.
	contacts.open("contacts.in");			// Open file contacts.in stored in contacts.
	string tempLine;						// Tempary line for each line in the file.
	while (getline(contacts, tempLine)) {	// Get new line in contacts file and store it into tempLine string.
		if (cp.root == NULL) {				// If the Tree is Empty Then...
			cp.root = newNode(getData(tempLine));	// Root equals the new node created(Create a newnode and store the data in the new node)
		}
		else {								// If the tree isn't empty.....
			NodePtr node = findOrInsert(cp.root, getData(tempLine));  // Find where the node should be placed in the tree and put it there.
		}
	}
	return cp; // Return the Tree Root.
}

NodePtr newNode(Payload d)
{
	NodePtr p = (NodePtr)malloc(sizeof(Node));  //allocate enough memory for new node
	p->pl = d;				// Update the Payload !!! xD
	p->left = NULL;			// Left Pointer Equals NULL!
	p->right = NULL;		// Right Pointer Equals NULL!
	return p;				// Return the Newly created Node!!!!
}

Payload getData(string tempLine)	// Update the payload for each Newline in a file.
{
	Payload pl;								// Create a payload Structure to put the data in.
	stringstream inputLine(tempLine);		// Create a stringstream from the return value of inputLine.
	inputLine >> pl.lName >> pl.fName >> pl.phone;  // Store each word in the correct payload char
	return pl;								// Return the newly created payload!! xD
}

NodePtr findOrInsert(NodePtr curr, Payload d) // This function will find the correct place to store a node!!
{
	int last;			// Used to store value from compare function

	if (curr == NULL)
	{
		curr = newNode(d);  // Make a new node here
	}
	//  **  combine (combines the last name and first name of both the current node and the payload being inserted) Then it Compares them
	// and returns -1 if it is less, 0 if it is equal, and 1 if it is more.
	// ##  To store the contacts in reverse alphabetical order change 1 to -1  ##
	else if ((last = combine(curr->pl.lName, curr->pl.fName, d.lName, d.fName)) == 1) {
		curr->left = findOrInsert(curr->left, d);		//  Go Left
	}
	else {
		curr->right = findOrInsert(curr->right, d);		// Go Right
	}

	return curr;
}



void printContacts(NodePtr np)   // Print the contacts
{
	if (np == NULL) return;
	printContacts(np->left);	// Go Left
	cout << np->pl.lName << "\t\t" << np->pl.fName << "\t\t" << np->pl.phone << endl; // Print Current Contact
	printContacts(np->right);	// Go Right
}


NodePtr deleteContact(NodePtr curr, Payload data)
{
	int last;

	// ** Search for the node to delete  **
	if (curr == NULL) return curr;
	else if ((last = combine(curr->pl.lName, curr->pl.fName, data.lName, data.fName)) == -1) curr->left = deleteContact(curr->left, data);  // data < curr->data
	else if ((last = combine(curr->pl.lName, curr->pl.fName, data.lName, data.fName)) == 1) curr->right = deleteContact(curr->right, data);
	// **  Node Found begin del branch 	** 
	else {

		//  No child
		if (curr->left == NULL && curr->right == NULL) { // If Both branches are NULL then...
			free(curr);		// Delete curr node
			curr = NULL;	// reset curr to NULL
		}
		// One child 
		else if (curr->left == NULL) {		// If left is NULL then...
			NodePtr temp = (NodePtr)malloc(sizeof(Node));		// Create a tempary node
			temp = curr;										// Set temp to the current
			curr = curr->right;									// Set current equal to right
			free(temp);											// Delete temp node
		}
		else if (curr->right == NULL) {		// If right is NULL then...
			NodePtr temp = (NodePtr)malloc(sizeof(Node));		// Create a tempary node
			temp = curr;										// Set temp to the current node.
			curr = curr->left;									// Set current equal to the left
			free(temp);											// Delete temp node
		}
		// 2 children
		else {		// Both left and right has data that must be reserved
			NodePtr temp = (NodePtr)malloc(sizeof(Node));		// Create a tempary node
			temp = FindMin(curr->right);						// Find the Min value in the right branch set temp to equal that!! :)
			curr->pl = temp->pl;								// Store the payload in temp node
			curr->right = deleteContact(curr->right, temp->pl);	// Now we can delete the side with only one brance!!! 
		}
	}

	return curr;
}

NodePtr FindMin(NodePtr root)	// This finds the min value on the branch
{
	while (root->left != NULL) root = root->left;
	return root;		// Return the ptr to the lowest value on the branch!
}

int combine(char currlname[], char currfname[], char targetlname[], char targetfname[])
{
	int cmp;
	char currFullName[(MaxWordSize + 1) * 2];
	char targetFullName[(MaxWordSize + 1) * 2];
	strcpy(currFullName, currlname);			// Copy the data from currlname to currFullName
	strcat(currFullName, currfname);			// Concatinate the data from currfname to currFullName
	strcpy(targetFullName, targetlname);		// Copy the data from targetlname to targetFullName
	strcat(targetFullName, targetfname);		// Concatinate the data from targetfname to targetFullName
	cmp = strcmp(currFullName, targetFullName);	// Compare the strings and set cmp to equil (-1 is less, 0 is equal, 1 is greater)
	return cmp;  // return the comparison! xD
}

void search(NodePtr curr, Payload d) // Search for a contact!
{
	int last;
	if (curr == NULL) return;
	search(curr->left, d); // Got left
	if ((last = combine(curr->pl.lName, curr->pl.fName, d.lName, d.fName)) == 0)  // If Payloads are equil print found: pl
	{
		cout << "\nfound: " << curr->pl.fName << " " << curr->pl.lName << "   " << curr->pl.phone << endl << endl;
	}
	search(curr->right, d); // Go Right
}

Payload getFullName(bool needNumber) // 
{
	Payload pl;
	cout << "Enter First Name: "; cin >> pl.fName; // Ask user for first name
	cout << "Enter Last Name: "; cin >> pl.lName;	// Ask user for last name
	if (needNumber) { cout << "Enter Phone Number: "; cin >> pl.phone; }	// If neednumber is true Ask user for number.
	cout << endl;		// New line
	return pl;
}

void printToFile(NodePtr curr, FILE * out) // Print the current tree to the file specified!!!
{

	if (curr == NULL) return;			// If the tree is empty do nothing!
	printToFile(curr->left, out);		// Go left
										// ** Print the payload of the current node **
	fprintf(out, curr->pl.lName);
	fprintf(out, " ");
	fprintf(out, curr->pl.fName);
	fprintf(out, " ");
	fprintf(out, curr->pl.phone);
	fprintf(out, "\n");
	// Goto the next Node
	printToFile(curr->right, out);

}
bool searchEqual(NodePtr curr, Payload d)
{
	int last,
		cmp;
	bool found = false;
	if (curr == NULL) return found;
	searchEqual(curr->left, d); // Got left
	if (((last = combine(curr->pl.lName, curr->pl.fName, d.lName, d.fName)) == 0) && (cmp = strcmp(curr->pl.phone, d.phone)) == 0)  // If Payloads are equil print found: pl
	{
		found = true;
		return found;
	}
	searchEqual(curr->right, d); // Go Right
}
// END OF PROGRAM