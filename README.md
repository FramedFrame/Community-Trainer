Community-Trainer
=================

Community Trainer Devlopment @ CCPLZ


Build Instructions
==================

Requirments:
http://puu.sh/3oJOb.7z
7 Zip
Boost
Visual Studio 12

1. Download boost and compile it by following the instructions on their web page
2. Unzip the 7z Archive and move the file: 'put this in your boost folder(where libs and boost is).bat' into your boost directory(CAUTION: Example -> F:\Projects-External\Boost\boost_1_53_0)

INFORMATION:
Run the files as Admin

3. Run the bat file
4. Run the file: 'set enviroment variables.bat'



5. Open the project with Visual Studio 12(CommunityProject) and then compile CommunityLib
6. Edit the MAPLE_PATH define into your Maple Path(CommunityHost -> main.cpp)
7. Edit this: this->m_strLibary = "C:\\Users\\Home\\Documents\\Community-Trainer\\Debug\\CommunityClient.dll"; to your Dll Path
6. Compile CommunityClient and CommunityHost
7. Run CommunityHost as Admin
