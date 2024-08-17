# CmyMessage

<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#results">Results</a></li>
    <li><a href="#what-i-learned">What I learned until now!</a></li>
    <li><a href="#plan-/-roadmap">Plan / Roadmap</a></li>
  </ol>
</details>

<!-- ABOUT THE PROJECT -->
## About the project
Concurrent servers made in _C_: a simplified <a href="https://www.rfc-editor.org/rfc/rfc959">File transfer Protocol<a/>
server and a broadcast chat,<br/>
both made upon a layer of abstration of mine,<br/>
and aiming to foster and apply the knowledge of Computer Networks and Operating Systems courses.<br/>

▶️ Curious? Click <a href="https://youtu.be/v7n3sTKgzqM">here</a> for video demonstration!

<!-- HOW TO START IT -->
## Getting started
### Prerequisites
- C programming language
- C compiler (gcc, for example)
- Make

## Usage
```bash
cd 1_FTP # for example
make
```

## Results
- Simplified FTP service (<a href="https://youtu.be/v7n3sTKgzqM">here</a>)
- Broadcast Chat (<a href="https://www.youtube.com/watch?v=_FiXoDS4TtU">here</a>)
- Common layer of abstraction (<a href="">here</a>)

https://www.youtube.com/watch?v=_FiXoDS4TtU&list=PLg2QGjEYh2JA3MHilWrZN3onc1rdP3udf&index=1

## What I learned more about!
Had fun knowing more about:
* C programming language
  * careful memory management
    * good practices
  * multi-threading
    * race conditions
    * thread pools with condition variables
    * I/O assync operations
    * mutex locks
    * semaphors
* Networking
  * IPv4 mapped-addresses, dual stack network
  * Bloom filters and routing tables
* Information security practices
  * Input sanitization
  * Forms of AUTH (and its fragilities)
  * Shadow networks
  * Hashes (its fragilities, some types, ...)
* Cmake (build system generator)
* Software architecture 

## Plan / Roadmap 
✅ = implemented <br />
🛠️ = in process... <br />
🔁 = being applied to <a href="https://github.com/axelcarapinha/Lexigraph">_Lexigraph_</a> (another project) <br />
🔁 = being applied with <a href="https://github.com/axelcarapinha/CTFs">CTFs</a><br />

- Plan the architecture / structure of the code ✅
- Simple connection ✅
- Chat between clients 🛠️ 
  - Supporting concurrency ✅
  - Generic hash table for the user's struct ✅
  - Broadcast chat ✅
  - Direct chat 🛠️
- Containers (probably with Docker compose) 🔁
  - Docker Swarm (ease the scalability)
- Database 🔁
    - Designing the schema 🔁
    - Making the queries 🔁
    - Connecting it 🔁
- Authentication (probalby with JWT tokens) 🔁
  - Hashing + Salting
- Security check, like avoiding: 🔁
  - Open ports dangers 🔁
  - SQL injection 🔁
  - Command and code injection 🔁
  - Buffer overflows 🔁
  - ... 🔁
- Pentests to check for vulnerabilities 🔁
- Deployment 🔁
    - Consider using shared object files for shared code
    - Server setup and configuration (probably in an Azure VM) 🔁
    - Documentation of the code 🔁
