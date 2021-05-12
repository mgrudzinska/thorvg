/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd. All rights reserved.

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <string.h>
#include <fstream>
#include "tvgTvgSaver.h"
#include "tvgTvgHelper.h"

/************************************************************************/
/* Internal Class Implementation                                        */
/************************************************************************/

/************************************************************************/
/* External Class Implementation                                        */
/************************************************************************/

TvgSaver::TvgSaver(Paint* paint) : root(paint)
{
}


TvgSaver::~TvgSaver()
{
    close();
}


bool TvgSaver::open(const string& path)
{
    reserved = 256;
    buffer = static_cast<char*>(malloc(reserved));
    if (!buffer) {
        reserved = 0;
        return false;
    }
    return true;
}


bool TvgSaver::write()
{
    if (!buffer) return false;

    TaskScheduler::request(this);

    return true;
}


void TvgSaver::run(unsigned tid)
{
    root->serialize();  //PROBLEM - in this point I need to start somehow the serialization process
};


bool TvgSaver::close()
{
    this->done();

    if (buffer) free(buffer);

    return true;
}


void TvgSaver::saveMemberIndicator(TvgIndicator ind)
{
    printf("%s %s \n", __FILE__, __func__);
    return;
}


void TvgSaver::saveMemberDataSize(ByteCounter byteCnt)
{
    printf("%s %s \n", __FILE__, __func__);
    return;
}
