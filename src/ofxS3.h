// =============================================================================
//
// ofxS3.h
// s3
//
// Created by Andreas Borg on 10/5/17
//
// Copyright (c) 2015-2017 Andreas Borg <http://crea.tion.to>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================

/*

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




Trial and error...gave these results

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



Get access with your account key
http://docs.aws.amazon.com/sdk-for-cpp/v1/developer-guide/credentials.html


This worked...not the environment variables
Put into ~/.aws/credentials
[default]
aws_access_key_id = xxxx
aws_secret_access_key = xxxx


To make the dylibs portable I had to change all the install paths when compiling
https://stackoverflow.com/questions/24993752/os-x-framework-library-not-loaded-image-not-found

/absolutepath/aws-sdk-cpp/build/aws-cpp-sdk-s3/Debug
to @executable_path/../Frameworks

I change every instance. 
When adding dylib also add to copy to framework in build settings


*/

#ifndef _ofxS3
#define _ofxS3


#include "ofMain.h"

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/DeleteObjectRequest.h>
#include <aws/s3/model/Object.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <iostream>
#include <fstream>

class S3Event : public ofEventArgs {
public:
    S3Event(){};
    string file = "";
    string error = "";
    string url = "";
    string bucket ="";
    vector<string> bucketList;
    
    static ofEvent <S3Event> FILE_UPLOADED;
    static ofEvent <S3Event> ERROR_UPLOAD;
    
    static ofEvent <S3Event> BUCKET_CREATED;
    static ofEvent <S3Event> ERROR_CREATE_BUCKET;
    
    static ofEvent <S3Event> BUCKET_LISTED;
    static ofEvent <S3Event> ERROR_LISTING;
    
    static ofEvent <S3Event> FILE_DOWNLOADED;
    static ofEvent <S3Event> ERROR_DOWNLOAD;
    
    static ofEvent <S3Event> FILE_DELETED;
    static ofEvent <S3Event> ERROR_DELETE;
};


class ofxS3 {
	
  public:
	
	ofxS3(){};
    ~ofxS3(){};
	
    static void upload(string bucket, string key, string file, bool makePublic = true,string region = "",string contentType = "binary/octet-stream"){
    
        if(!ofFile::doesFileExist(file)){
            S3Event e;
            e.file = file;
            e.error = "Local file does not exist";
            ofNotifyEvent(S3Event::ERROR_UPLOAD, e);
            cout<<"ofxS3 error: "+e.error<<endl;
            return;
        }
        
        Aws::SDKOptions options;
        Aws::InitAPI(options);
        {
        
        
            const Aws::String bucket_name = bucket.c_str();
            const Aws::String key_name = key.c_str();
            const Aws::String file_name = file.c_str();
            const Aws::String region = region.c_str();
            
            cout << "Uploading " << file_name << " to S3 bucket " <<
                bucket_name << " at key " << key_name << endl;

            Aws::Client::ClientConfiguration clientConfig;
            clientConfig.requestTimeoutMs = 10000;//default is 3sec, but it sometimes is too short
            clientConfig.connectTimeoutMs = 10000;//default is 1sec
            if (!region.empty())
                clientConfig.region = region;
            Aws::S3::S3Client s3_client(clientConfig);

            Aws::S3::Model::PutObjectRequest object_request;
            object_request.WithBucket(bucket_name).WithKey(key_name);
            
            if(makePublic){
                //make object public readable
                object_request.SetACL(Aws::S3::Model::ObjectCannedACL::public_read);
            }
            
            
            
            
            // Binary files must also have the ios_base::bin flag or'ed in
            auto input_data = Aws::MakeShared<Aws::FStream>("PutObjectInputStream",
                file_name.c_str(), ios_base::in | ios_base::binary);

            object_request.SetBody(input_data);
            
            //make sure this is set correctly
            object_request.SetContentType(contentType.c_str());
            

            auto put_object_outcome = s3_client.PutObject(object_request);

            if (put_object_outcome.IsSuccess()){
                S3Event e;
                e.file = file;
                e.url = "https://s3.amazonaws.com/"+bucket+"/"+key;
                

                ofNotifyEvent(S3Event::FILE_UPLOADED, e);
                cout << "Successfully uploaded to "<<e.url << endl;
            }else{
                
                S3Event e;
                e.file = file;
                stringstream ss;
                ss << put_object_outcome.GetError().GetExceptionName() << " " <<
                    put_object_outcome.GetError().GetMessage()<< endl;
                e.error = ss.str();
                

                ofNotifyEvent(S3Event::ERROR_UPLOAD, e);
                
                
                cout << "PutObject error: " <<
                    put_object_outcome.GetError().GetExceptionName() << " " <<
                    put_object_outcome.GetError().GetMessage() << endl;
            }
            
        }
        Aws::ShutdownAPI(options);
    
    };
    
    
    
