#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include "tclled.h"
#include <node.h>
#include <v8.h>

using namespace v8;

static const char *device = SPIFILE;

class TCLData {
    int ledCount;
    int deviceFD;
    tcl_buffer buffer;

public:
    TCLData(int count): ledCount(count) {
        deviceFD = open(device, O_WRONLY);
        spi_init(deviceFD);
        tcl_init(&buffer, ledCount);
        set_gamma(2.2, 2.2, 2.2);
    }

    ~TCLData() {
        tcl_free(&buffer);
        close(deviceFD);
    }

    void blank() {
        for (int i = 0; i < ledCount; ++i) {
            write_gamma_color(&buffer.pixels[i], 0, 0, 0);
        }
    }

    void blue(int led) {
        write_gamma_color(&buffer.pixels[led], 0x00, 0x00, 0xff);
    }

    void send() {
        send_buffer(deviceFD, &buffer);
    }
};

static TCLData *data = NULL;

Handle<Value> Blank(const Arguments& args) {
    data->blank();
    return args.This();
}

Handle<Value> Blue(const Arguments& args) {
    data->blue(args[0]->IntegerValue());
    return args.This();
}

Handle<Value> Send(const Arguments& args) {
    data->send();
    return args.This();
}

static void cleanup(void *arg) {
    delete (TCLData*)arg;
}

void init(Handle<Object> target) {
    node::AtExit(cleanup, data = new TCLData(100));

    NODE_SET_METHOD(target, "blank", Blank);
    NODE_SET_METHOD(target, "blue", Blue);
    NODE_SET_METHOD(target, "send", Send);
}

NODE_MODULE(offgrid, init);
