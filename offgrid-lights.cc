#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "offgrid-lights.h"
#include <node.h>
#include <v8.h>

using namespace v8;

static const char *device = SPIFILE;
static TCLData *data = NULL;

TCLData::TCLData(int count): ledCount(count) {
  deviceFD = open(device, O_WRONLY);

  if (deviceFD < 0) {
    fprintf(stderr, "Error %d opening %s: %s\n",
            errno, device, strerror(errno));
    exit(1);
  }

  if (spi_init(deviceFD) < 0) {
    fprintf(stderr, "SPI initialization error %d: %s\n", errno, strerror(errno));
    exit(1);
  }

  if (tcl_init(&buffer, ledCount) < 0) {
    fprintf(stderr, "Pixel buffer initialization error: Not enough memory.\n");
    exit(1);
  }

  set_gamma(2.2, 2.2, 2.2);

  blank();
}

TCLData::~TCLData() {
  tcl_free(&buffer);
  close(deviceFD);
}

void TCLData::blank() {
  for (int i = 0; i < ledCount; ++i) {
    write(i, 0, 0, 0);
  }
}

void TCLData::write(int led, uint8_t r, uint8_t g, uint8_t b) {
  if (led >= 0 &&
      led < ledCount) {
    write_gamma_color(&buffer.pixels[led], r, g, b);
  }
}

void TCLData::red(int led) {
  write(led, 0xff, 0, 0);
}

void TCLData::green(int led) {
  write(led, 0, 0xff, 0);
}

void TCLData::blue(int led) {
  write(led, 0, 0, 0xff);
}

void TCLData::send() {
  send_buffer(deviceFD, &buffer);
}

int TCLData::count() {
  return ledCount;
}

static void Blank(const FunctionCallbackInfo<Value>& args) {
  data->blank();
  args.GetReturnValue().Set(args.This());
}

static void Red(const FunctionCallbackInfo<Value>& args) {
  data->red(args[0]->IntegerValue());
  args.GetReturnValue().Set(args.This());
}

static void Green(const FunctionCallbackInfo<Value>& args) {
  data->green(args[0]->IntegerValue());
  args.GetReturnValue().Set(args.This());
}

static void Blue(const FunctionCallbackInfo<Value>& args) {
  data->blue(args[0]->IntegerValue());
  args.GetReturnValue().Set(args.This());
}

static void Write(const FunctionCallbackInfo<Value>& args) {
  data->write(args[0]->IntegerValue(),
              args[1]->IntegerValue(),
              args[2]->IntegerValue(),
              args[3]->IntegerValue());
  args.GetReturnValue().Set(args.This());
}

static void Send(const FunctionCallbackInfo<Value>& args) {
  data->send();
  args.GetReturnValue().Set(args.This());
}

static void Count(const FunctionCallbackInfo<Value>& args) {
  args.GetReturnValue().Set(Integer::New(args.GetIsolate(), data->count()));
}

static void Init(const FunctionCallbackInfo<Value>& args) {
  delete data;
  data = new TCLData(args[0]->IntegerValue());
  args.GetReturnValue().Set(args.This());
}

static void cleanup(void*) {
  delete data;
}

void init_lights(Local<Object> exports) {
  node::AtExit(cleanup, NULL);

  NODE_SET_METHOD(exports, "init", Init);
  NODE_SET_METHOD(exports, "blank", Blank);
  NODE_SET_METHOD(exports, "red", Red);
  NODE_SET_METHOD(exports, "green", Green);
  NODE_SET_METHOD(exports, "blue", Blue);
  NODE_SET_METHOD(exports, "write", Write);
  NODE_SET_METHOD(exports, "send", Send);
  NODE_SET_METHOD(exports, "count", Count);
}
