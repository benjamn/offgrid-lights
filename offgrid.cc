#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

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

        if (deviceFD < 0) {
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
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

    ~TCLData() {
        tcl_free(&buffer);
        close(deviceFD);
    }

    void blank() {
        for (int i = 0; i < ledCount; ++i) {
            write(i, 0, 0, 0);
        }
    }

    void write(int led, uint8_t r, uint8_t g, uint8_t b) {
        if (led >= 0 &&
            led < ledCount) {
            write_gamma_color(&buffer.pixels[led], r, g, b);
        }
    }

    void red(int led) {
        write(led, 0xff, 0, 0);
    }

    void green(int led) {
        write(led, 0, 0xff, 0);
    }

    void blue(int led) {
        write(led, 0, 0, 0xff);
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

Handle<Value> Write(const Arguments& args) {
    data->write(args[0]->IntegerValue(),
                args[1]->IntegerValue(),
                args[2]->IntegerValue(),
                args[3]->IntegerValue());
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
    node::AtExit(cleanup, data = new TCLData(575));

    NODE_SET_METHOD(target, "blank", Blank);
    NODE_SET_METHOD(target, "blue", Blue);
    NODE_SET_METHOD(target, "write", Write);
    NODE_SET_METHOD(target, "send", Send);
}

NODE_MODULE(offgrid, init);
