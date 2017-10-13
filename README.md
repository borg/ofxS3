
**ofxS3**  
This has been compiled on OSX only. Feel free to contribute and compile for other platforms.

**Usage**
`
 //key is the AWS file name
string key = ofGetTimestampString()+".png";
string bucket = "mybucket";
string file = "the file.png";
bool makePublic = true;
string region = "us-east-1";
ofxS3::upload(bucket,key,file,makePublic,region);
    
    
vector<string> ll = ofxS3::listBucket(bucket);
    
    
 string toDirectory = "absolutePath/data";
 ofxS3::download(bucket, "2017-10-06-12-19-51-318.png", toDirectory);
    
 ofxS3::deleteRemoteFile(bucket, "index.htm");

`  
Listen to complete events if more asynchronous approach is needed.


**How to install**   

`
Add libraries to the project by
1. Include dylibs in ofxS3 folder path

2.Library search path
../../ExternalAddons/ofxS3/libs/aws/lib

3. Do NOT include headers in ofxS3 tree and do NOT include any Header search path other than
/Library/Frameworks/GStreamer.framework/Headers
../../ExternalAddons/ofxS3/src
../../ExternalAddons/ofxS3/libs/aws/lib
(Including the headers directly created conflict with native ctime header)


4. In Other C++ flags link
-I../../ExternalAddons/ofxS3/libs/aws/include

5. Drag and drop dylibs into Build Phases>Copy files (to framework)
`


Get access with your account key
http://docs.aws.amazon.com/sdk-for-cpp/v1/developer-guide/credentials.html


This worked...not the environment variables
Put into ~/.aws/credentials
[default]
aws_access_key_id = xxxx
aws_secret_access_key = xxxx



**Note on compilation**  
To make the dylibs portable I had to change all the install paths when compiling
https://stackoverflow.com/questions/24993752/os-x-framework-library-not-loaded-image-not-found

/absolutepath/aws-sdk-cpp/build/aws-cpp-sdk-s3/Debug
to @executable_path/../Frameworks

I change every instance. 
When adding dylib also add to copy to framework in build settings


**License**  
Whatever AWS C++ SDK license is under.
