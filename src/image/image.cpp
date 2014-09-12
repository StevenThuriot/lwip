#include "image.h"

Persistent<FunctionTemplate> LwipImage::constructor;

void LwipImage::Init(Handle<Object> exports) {
    // Prepare constructor template
    Local<FunctionTemplate> tpl = NanNew<FunctionTemplate>(New);
    tpl->InstanceTemplate()->SetInternalFieldCount(1);
    tpl->SetClassName(NanNew("LwipImage"));
    NODE_SET_PROTOTYPE_METHOD(tpl, "width", width);
    NODE_SET_PROTOTYPE_METHOD(tpl, "height", height);
    NODE_SET_PROTOTYPE_METHOD(tpl, "buffer", buffer);
    NODE_SET_PROTOTYPE_METHOD(tpl, "resize", resize);
    NODE_SET_PROTOTYPE_METHOD(tpl, "rotate", rotate);
    NODE_SET_PROTOTYPE_METHOD(tpl, "blur", blur);
    NODE_SET_PROTOTYPE_METHOD(tpl, "crop", crop);
    NODE_SET_PROTOTYPE_METHOD(tpl, "mirror", mirror);
    NODE_SET_PROTOTYPE_METHOD(tpl, "pad", pad);
    NanAssignPersistent(constructor, tpl);
    exports->Set(
        NanNew("LwipImage"),
        NanNew<FunctionTemplate>(constructor)->GetFunction()
    );
}

LwipImage::LwipImage(unsigned char * data, size_t width, size_t height) {
    // TODO: CImg constructor may throw an exception. handle it in LwipImage::New.
    _cimg = new CImg<unsigned char>(data, width, height, 1, 3, false);
}

LwipImage::~LwipImage() {
    delete _cimg;
};

Handle<Value> LwipImage::NewInstance() {
    NanEscapableScope();
    Local<FunctionTemplate> constructorHandle = NanNew<FunctionTemplate>(constructor);
    Local<Object> instance = constructorHandle->GetFunction()->NewInstance();
    return NanEscapeScope(instance);
}

NAN_METHOD(LwipImage::New) {
    NanScope();
    // args[0] - pixels buffer
    // args[1,2] - width and height
    Local<Object> pixBuff = args[0].As<Object>();
    size_t width = args[1]->NumberValue();
    size_t height = args[2]->NumberValue();
    unsigned char * pixels = (unsigned char *)Buffer::Data(pixBuff);
    // TODO: handle CImg exception
    LwipImage * obj = new LwipImage(pixels, width, height);
    obj->Wrap(args.This());
    NanReturnValue(args.This());
}

// IMAGE JS OBJECT METHODS:
// ========================

// image.width():
// --------------
NAN_METHOD(LwipImage::width) {
    NanScope();
    LwipImage * obj = ObjectWrap::Unwrap<LwipImage>(args.Holder());
    NanReturnValue(NanNew<Number>(obj->_cimg->width()));
}

// image.height():
// ---------------
Handle<Value> LwipImage::height(const Arguments & args) {
    NanScope();
    LwipImage * obj = ObjectWrap::Unwrap<LwipImage>(args.Holder());
    NanReturnValue(NanNew<Number>(obj->_cimg->height()));
}

// image.buffer():
// ---------------
Handle<Value> LwipImage::buffer(const Arguments & args) {
    NanScope();
    LwipImage * obj = ObjectWrap::Unwrap<LwipImage>(args.Holder());
    // return a new buffer. don't use same memory an image. make a copy.
    // image object may be gc'ed, but buffer needs to stay alive.
    NanReturnValue(NanNewBufferHandle((char *)obj->_cimg->data(), obj->_cimg->size()));
}

// image.resize(width, height, inter, callback):
// ---------------------------------------------

// args[0] - width
// args[1] - height
// args[2] - inter(polation)
// args[3] - callback
Handle<Value> LwipImage::resize(const Arguments & args) {
    NanScope();

    size_t width = args[0].As<Integer>()->Value();
    size_t height = args[1].As<Integer>()->Value();
    int inter = args[2].As<Integer>()->Value();
    NanCallback * callback = new NanCallback(args[3].As<Function>());
    CImg<unsigned char> * cimg = ObjectWrap::Unwrap<LwipImage>(args.This())->_cimg;

    NanAsyncQueueWorker(
        new ResizeWorker(
            width,
            height,
            inter,
            cimg,
            callback
        )
    );

    NanReturnUndefined();
}

