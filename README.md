[![testing](https://github.com/paul-caron/curlychungus/actions/workflows/testing.yml/badge.svg)](https://github.com/paul-caron/curlychungus/actions/workflows/testing.yml)

# 🐰 Curly Chungus

Curly Chungus is a robust, minimal WebDriver client for C++ built around `curling`. It speaks WebDriver Protocol fluently and doesn’t skip leg day.

- 🌀 Uses `curl` under the hood for raw HTTP power
- 🧠 Provides high-level browser automation features
- 🕸️ Supports any W3C-compliant WebDriver (e.g. GeckoDriver, ChromeDriver)

It’s like Selenium WebDriver, but thicc.

## Features

* **WebDriver Protocol:**  Implements the WebDriver protocol for controlling remote browsers.
* **C++17:**  Written in modern C++17 for performance and readability.
* **JSON Communication:**  Uses nlohmann::json for serializing and deserializing communication with the 
WebDriver server.
* **Curling Integration:** Leverages the `curling` library for efficient HTTP requests.
* **Comprehensive API:**  Provides a rich API for common browser actions, including:
    * Session management
    * Navigation
    * Element interaction
    * Script execution
    * Cookie management
    * Alert handling
    * Screenshot capture

## Dependencies

* **C++17 Compiler:**  A C++17 compatible compiler (e.g., GCC 8+, Clang 8+).
* **nlohmann/json:** A header-only JSON library.  
[https://github.com/nlohmann/json](https://github.com/nlohmann/json)
* **curling:** A modern C++ HTTP client library. 
[https://github.com/PonyUV/curling](https://github.com/paul-caron/curling)

## Installation

1.  **Clone the repository:**

    ```bash
    git clone https://github.com/paul-caron/curlychungus.git
    cd curlychungus
    ```

2.  **Install dependencies:**

    *   **nlohmann/json:**  Simply include the `json.hpp` header file. It's a header-only library.
    *   **curling:** You'll need to build and install `curling`.  Follow the instructions on the 
`curling` GitHub page (https://github.com/paul-caron/curling).


3.  **Compilation:**

    Use a `Makefile` (provided in the repository) or your preferred build system to compile the 
project.  For example:

    ```bash
    make
    ```

## Usage

```c++
#include "webdriver.hpp"

int main() {
  WebDriverClient client("http://localhost:4444"); // Replace with your WebDriver server URL

  // Create a session
  std::string sessionId = client.createSession();

  // Navigate to a URL
  client.navigateTo("https://www.example.com");

  // Get the current URL
  std::string currentUrl = client.getCurrentUrl();
  std::cout << "Current URL: " << currentUrl << std::endl;

  // Delete the session
  client.deleteSession();

  return 0;
}
```

## Contributing

Contributions are welcome!  Please submit pull requests with clear descriptions of your changes.  
Ensure your code adheres to the project's coding style.

## License

This project is licensed under the [MIT License](LICENSE).
