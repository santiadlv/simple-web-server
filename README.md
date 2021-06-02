# Simple Web Server in C
In order to run the program, you first need to download the source code and put all files in the same directory. You can then proceed to the compilation of the program with your preferred toolset. In our case we opted for GCC and ran `gcc webserver.c -o webserver`.

After that, you can execute `./webserver` in any Unix environment to boot up the server and start listening for requests.

------------

The routes that are supported by this server, in order to showcase the class syllabus, are the following:
- [http://localhost:8080/TC2025.html](http://localhost:8080/TC2025.html "http://localhost:8080/TC2025.html")
- [http://localhost:8080/syllabus](http://localhost:8080/syllabus "http://localhost:8080/syllabus")

Any other route will default to the `index.html` file and show a 404 NOT FOUND error code.