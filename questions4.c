
#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>   

#define HASHSIZE 101     // We'll make the hash table size 101 
#define MAXNUM  5        // We can have up to 5 strings in our array 
#define MAXWORD 100      // And each string can be up to 100 characters long

//  a hash function that takes a string to a int value
int hash (char *str)  
{
    int hash = 0;
    int c;  
    while (c = *str++)     // Go through each character in the string
        hash = c + (hash << 6) + (hash << 16) - hash;   // And calculate its hash value
    return hash % HASHSIZE;   // Modulo it with the hash table size 
}

//  get all possible substrings of a given length from a string
char **get_substrings(char *str, int length, int *count) {  
    int i, j;
    int n = strlen(str);   // Get the length of the string 

    char substr[length + 1];  // Make a char array to hold each substring
    char **substrings = NULL; // And a pointer to an array of strings

    // First count how many substrings we'll have
    *count = n - length + 1; 

    // Allocate memory for the array
    substrings = (char **)malloc(*count * sizeof(char *));  
    for (i = 0; i < *count; i++) {  
        substrings[i] = (char *)malloc((length + 1) * sizeof(char));  
    }

    // Now get all the substrings of length 'length' 
    for (i = 0; i <= n - length; i++) {  
        for (j = 0; j < length; j++) {  
            substr[j] = str[i+j];   // Copy the characters from i to i + length - 1
        }
        substr[length] = '\0';     // Add the null character  
        strcpy(substrings[i], substr);  // And copy the substring into the array
    }

    return substrings; // Return the array of substrings
} 

int main()  
{
    // I'll start with an array of strings 
    char str[MAXNUM][MAXWORD]={"abc","def","ghi","jkl","abd"};

    // The substring I want to search for 
    char *parttern = "ab";      

    // Calculate the hash value for that substring
    int hash_p = hash(parttern);

    // search the strings for that substring using hashing
    int len = strlen(parttern); // Get the length of the substring 
    int count = 0 ;            // And a count variable

    // Go through each string
    for (int i = 0; i < MAXNUM; i++)  
    {
        // Get all substrings of length 'len' from the current string
        char **substrings = get_substrings(str[i], len, &count); 

        // Check if any of those substrings have the same hash as our pattern
        for(int j = 0; j < count; j++)
        {  
            if(hash(substrings[j]) == hash_p)  // If we found a match!
            {
                // Print the string 
                printf("%s\n", str[i]);
                break;  // And move on to the next string
            }
        }
    }
}
