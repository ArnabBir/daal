# Intel&reg; Data Analytics Acceleration Library
Intel&reg; Data Analytics Acceleration Library (Intel&reg; DAAL) helps speed up big data analysis by providing highly optimized algorithmic building blocks for all stages of data analytics (preprocessing, transformation, analysis, modeling, validation, and decision making) in batch, online, and distributed processing modes of computation.

## License
Intel DAAL is licensed under [Apache License, Version 2.0](http://www.apache.org/licenses/LICENSE-2.0).

## Online Documentation
You can find the latest Intel DAAL documentation on [this page](https://software.intel.com/en-us/intel-daal-support/documentation).

We also have many [tutorials and training
materials](https://software.intel.com/en-us/intel-daal-support/training), and an actively monitored
[user forum](https://software.intel.com/en-us/forums/intel-data-analytics-acceleration-library).

## How to Contribute
We welcome community contributions to Intel DAAL. If you have an idea how to improve the product:

* Let us know about your proposal via [https://github.com/01org/daal/issues](https://github.com/01org/daal/issues)
* Make sure you can build the product and run all the examples with your patch
* In case of a larger feature, provide a relevant example
* Submit a pull request at [https://github.com/01org/daal/pulls](https://github.com/01org/daal/pulls)

We will review your contribution and, if any additional fixes or modifications are necessary, may give some feedback to guide you. When accepted, your pull request will be merged into our internal and GitHub* repositories.

Intel DAAL is licensed under [Apache License, Version
2.0](http://www.apache.org/licenses/LICENSE-2.0). By contributing to the
project, you agree to the license and copyright terms therein and release your
contribution under these terms. 

## <a name="system-requirements"></a>System Requirements
Intel DAAL supports the IA-32 and Intel&reg; 64 architectures. For a detailed explanation of these architecture names, read the [Intel Architecture Platform Terminology for Development Tools](https://software.intel.com/en-us/articles/intel-architecture-platform-terminology-for-development-tools) article.

The lists below contain the system requirements necessary to support application development with Intel DAAL. We tested Intel DAAL on the operating systems and with the compilers listed below, but Intel DAAL is expected to work on many more Linux* distributions as well.

Let us know if you have any trouble with the distribution you are using.

### Validated Operating Systems
* Windows* 8 (IA-32 / Intel&reg; 64)
* Windows* 8.1 (IA-32 / Intel&reg; 64)
* Windows* 10 (IA-32 / Intel&reg; 64)
* Windows Server* 2008 R2 SP1 and SP2
* Windows HPC Server* 2008 R2
* Windows Server* 2012
* Red Hat Enterprise Linux* 6 (IA-32 / Intel&reg; 64)
* Red Hat Enterprise Linux* 7
* Red Hat Fedora Core* 20 (IA-32 / Intel&reg; 64)
* Red Hat Fedora Core* 23 (IA-32 / Intel&reg; 64)
* Red Hat Fedora Core* 24
* SUSE Linux Enterprise Server* 11
* SUSE Linux Enterprise Server* 12
* Debian GNU/Linux* 8 (IA-32 / Intel&reg; 64)
* Ubuntu* 14.04 LTS (IA-32 / Intel&reg; 64)
* Ubuntu* 15.10 (IA-32 / Intel&reg; 64)
* OS X\* 10.11 (Xcode* 7.0)

### Validated C/C++ Compilers for Windows*
* Intel&reg; C++ Compiler 16.0 for Windows* OS
* Intel&reg; C++ Compiler 17.0 Beta for Windows* OS
* Microsoft Visual Studio* 2013
* Microsoft Visual Studio* 2015

### Validated C/C++ Compilers for Linux*
* Intel&reg; C++ Compiler 16.0 for Linux* OS
* Intel&reg; C++ Compiler 17.0 Beta for Linux* OS
* GNU Compiler Collection* 5.1 and later

### Validated C/C++ Compilers for OS X*
* Intel&reg; C++ Compiler 16.0 for OS X*
* Intel&reg; C++ Compiler 17.0 Beta for OS X*
* Clang\* from Xcode* 7

### Validated Java* Compilers:
* Java\* SE 8 from Sun Microsystems*

## Installation
You can install Intel DAAL from the provided binary packages or from the GitHub* sources.

For platform-specific getting started documents, see the following pages:

* [Getting Started with Intel&reg; Data Analytics Acceleration Library for Windows*](https://software.intel.com/en-us/get-started-with-daal-for-windows)
* [Getting Started with Intel&reg; Data Analytics Acceleration Library for Linux*](https://software.intel.com/en-us/get-started-with-daal-for-linux)
* [Getting Started with Intel&reg; Data Analytics Acceleration Library for OS X*](https://software.intel.com/en-us/get-started-with-daal-for-osx)

### Installing from the Binaries
You can download an archive from the GitHub\* release page at [https://github.com/01org/daal/releases](https://github.com/01org/daal/releases). This archive contains a script to set the environment variables for library usage in the *daal/bin* directory.

If you have issues with running the script, you may need to replace the *INSTALLDIR* string in *daal/bin/daalvars.sh* and/or *daal/bin/daalvars.csh* with the name of the directory where you unpacked the archive.

### Installing from the Sources

#### Required Software
* C/C++ compiler (see [System Requirements](#system-requirements))
* Java\* JDK (see [System Requirements](#system-requirements))
* Microsoft Visual Studio\* (Windows* only)
* [http://msys2.github.io](http://msys2.github.io) with the msys/make package (Windows* only); install the package as follows:

        pacman -S msys/make

#### Installation Steps
1. Clone the sources from GitHub* as follows:

        git clone --recursive https://github.com/01org/daal.git

2. Set the PATH environment variable to the MSYS2\* bin directory (Windows* only); for example:

        set PATH=C:\msys64\usr\bin;%PATH%

3. Set an environment variable for Microsoft Visual Studio\* (Windows* only); for example:

        call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x64

4. Set an environment variable for one of the supported C/C++ compilers

5. Set an environment variable for one of the supported Java* compilers; for example:

        set PATH=C:\Program Files\Java\jdk1.8.0_77\bin;%PATH%
        set INCLUDE=C:\Program Files\Java\jdk1.8.0_77\include;C:\Program Files\Java\jdk1.8.0_77\include\win32;%INCLUDE%

6. Build Intel DAAL via the command-line interface with the following commands, depending on your platform:

 *  on Linux\* using Intel&reg; C++ Compiler:

            make daal PLAT=lnx32e

 *  on Linux\* using GNU Compiler Collection\*:

            make daal PLAT=lnx32e COMPILER=gnu

 *  on OS X* using Intel&reg; C++ Compiler:

            make daal PLAT=mac32e

 *  on OS X\* using Clang\*:

            make daal PLAT=mac32e COMPILER=clang

 *  on Windows* using Intel&reg; C++ Compiler:

            make daal PLAT=win32e

 *  on Windows\* using Microsoft Visual* C++ Compiler:

            make daal PLAT=win32e COMPILER=vc

Built libraries are located in the *\_\_release\_{os_name}/daal* directory.

## Python*
<!--- Add this back when we are clear about Python support.
Intel DAAL can be also used with Python\* interfaces. You can find the pyDAAL package at [http://anaconda.org/intel/pydaal](http://anaconda.org/intel/pydaal).
-->
Python support is not available for the DAAL open source project at this time.
But it is available in the proprietary edition of [Intel DAAL 2017
Beta](https://software.intel.com/en-us/articles/intel-daal-2017-release-notes).
Users can request Intel DAAL 2017 Beta from
[here](https://softwareproductsurvey.intel.com/f/150587/1103/).

## See Also
* [Intel&reg; DAAL Product Page](https://software.intel.com/en-us/intel-daal)
* [Intel&reg; DAAL Forum](https://software.intel.com/en-us/forums/intel-data-analytics-acceleration-library)
