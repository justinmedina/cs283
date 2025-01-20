#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SZ 50

// prototypes
void usage(char *);
void print_buff(char *, int);
int setup_buff(char *, char *, int);

// prototypes for functions to handle required functionality
int count_words(char *, int, int);
char *reverse_string(char *, int);
void print_words(char *, int);
// add additional prototypes here
int is_whitespace(char c);

int setup_buff(char *buff, char *user_str, int len)
{
  // TODO: #4:  Implement the setup buff as per the directions
  char *buffer_ptr = buff;    // Pointer to traverse and write to the buffer
  char *input_ptr = user_str; // Pointer to traverse the input string
  int count = 0;              // Keeps track of characters added to the buffer
  int str_len = 0;            // Stores the resulting string length

  // Loop through the input string and populate the buffer
  while (*input_ptr != '\0' && count < len)
  {
    if (!is_whitespace(*input_ptr)) // Copy non-whitespace characters
    {
      *buffer_ptr++ = *input_ptr;
      count++;
    }
    else if (count > 0 && *(buffer_ptr - 1) != ' ') // Add a single space if necessary
    {
      *buffer_ptr++ = ' ';
      count++;
    }
    input_ptr++;
  }

  // If the input string is longer than the allowed buffer size, return an error
  if (*input_ptr != '\0')
  {
    return -1;
  }

  str_len = buffer_ptr - buff; // Calculate the length of the resulting string

  // Fill the rest of the buffer with dots to reach the maximum buffer size
  while (count < len)
  {
    *buffer_ptr++ = '.';
    count++;
  }

  return str_len; // Return the length of the valid part of the buffer
}

int is_whitespace(char c)
{
  // Returns 1 (true) if the character is whitespace, otherwise 0 (false)
  return (c == ' ' || c == '\t' || c == '\n');
}

void print_buff(char *buff, int len)
{
  printf("Buffer:  ");
  // Print each character in the buffer
  for (int i = 0; i < len; i++)
  {
    putchar(*(buff + i));
  }
  putchar('\n');
}

void usage(char *exename)
{
  // Print the correct usage of the program
  printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);
}

int count_words(char *buff, int len, int str_len)
{
  int word_counter = 0; // Counter for the number of words
  int in_word = 0;      // Tracks if we are inside a word

  // Iterate through the buffer up to the provided string length
  for (int i = 0; i < str_len && i < len; i++)
  {
    if (!is_whitespace(buff[i]) && !in_word) // Detect start of a new word
    {
      word_counter++;
      in_word = 1;
    }
    else if (is_whitespace(buff[i])) // Detect end of a word
    {
      in_word = 0;
    }
  }

  return word_counter; // Return the total word count
}

char *reverse_string(char *buff, int len)
{
  int i = 0; // Index for traversing the buffer
  // Allocate memory for the reversed string
  char *reversed_string = (char *)malloc(len + 1);

  // Reverse the characters in the buffer
  while (i < len && buff[i] != '\0')
  {
    reversed_string[i] = buff[len - 1 - i];
    i++;
  }
  reversed_string[i] = '\0'; // Null-terminate the reversed string
  return reversed_string;
}

void print_words(char *buff, int str_len)
{
  printf("Word Print\n");
  printf("----------\n");
  int word_count = 1; // Counter for word numbering
  char *start = buff; // Pointer to the start of a word
  char *end = buff;   // Pointer to the end of a word

  // Iterate through the buffer to find words
  while (start < buff + str_len)
  {
    // Skip leading whitespace
    while (start < buff + str_len && is_whitespace(*start))
    {
      start++;
    }
    end = start;

    // Find the end of the current word
    while (end < buff + str_len && !is_whitespace(*end))
    {
      end++;
    }

    // Print the word if valid
    if (start < end)
    {
      printf("%d. ", word_count++);
      for (char *ptr = start; ptr < end; ptr++)
      {
        putchar(*ptr);
      }
      printf(" (%ld)\n", end - start); // Print word length
    }
    start = end; // Move to the next word
  }
}

// ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

int main(int argc, char *argv[])
{

  char *buff;         // Placeholder for the internal buffer
  char *input_string; // Holds the string provided by the user on cmd line
  char opt;           // Used to capture user option from cmd line
  int rc;             // Used for return codes
  int user_str_len;   // Length of user-supplied string

  // TODO:  #1. WHY IS THIS SAFE, aka what if argv[1] does not exist?
  /*
  // It is safe because we check if argc < 2. If argv[1] does not exist, the program exits.
  */
  if ((argc < 2) || (*argv[1] != '-'))
  {
    usage(argv[0]);
    exit(1);
  }

  opt = (char)*(argv[1] + 1); // Get the option flag

  // Handle the help flag and then exit normally
  if (opt == 'h')
  {
    usage(argv[0]);
    exit(0);
  }

  // WE NOW WILL HANDLE THE REQUIRED OPERATIONS

  // TODO:  #2 Document the purpose of the if statement below
  /*
  // The if statement checks if the user supplied enough arguments (argc < 3).
  */

  if (argc < 3)
  {
    usage(argv[0]);
    exit(1);
  }

  input_string = argv[2]; // Capture the user input string

  // TODO:  #3 Allocate space for the buffer using malloc and
  //           handle error if malloc fails by exiting with a
  //           return code of 99
  //  CODE GOES HERE FOR #3
  buff = (char *)malloc(BUFFER_SZ);
  if (buff == NULL)
  {
    printf("Error allocating buffer");
    exit(99);
  }

  user_str_len = setup_buff(buff, input_string, BUFFER_SZ); // See todos
  if (user_str_len < 0)
  {
    printf("Error setting up buffer, error = %d", user_str_len);
    exit(2);
  }

  switch (opt)
  {
  case 'c':
    rc = count_words(buff, BUFFER_SZ, user_str_len); // Count the words
    if (rc < 0)
    {
      printf("Error counting words, rc = %d", rc);
      exit(2);
    }
    printf("Word Count: %d\n", rc);
    break;

  case 'r':
    // Reverse the buffer content
    char *reversed_string = reverse_string(buff, user_str_len);
    printf("Reversed String: %s\n", reversed_string);
    free(reversed_string); // Free the memory for the reversed string
    break;

  case 'w':
    print_words(buff, user_str_len); // Print individual words
    break;

  default:
    usage(argv[0]); // Print usage for invalid option
    free(buff);
    exit(1);
  }

  // TODO:  #6 Don't forget to free your buffer before exiting
  print_buff(buff, BUFFER_SZ);
  free(buff);
  exit(0);
}

// TODO:  #7 Notice all of the helper functions provided in the
//           starter take both the buffer as well as the length.  Why
//           do you think providing both the pointer and the length
//           is a good practice, after all we know from main() that
//           the buff variable will have exactly 50 bytes?
//
//           PLACE YOUR ANSWER HERE
/*
// Providing both the pointer and length prevents buffer overflows.
// The length ensures we do not process beyond the allocated memory.

*/

