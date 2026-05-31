
// NOTE(s):
// - Implementation of httpServer make difficult to implement
//   - httpRequestHandlerCallback() function as own ".cpp" source code module
//   - wsRequestHandlerCallback()   function as own ".cpp" source code module


// A callback function that would handle HTTP requests, it returns the content part of HTTP reply, the header will be added later by the HTTP server
String httpRequestHandlerCallback (const char *httpRequest, httpServer_t::httpConnection_t *hcn)
{
  // Let the httpServer handle the request itself
  return "";
}


// A callback function that would handle HTTP WebSocket requests
void wsRequestHandlerCallback (const char *httpRequest, httpServer_t::webSocket_t *webSck)
{
  // Must be reentrant !!!
}
