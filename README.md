# HTTP Server 🌐

`http-server` is a project that implements an HTTP/1.0 server using TCP socket programming and threading in C. This provides practice in working with sockets, processes, and getting familiar with the HTTP protocol.

## Table of Contents

- [Features](#features)
- [Dependencies](#dependencies)
- [Installation](#installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)

## Features ✨

- HTTP server written in C
- Listens on TCP port 80
- Serves HTML, JavaScript, CSS, and image files (PNG)
- Supports concurrent HTTP/1.0 requests using threading
- Ability to handle 404 Not Found responses
- Generic implementation to handle any website with any number of objects and any filenames (with supported extensions)

## Dependencies 📚

- A C compiler (such as GCC) and a Unix-based system (Linux, macOS, etc.) with standard C libraries
- Pthreads library for threading

## Installation ⚙️

1. Clone this repository:
   ```
   git clone https://github.com/zulfkhar00/http-server.git
   ```
2. Compile the server program using the provided Makefile:

   ```
   cd http-server
   make server
   ```

3. This will create an executable: `./server`.

## Usage 🚀

1. Run the server program:

   ```
   ./server
   ```

   The server will open a TCP socket, listen on port 80, and handle incoming HTTP GET requests.

2. Open a web browser and enter the following URL:

   `http://127.0.0.1/index.html`

   The browser initiates an HTTP request to obtain the index.html file. Once the index.html file is received, the browser simultaneously sends requests for supplementary files mentioned within index.html, such as style.css, script.js, and logo.png.

3. After each request and successful response, the server will terminate the associated TCP connection.

4. If requests are made for files that do not exist (like index.htm, main.php, logo.jpg, favicon.ico, etc.), the server should reply with a 404 response. This response should include an HTML message indicating that the requested file was not found.

## Contributing 🤝

1. Fork the Repository

2. Clone your fork

3. Create your feature branch (`git checkout -b feature/AmazingFeature`)

4. Commit your changes (`git commit -m 'Add some AmazingFeature'`)

5. Push to the branch (`git push origin feature/AmazingFeature`)

6. Open a Pull Request

## License 📄

This project is licensed under the MIT License. Please see the [LICENSE](LICENSE) file for more details.
