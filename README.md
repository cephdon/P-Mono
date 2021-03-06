# P-Mono
Version of P tool chain for Linux/MacOS using Mono

## Installation
Dependencies for this project for now are mono, P
#### Dependencies
  - First you need to install mono. You may find installation guide from the [Mono-project]( http://www.mono-project.com/docs/getting-started/install/ "Mono Project")
  - Then you can clone this repo
```{r, engine='bash', count_lines}
  $ git clone git@github.com:p-org/P-Mono.git
```
or if you prefer https:
```{r, engine='bash', count_lines}
  $ git clone https://github.com/p-org/P-Mono.git
```
and fetch dependencies via git submodule
```{r, engine='bash', count_lines}
  $ git submodule update --init --recursive --remote
```
  - To avoid errors because of case-sensitive paths on some distribution of Linux systems, mono provides an option which is controlled by the *MONO_IOMAP* environment variable. You need to execute the following command to set the environment varibale before building 
```{r, engine='bash', count_lines}
  $ export MONO_IOMAP=case
```
You may also consider add the above export command to your `bashrc`. You can get more info [here](http://www.mono-project.com/archived/porting_msbuild_projects_to_xbuild/)

#### Build
  - The project uses *xbuild*, a port of *msbuild* which is shipped along with mono. To build the *P-Mono* project, under the project root directory:
```{r, engine='bash', count_lines}
  $ xbuild
```

## Usage
  - Once you have built the project, you should be able to find executables inside *Bld/Drop/*. You should find the P Compiler executable *Pc.exe* there. 
  - You can now try running the P Compiler executable with mono.
```{r, engine='bash', count_lines}
  $ mono path_to_executable/Pc.exe
```

## Reference

[Original P repo](https://github.com/p-org/P "P Github Page")
