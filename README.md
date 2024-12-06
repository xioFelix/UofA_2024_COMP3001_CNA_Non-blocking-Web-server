
# Web Server Practical

In this practical, you will develop a non-blocking web server capable of handling multiple concurrent requests in parallel. By the end, you will have built a server that can return your home page to a browser and generate standards-compliant HTTP responses.

## Resources to Study

- **HTTP/1.1 and older versions**  
- [RFC 1945](https://www.ietf.org/rfc/rfc1945.txt) – Focus on request and response sections.  
- Use `curl -v` and Wireshark to view the raw HTTP data sent and received.

**Tip for Felix:** Pay special attention to the required HTTP headers and formatting, especially request methods, status codes, and response headers.

## Development Outline

1. **Create a Socket**:  
   - Set up a listening socket (`listen_socket`) and a connection socket (`connection_socket`).

2. **Bind and Listen**:  
   - Bind the `listen_socket` to a port number greater than 1024.
   - Start listening for incoming connections.

3. **Accept Connections**:  
   - Use `accept()` to block until a client connects.
   
4. **Parallel Processing via fork()**:  
   - For each new connection, `fork()` a child process to handle the request.
   - The parent process returns to accept new connections immediately.
   
5. **Parse HTTP Request**:  
   - Call `Parse_HTTP_Request()` to fill a `http_request` structure with the client’s request details (method, URI, etc.).

6. **Determine the Response**:  
   - Check the request method (e.g., GET, HEAD) and validity.
   - Decide on the appropriate HTTP status code and phrase (e.g., 200 OK, 404 Not Found, 400 Bad Request, 501 Not Implemented).
   - For GET requests and valid resources, prepare to send the file.  
   - For HEAD requests or invalid scenarios, adjust the response accordingly.

7. **Send Response**:  
   - First, send the status line and the required headers.
   - If it’s a GET request and the resource exists, use `Send_Resource()` to send the content.
   - If it’s a HEAD request or no resource is needed, send headers only.

8. **Cleanup and Close**:  
   - Close the sockets and terminate the child process after sending the response.
   - The parent process continues accepting new requests in parallel.

**Tip for Felix:** Ensure your HTTP responses strictly follow the protocol format. For example, include `\r\n` line endings, the correct `Content-Length`, and handle various status codes properly.

## Testing

- **Web Browser**:  
  Open `http://localhost:[port]/index.html` in a browser.  
  If configured correctly, you should see your test page or a correct error response.
  
- **cURL**:  
  ```bash
  curl -v -X GET http://localhost:[port]/index.html
  ```

- **telnet**:  
  ```bash
  telnet localhost [port]
  GET /index.html HTTP/1.1
  ```
  
You can verify that the server returns a proper HTTP response, including the status line, headers, and (if applicable) the requested content.