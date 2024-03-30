# CmyMessage
C chat application done for learning purposes. 

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
    <li><a href="#what-i-learned">What I learned!</a></li>
  </ol>
</details>

<!-- ABOUT THE PROJECT -->
## About the project
A chat application made with C.

<!-- HOW TO START IT -->
## Getting started
### Prerequisites
1. 

### Installation and usage

## Results

## What I learned until now!
Had fun knowing more about:
* C programming language
  * careful memory management
    * good practices
  * multi-threading
    * race conditions
    * thread pools with condition variables
    * I/O assync operations
    * 
    * mutex locks
    * semaphors
* Networking
  * IPv4 mapped-addresses, dual stack network
  * Bloom filters and routing tables
  * 
* Information security practices
  * Input sanitization
  * Forms of AUTH (and its fragilities)
  * Shadow networks
  * Hashes (its fragilities, some types, ...)
  * 
* Cmake (build system generator)
* Software architecture 


## Plan / Roadmap 
✅ = implemented
🛠️ = in process...
🔁 = NOT implemented

- Plan the architecture / structure of the code ✅
- Simple connection ✅
- Chat between clients 🛠️ 
  - Supporting concurrency ✅
  - Generic hash table for the user's struct ✅
  - Broadcast chat 🛠️
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

  
  









