// =============================================================================
//
// ofxS3.cpp
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



#include "ofxS3.h"

ofEvent <S3Event> S3Event::FILE_UPLOADED;
ofEvent <S3Event> S3Event::ERROR_UPLOAD;
ofEvent <S3Event> S3Event::BUCKET_CREATED;
ofEvent <S3Event> S3Event::ERROR_CREATE_BUCKET;
ofEvent <S3Event> S3Event::BUCKET_LISTED;
ofEvent <S3Event> S3Event::ERROR_LISTING;
ofEvent <S3Event> S3Event::FILE_DOWNLOADED;
ofEvent <S3Event> S3Event::ERROR_DOWNLOAD;
ofEvent <S3Event> S3Event::FILE_DELETED;
ofEvent <S3Event> S3Event::ERROR_DELETE;