// image.rotate(degs, inter, callback):
// ------------------------------------

// args[0] - degs
// args[1] - R
// args[2] - G
// args[3] - B
// args[4] - callback
Handle<Value> LwipImage::rotate(const Arguments & args) {
    NanScope();

    float degs = (float) args[0].As<Number>()->Value();
    unsigned char r = (unsigned char) args[1].As<Integer>()->Value();
    unsigned char g = (unsigned char) args[2].As<Integer>()->Value();
    unsigned char b = (unsigned char) args[3].As<Integer>()->Value();
    NanCallback * callback = new NanCallback(args[4].As<Function>());
    CImg<unsigned char> * cimg = ObjectWrap::Unwrap<LwipImage>(args.This())->_cimg;

    NanAsyncQueueWorker(
        new RotateWorker(
            degs,
            r,
            g,
            b,
            cimg,
            callback
        )
    );

    NanReturnUndefined();
}

// image.blur(sigma, callback):
// ----------------------------

// args[0] - sigma
// args[1] - callback
Handle<Value> LwipImage::blur(const Arguments & args) {
    NanScope();

    float sigma = (float) args[0].As<Number>()->Value();
    NanCallback * callback = new NanCallback(args[1].As<Function>());
    CImg<unsigned char> * cimg = ObjectWrap::Unwrap<LwipImage>(args.This())->_cimg;

    NanAsyncQueueWorker(
        new BlurWorker(
            sigma,
            cimg,
            callback
        )
    );

    NanReturnUndefined();
}

// image.crop(left, top, right, bottom, callback):
// -----------------------------------------------

// args[0] - left
// args[1] - top
// args[2] - right
// args[3] - bottom
// args[4] - callback
Handle<Value> LwipImage::crop(const Arguments & args) {
    NanScope();

    size_t left = (size_t) args[0].As<Number>()->Value();
    size_t top = (size_t) args[1].As<Number>()->Value();
    size_t right = (size_t) args[2].As<Number>()->Value();
    size_t bottom = (size_t) args[3].As<Number>()->Value();
    NanCallback * callback = new NanCallback(args[4].As<Function>());
    CImg<unsigned char> * cimg = ObjectWrap::Unwrap<LwipImage>(args.This())->_cimg;

    NanAsyncQueueWorker(
        new CropWorker(
            left,
            top,
            right,
            bottom,
            cimg,
            callback
        )
    );

    NanReturnUndefined();
}

// image.mirror(xaxis, yaxis, callback):
// -----------------------------------------------

// args[0] - xaxis (boolean)
// args[1] - yaxis (boolean)
// args[2] - callback
Handle<Value> LwipImage::mirror(const Arguments & args) {
    NanScope();

    bool xaxis = args[0]->BooleanValue();
    bool yaxis = args[1]->BooleanValue();
    NanCallback * callback = new NanCallback(args[2].As<Function>());
    CImg<unsigned char> * cimg = ObjectWrap::Unwrap<LwipImage>(args.This())->_cimg;

    NanAsyncQueueWorker(
        new MirrorWorker(
            xaxis,
            yaxis,
            cimg,
            callback
        )
    );

    NanReturnUndefined();
}

// image.pad(left, top, right, bottom, color, callback):
// -----------------------------------------------------

// args[0] - left
// args[1] - top
// args[2] - right
// args[3] - bottom
// args[4] - red
// args[5] - green
// args[6] - blue
// args[7] - callback
Handle<Value> LwipImage::pad(const Arguments & args) {
    NanScope();

    size_t left = (size_t) args[0].As<Number>()->Value();
    size_t top = (size_t) args[1].As<Number>()->Value();
    size_t right = (size_t) args[2].As<Number>()->Value();
    size_t bottom = (size_t) args[3].As<Number>()->Value();
    unsigned char r = (unsigned char) args[4].As<Integer>()->Value();
    unsigned char g = (unsigned char) args[5].As<Integer>()->Value();
    unsigned char b = (unsigned char) args[6].As<Integer>()->Value();
    NanCallback * callback = new NanCallback(args[7].As<Function>());
    CImg<unsigned char> * cimg = ObjectWrap::Unwrap<LwipImage>(args.This())->_cimg;

    NanAsyncQueueWorker(
        new PadWorker(
            left,
            top,
            right,
            bottom,
            r,
            g,
            b,
            cimg,
            callback
        )
    );

    NanReturnUndefined();
}
