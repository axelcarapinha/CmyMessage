# CmyMessage
A server made in _C_ with multiple services.
⚠️ In progress!


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
A server made in _C_, with multiple services, such as a BroadcastChat, FTP, and more to come! It's a way of applying the knowledge of the Computer Networks' class to a considerably broader coding architecture and extension of topics.

▶️ Curious? Click <a href="#results">HERE</a> for video demonstrations!

<!-- HOW TO START IT -->
## Getting started
### Prerequisites
⚠️ In progress!

### Installation 
⚠️ In progress!

## Usage
⚠️ In progress!

## Results
<a href="https://youtu.be/v7n3sTKgzqM">FTP concurrent service (simple implementation)</a>

## What I learned until now!
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
  * 
* Information security practices
  * Input sanitization
  * Forms of AUTH (and its fragilities)
  * Shadow networks
  * Hashes (its fragilities, some types, ...)
* Cmake (build system generator)
* Software architecture 

## Plan / Roadmap 
✅ = implemented
🛠️ = in process...
🔁 = NOT implemented yet

- Plan the architecture / structure of the code ✅
- Simple connection ✅
- Chat between clients 🛠️ 
  - Supporting concurrency ✅
  - Generic hash table for the user's struct ✅
  - Broadcast chat ✅
  - Direct chat 🛠️

 <br />
The following goals were passed to another project in its majority (Lexigraph):
- Containers (probably with Docker compose) 
  - Docker Swarm (ease the scalability)
- Database 
    - Designing the schema 
    - Making the queries 
    - Connecting it 
- Authentication (probalby with JWT tokens) 
  - Hashing + Salting
- Security check, like avoiding: 
  - Open ports dangers 
  - SQL injection 
  - Command and code injection 
  - Buffer overflows 
  - ... 
- Pentests to check for vulnerabilities 
- Deployment 
    - Consider using shared object files for shared code
    - Server setup and configuration (probably in an Azure VM) 
    - Documentation of the code 
