# safer 

## Overview

* Windows Build Instructions

## Compatible

* OS
  * Windows 7 or later (Tested by 7 Professional and 10 Professional) 

## Requirement

* Xerces-C++
* libhunspell
* boost Version 1.58.0.0
* boost_program_options
* boost_regex
* boost_filesystem
* boost_system
* boost_date_time

## Tutorial for build (for Windows)

1. Get source code
  1. Example command : ` git.exe clone --recursive https://github.com/asugeno/safer.git . `
1. Open the "safer/safer.sln" by "Visual Studio 2013" (e.g. "Visual Studio Express 2013 for Windows Desktop")
1. Please step on following procedure below in Visual Studio.
  1. Add libhunspell and XercesLib and simplexlsx project to solution of Visual Studio.
    1. In Solution Explorer, right-click the "safer" solution, click Add, and then click Existing Project.
    1. In the Add Existing Project dialog box, select Hunspell and Xerces-C++ and simplexlsx project then click OK.
  1. Change the project Dependencies.
    1. In Solution Explorer, right-click the ER-StaticAnalysis solution , click Build Dependencies, and then click Project Dependencies.
    ![ProjectDependencies](image/ProjectDependencies.png?1)
    1. In Project Dependencies window. To check a box libhunspell and XercesLib and simplexlsx in "Depends on".
    ![ProjectDependenciesMenu](image/ProjectDependenciesMenu.png?1)
  1. Add reference in ER-StaticAnalysis project.
    1. In Solution Explorer, right-click the "ER-StaticAnalysis project". And Click "Properties". 
    ![ProjectReference](image/ProjectReference.png?1)
    1. In ER-StaticAnalysis Property Pages window, Click the "Reference" under "Common Properties".
    ![ProjectReference2](image/ProjectReference2.png?1)
    1. And click "Add New Reference".
    1. In "Add Reference" window, To check a box libhunspell and XercesLib and simplexlsx. And click "OK".
    ![ProjectReference3](image/ProjectReference3.png?1)
    1. In ER-StaticAnalysis Property Pages window, Switch to "true" at "Copy Local Satellite Assemblies", "Reference Assembly Output", "Link Library Dependencies", "User Library Dependency Inputs"
      1. Perform the same step as described above to "libhunspell".
    ![ProjectReference2](image/ProjectReference2.png?1)
1. Rename file
  1. Change the file name "hunvisapi.h.in" to "hunvisapi.h" in libhunspell dir
1. Change output dir on "XercesLib" project.
  1. In Solution Explorer, right-click the "XercesLib project". And Click "Properties". 
  ![XercesProjectSetting1](image/XercesProjectSetting1.png?1)
  1. In "XercesLib Properties" window, Click "General" under the "Configuration Properties". And change the "Output Directory" value to "$(SolutionDir)$(Configuration)".
  ![XercesProjectSetting2](image/XercesProjectSetting2.png?1)

## Licence

[MIT](https://opensource.org/licenses/mit-license.php)

## Author

[Akihiro Sugeno](https://github.com/asugeno)