    static void download(string bucket, string key, string absoluteDirectory){
    
        Aws::SDKOptions options;
        Aws::InitAPI(options);
        {
        
        
            const Aws::String bucket_name = bucket.c_str();
            const Aws::String key_name = key.c_str();

            
            cout << "download " << key_name << " from S3 bucket " <<
                bucket_name <<endl;

            Aws::Client::ClientConfiguration clientConfig;
            clientConfig.requestTimeoutMs = 10000;//default is 3sec, but it sometimes is too short
            clientConfig.connectTimeoutMs = 10000;//default is 1sec
        
            Aws::S3::S3Client s3_client(clientConfig);


            Aws::S3::Model::GetObjectRequest object_request;
            object_request.WithBucket(bucket_name).WithKey(key_name);

            auto get_object_outcome = s3_client.GetObject(object_request);

            if (get_object_outcome.IsSuccess()){
                Aws::OFStream local_file;
                local_file.open(key_name.c_str(), ios::out | ios::binary);
                local_file << get_object_outcome.GetResult().GetBody().rdbuf();
                local_file.close();
                
                if(!ofDirectory::doesDirectoryExist(absoluteDirectory)){
                    bool bRelativeToData = false;
                    bool recursive = true;
                    ofDirectory::createDirectory(absoluteDirectory,bRelativeToData,recursive);
                }

                string downloadedFile = absoluteDirectory+"/"+key;
                filesystem::rename(key_name.c_str(),downloadedFile);
	                
                
                cout <<"File downloaded to "<< downloadedFile<<" "<< endl;
                
                S3Event e;
                e.file = downloadedFile;
                ofNotifyEvent(S3Event::FILE_DOWNLOADED, e);
    
                
            } else{
    
                     
                S3Event e;
                e.file = key;
                stringstream ss;
                ss << get_object_outcome.GetError().GetExceptionName() << " " <<
                    get_object_outcome.GetError().GetMessage()<< endl;
                e.error = ss.str();
                

                ofNotifyEvent(S3Event::ERROR_DOWNLOAD, e);
                
                 cout << "Download error: " <<
                    get_object_outcome.GetError().GetExceptionName() << " " <<
                    get_object_outcome.GetError().GetMessage() << endl;

            }

            
        }
        Aws::ShutdownAPI(options);
    
    };
    
    
    
    
    static void deleteRemoteFile(string bucket, string key){
    
        Aws::SDKOptions options;
        Aws::InitAPI(options);
        {
        
        
            const Aws::String bucket_name = bucket.c_str();
            const Aws::String key_name = key.c_str();

            
            cout << "download " << key_name << " from S3 bucket " <<
                bucket_name <<endl;

            Aws::Client::ClientConfiguration clientConfig;
            clientConfig.requestTimeoutMs = 10000;//default is 3sec, but it sometimes is too short
            clientConfig.connectTimeoutMs = 10000;//default is 1sec
        
            Aws::S3::S3Client s3_client(clientConfig);




            Aws::S3::Model::DeleteObjectRequest object_request;
            object_request.WithBucket(bucket_name).WithKey(key_name);

            auto delete_object_outcome = s3_client.DeleteObject(object_request);

            if (delete_object_outcome.IsSuccess()){
                cout << "File deleted "<<key << endl;//this seems to yield success even if file doesn't exist. Funny ppl.
                S3Event e;
                e.file = key;
                ofNotifyEvent(S3Event::FILE_DELETED, e);
    
            }else {
                cout << "DeleteObject error: " <<
                    delete_object_outcome.GetError().GetExceptionName() << " " <<
                    delete_object_outcome.GetError().GetMessage() << endl;
                
                S3Event e;
                e.file = key;
                stringstream ss;
                ss << delete_object_outcome.GetError().GetExceptionName() << " " <<
                    delete_object_outcome.GetError().GetMessage()<< endl;
                e.error = ss.str();
                ofNotifyEvent(S3Event::ERROR_DELETE, e);
            }
            
        }
        Aws::ShutdownAPI(options);
    
    };
    
