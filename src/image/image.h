#ifndef LWIP_IMAGE_H
#define LWIP_IMAGE_H

#define cimg_display 0
#define cimg_verbosity 0

#include <string>
#include <cmath>
#include <node.h>
#include <node_buffer.h>
#include <nan.h>
#include <v8.h>
#include "CImg.h"

using namespace cimg_library;
using namespace v8;
using namespace node;

/*
 * LwipImage object is a wrapper for a CImg object.
 * It is constructed from a memory biffer of RGB pixels.
 * The memory buffer is of type 'unsigned char' such that for an image of size
 * W * H the buffer has (W*H) R(ed) values, then (W*H) G(reen) values and then
 * (W*H) B(lue) values.
 */
class LwipImage : public node::ObjectWrap {
public:
    static void Init(Handle<Object> exports);
    static Handle<Value> NewInstance();
    static NAN_METHOD(New);
    static NAN_METHOD(resize);
    static NAN_METHOD(rotate);
    static NAN_METHOD(blur);
    static NAN_METHOD(crop);
    static NAN_METHOD(mirror);
    static NAN_METHOD(pad);
    static NAN_METHOD(width);
    static NAN_METHOD(height);
    static NAN_METHOD(buffer);
    LwipImage(unsigned char * data, size_t width, size_t height);
    ~LwipImage();
    CImg<unsigned char> * _cimg;
private:
    static Persistent<FunctionTemplate> constructor;
};

class ResizeWorker : public NanAsyncWorker {
public:
    ResizeWorker(
        size_t width,
        size_t height,
        int inter,
        CImg<unsigned char> * cimg,
        NanCallback * callback
    );
    ~ResizeWorker();
    void Execute ();
    void HandleOKCallback ();
private:
    size_t _width;
    size_t _height;
    int _inter;
    CImg<unsigned char> * _cimg;
};

class RotateWorker : public NanAsyncWorker {
public:
    RotateWorker(
        float degs,
        unsigned char r,
        unsigned char g,
        unsigned char b,
        CImg<unsigned char> * cimg,
        NanCallback * callback
    );
    ~RotateWorker();
    void Execute ();
    void HandleOKCallback ();
private:
    float _degs;
    unsigned char _r;
    unsigned char _g;
    unsigned char _b;
    CImg<unsigned char> * _cimg;
};

class BlurWorker : public NanAsyncWorker {
public:
    BlurWorker(
        float sigma,
        CImg<unsigned char> * cimg,
        NanCallback * callback
    );
    ~BlurWorker();
    void Execute ();
    void HandleOKCallback ();
private:
    float _sigma;
    CImg<unsigned char> * _cimg;
};

class CropWorker : public NanAsyncWorker {
public:
    CropWorker(
        size_t left,
        size_t top,
        size_t right,
        size_t bottom,
        CImg<unsigned char> * cimg,
        NanCallback * callback
    );
    ~CropWorker();
    void Execute ();
    void HandleOKCallback ();
private:
    size_t _left;
    size_t _top;
    size_t _right;
    size_t _bottom;
    CImg<unsigned char> * _cimg;
};

class MirrorWorker : public NanAsyncWorker {
public:
    MirrorWorker(
        bool xaxis,
        bool yaxis,
        CImg<unsigned char> * cimg,
        NanCallback * callback
    );
    ~MirrorWorker();
    void Execute ();
    void HandleOKCallback ();
private:
    bool _xaxis;
    bool _yaxis;
    CImg<unsigned char> * _cimg;
};

class PadWorker : public NanAsyncWorker {
public:
    PadWorker(
        size_t left,
        size_t top,
        size_t right,
        size_t bottom,
        unsigned char r,
        unsigned char g,
        unsigned char b,
        CImg<unsigned char> * cimg,
        NanCallback * callback
    );
    ~PadWorker();
    void Execute ();
    void HandleOKCallback ();
private:
    size_t _left;
    size_t _top;
    size_t _right;
    size_t _bottom;
    unsigned char _r;
    unsigned char _g;
    unsigned char _b;
    CImg<unsigned char> * _cimg;
};

#endif
