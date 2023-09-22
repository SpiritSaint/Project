# Project

## About

> A programmer is measured by his ability to analyze scenarios and implement the changes required so that a solution becomes reality.

This repository contains a generic C++ project that can be used as a basis guide on application developments that require authentication and use of TCP/UDP and Websockets communication. 

## Instructions

### Encryption keys

```shell
openssl req -new -newkey rsa:1024 -nodes -out ca.csr -keyout ca.key -subj "/C=CL/ST=Santiago/L=Santiago/O=Project/OU=Project/CN=project.local"
```

## Rules ⚡

- Attributes must be declared using dash prefix.
- Classes, functions must declare using snake case convention.
- Variables must be declared nearest from where they are being used.
- Exceptions must be thrown if not perfect conditions are complaint.

## License

The code is distributed using author license and without guarantee explicit while the third-party code is distributed under their respective licenses. In relation of granted rights to third party in relationship with author code, any modification of the code is prohibited and its use is limited to academic purposes only, always safeguarding the rights of the author and his unilateral right.