    static vector<string> listBucket(string bucket){
        
        vector<string>list;
        
        Aws::SDKOptions options;
        Aws::InitAPI(options);
        {
        
            Aws::Client::ClientConfiguration clientConfig;
            clientConfig.requestTimeoutMs = 10000;//default is 3sec, but it sometimes is too short
            clientConfig.connectTimeoutMs = 10000;//default is 1sec
            Aws::S3::S3Client s3_client(clientConfig);
                
                
                
            
            const Aws::String bucket_name = bucket.c_str();
            
            
            
            
            Aws::S3::Model::ListObjectsRequest objects_request;
            objects_request.WithBucket(bucket_name);

            auto list_objects_outcome = s3_client.ListObjects(objects_request);

            if (list_objects_outcome.IsSuccess()){
                Aws::Vector<Aws::S3::Model::Object> object_list =
                    list_objects_outcome.GetResult().GetContents();

                for (auto const &s3_object : object_list){
                    cout <<s3_object.GetKey() << endl;
                    stringstream ss;
                    ss << s3_object.GetKey();
                    list.push_back(ss.str());
                }
                
                S3Event e;
                e.bucket = bucket;
                e.bucketList = list;

                ofNotifyEvent(S3Event::BUCKET_LISTED, e);

                return list;
            }else{
                cout << "ListObjects error: " <<
                    list_objects_outcome.GetError().GetExceptionName() << " " <<
                    list_objects_outcome.GetError().GetMessage() << endl;
                
                
                    S3Event e;
                    e.bucket = bucket;
                    stringstream ss;
                    ss << list_objects_outcome.GetError().GetExceptionName() << " " <<
                        list_objects_outcome.GetError().GetMessage()<< endl;
                    e.error = ss.str();
                    

                    ofNotifyEvent(S3Event::ERROR_LISTING, e);
            }
            
        
        }
        Aws::ShutdownAPI(options);
        
        return list;
    
    }



    static void createBucket(string bucket){
        
        
        Aws::SDKOptions options;
        Aws::InitAPI(options);
        {
    
        
            Aws::Client::ClientConfiguration clientConfig;
            clientConfig.requestTimeoutMs = 10000;//default is 3sec, but it sometimes is too short
            clientConfig.connectTimeoutMs = 10000;//default is 1sec
            Aws::S3::S3Client s3_client(clientConfig);
                
            const Aws::String bucket_name = bucket.c_str();
            
                
            Aws::S3::Model::CreateBucketRequest request;
            request.SetBucket(bucket_name);

            auto outcome = s3_client.CreateBucket(request);

            if (outcome.IsSuccess()){
                cout << "Created bucket " <<bucket<< endl;
                
                S3Event e;
                e.bucket = bucket;
                ofNotifyEvent(S3Event::BUCKET_CREATED, e);
            }else{
                cout << "CreateBucket error: "
                    << outcome.GetError().GetExceptionName() << endl
                    << outcome.GetError().GetMessage() << endl;
                
                    S3Event e;
                    e.bucket = bucket;
                    stringstream ss;
                    ss << outcome.GetError().GetExceptionName() << " " <<
                        outcome.GetError().GetMessage()<< endl;
                    e.error = ss.str();
                
                    ofNotifyEvent(S3Event::ERROR_CREATE_BUCKET, e);
            }

        
        }
        Aws::ShutdownAPI(options);
        

    
    }

	
};

#endif
