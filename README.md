# Webserv
`webserv` is a simple yet powerful web server, written in C++ and inspired by Nginx's design. It's engineered to efficiently handle fundamental HTTP methods such as GET, POST, and DELETE, as well as execute CGI scripts. Our focus is to provide a lightweight and flexible server solution that is easy to configure and deploy for basic web serving needs and CGI scripting capabilities.

## Prerequisites
- GNU Make
- GCC 4.8 or higher

## Installation
```
git clone https://github.com/ilovenoah/webserv.git
cd webserv
make
```

## Config
- `server_name`: Specifies the domain name of the server.
- `listen`: Defines the ip and port number on which the server listens. Only user ports are allowed to bind.
- `root`: Sets the root directory for requests.
- `autoindex`: When set to "off", directory listings are disabled.
- `index`: Lists files that will be used as an index.
- `client_body_limit`: Limits the size of the client request body.
- `allow_methods`: Defines the allowed HTTP methods for a given location. This configuration supports `GET`, `POST`, and `DELETE` methods.
- `error_page`: Configures the URI that will be shown when an error occurs.
- `upload_store`: Designates the location where files uploaded from clients are stored.
- `cgi_extensions`: Defines the file extensions that will be processed as Common Gateway Interface (CGI) scripts. 
- `alias`: Sets a path to be used as an alternative to the specified location.
- `return`: Provides a URL to which the server will send a redirect response. 

## USAGE
```
./webserv /path/to/config
```

## Authors

- **TANE** - [GitHub](https://github.com/ShotaTanemura)
- **taiphyyy** - [GitHub](https://github.com/ilovenoah)
