#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "config.h"
#include "helpers.h"

/*------------------------------------------------------------------------
 * Program:   http server
 *
 * Purpose:   allocate a socket and then repeatedly execute the following:
 *              (1) wait for the next connection from a client
 *              (2) read http request, reply to http request
 *              (3) close the connection
 *              (4) go back to step (1)
 *
 * Syntax:    http_server [ port ]
 *
 *               port  - protocol port number to use
 *
 * Note:      The port argument is optional.  If no port is specified,
 *            the server uses the port specified in config.h
 *
 *------------------------------------------------------------------------
 */

int main(int argc, char *argv[]) {
  /* structure to hold server's and client addresses, respectively */
  struct sockaddr_in server_address, client_address;

  int listen_socket = -1;
  int connection_socket = -1;
  int port = 0;

  /* id of child process to handle request */
  pid_t pid = 0;

  char response_buffer[MAX_HTTP_RESPONSE_SIZE] = "";
  int status_code = -1;
  char *status_phrase = "";

  /* 1) Create a socket */
  /* START CODE SNIPPET 1 */

  listen_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_socket < 0) {
    fprintf(stderr, "ERROR opening socket\n");
    exit(EXIT_FAILURE);
  }

  int yes = 1;
  if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==
      -1) {
    perror("setsockopt");
    exit(1);
  }

  /* END CODE SNIPPET 1 */

  /* Check command-line argument for port and extract
   * port number if one is specified. Otherwise, use default
   */
  if (argc > 1) {
    /* Convert from string to integer */
    port = atoi(argv[1]);
  } else {
    port = DEFAULT_PORT;
  }

  if (port <= 0) {
    /* Test for legal value */
    fprintf(stderr, "bad port number %d\n", port);
    exit(EXIT_FAILURE);
  }

  /* Clear the server address */
  memset(&server_address, 0, sizeof(server_address));

  /* 2) Set the values for the server address structure */
  /* START CODE SNIPPET 2 */

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(port);
  server_address.sin_addr.s_addr = INADDR_ANY;
  memset(&(server_address.sin_zero), '\0', 8);

  /* END CODE SNIPPET 2 */

  /* 3) Bind the socket to the address information set in server_address */
  /* START CODE SNIPPET 3 */

  if (bind(listen_socket, (struct sockaddr *)&server_address,
           sizeof(server_address)) == -1) {
    perror("bind");
    exit(1);
  }

  /* END CODE SNIPPET 3 */

  /* 4) Start listening for connections */
  /* START CODE SNIPPET 4 */

  if (listen(listen_socket, 10) == -1) {
    perror("listen");
    exit(1);
  }

  /* END CODE SNIPPET 4 */

  /* Main server loop
   * Loop while the listen_socket is valid
   */
  while (listen_socket >= 0) {
    /* 5) Accept a connection */
    /* START CODE SNIPPET 5 */

    socklen_t sin_size = sizeof(client_address);
    connection_socket =
        accept(listen_socket, (struct sockaddr *)&client_address, &sin_size);
    if (connection_socket == -1) {
      perror("accept");
      continue;
    }

    /* END CODE SNIPPET 5 */

    /* Fork a child process to handle this request */
    if ((pid = fork()) == 0) {
      /*----------START OF CHILD CODE----------------*/
      /* We are now in the child process */

      /* Close the listening socket
       * The child process does not need access to listen_socket
       */
      if (close(listen_socket) < 0) {
        fprintf(stderr, "child couldn't close listen socket\n");
        exit(EXIT_FAILURE);
      }

      /* See httpreq.h for definition */
      struct http_request new_request;
      /* 6) call helper function to read the request
       * this will fill in the struct new_request for you
       * see helper.h and httpreq.h
       */
      /* START CODE SNIPPET 6 */

      // Try parsing the HTTP request
      if (!Parse_HTTP_Request(connection_socket, &new_request)) {
        // Parsing failed, assume the request format is incorrect
        status_code = 400;
        status_phrase = "Bad Request";
      } else {
        // Check if the method is recognized
        if (strcmp(new_request.method, "GET") == 0 ||
            strcmp(new_request.method, "HEAD") == 0) {
          // Method is recognized; further logic can determine the correct
          // status code
          if (Is_Valid_Resource(new_request.URI)) {
            // Resource exists
            status_code = 200;
            status_phrase = "OK";
          } else {
            // Resource does not exist
            status_code = 404;
            status_phrase = "Not Found";
          }
        } else if (strcmp(new_request.method, "PUT") == 0 || strcmp(new_request.method, "DELETE") == 0 || strcmp(new_request.method, "LINK") == 0 || strcmp(new_request.method, "UNLINK") == 0) {
          status_code = 501;
          status_phrase = "Not Implemented";
          } else {
          status_code = 400;
          status_phrase = "Bad Request";
        }
      }

      // // Special handling for HEAD requests; send headers only
      // if (status_code == 200 && strcmp(new_request.method, "HEAD") == 0) {
      //   send(connection_socket, response_buffer, strlen(response_buffer), 0);
      // } else if (status_code == 200 && strcmp(new_request.method, "GET") == 0) {
      //   // For GET requests with valid resources, send headers first
      //   send(connection_socket, response_buffer, strlen(response_buffer), 0);
      // }

      /* END CODE SNIPPET 6 */

      /* 7) Decide which status_code and reason phrase to return to client */
      /* START CODE SNIPPET 7 */
      // Combine with SNIPPET 6
      /* END CODE SNIPPET 7 */

      /* 8) Set the reply message to the client
       * Copy the following line and fill in the ??
       * sprintf(response_buffer, "HTTP/1.0 %d %s\r\n", ??, ??);
       */
      /* START CODE SNIPPET 8 */
      sprintf(response_buffer, "HTTP/1.0 %d %s\r\n", status_code,
              status_phrase);

      /* END CODE SNIPPET 8 */

      printf("Sending response line: %s\n", response_buffer);

      /* 9) Send the reply message to the client
       * Copy the following line and fill in the ??
       * send(??, response_buffer, strlen(response_buffer), 0);
       */
      /* START CODE SNIPPET 9 */

      send(connection_socket, response_buffer, strlen(response_buffer), 0);

      /* END CODE SNIPPET 9 */

      bool is_ok_to_send_resource = false;
      /* 10) Send resource (if requested) under what condition will the
       * server send an entity body?
       */
      /* START CODE SNIPPET 10 */

      if (status_code == 200 && strcmp(new_request.method, "GET") == 0) {
        // Only send body for GET requests when OK
        is_ok_to_send_resource = true;
      } else if (status_code == 200 && strcmp(new_request.method, "HEAD") == 0) {
        Send_Head(connection_socket, new_request.URI);
        is_ok_to_send_resource = false;
      } else {
            is_ok_to_send_resource = false;
          }

        /* END CODE SNIPPET 10 */

        if (is_ok_to_send_resource) {
          Send_Resource(connection_socket, new_request.URI);
        } else {
          /* 11) Do not send resource
           * End the HTTP headers
           * Copy the following line and fill in the ??
           * send(??, "\r\n\r\n", strlen("\r\n\r\n"), 0);
           */
          /* START CODE SNIPPET 11 */
        
          send(connection_socket, "\r\n\r\n", strlen("\r\n\r\n"), 0);

        /* END CODE SNIPPET 11 */
      }

      /* Child's work is done
       * Close remaining descriptors and exit
       */
      if (connection_socket >= 0) {
        if (close(connection_socket) < 0) {
          fprintf(stderr, "closing connected socket failed\n");
          exit(EXIT_FAILURE);
        }
      }

      /* All done return to parent */
      exit(EXIT_SUCCESS);
      }
    /*----------END OF CHILD CODE----------------*/

    /* Back in parent process
     * Close parent's reference to connection socket,
     * then back to top of loop waiting for next request
     */
    if (connection_socket >= 0) {
      if (close(connection_socket) < 0) {
        fprintf(stderr, "closing connected socket failed\n");
        exit(EXIT_FAILURE);
      }
    }

    /* if child exited, wait for resources to be released */
    waitpid(-1, NULL, WNOHANG);
  }
}