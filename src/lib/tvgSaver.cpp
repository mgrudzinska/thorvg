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
#include "tvgSaverImpl.h"
#include <iostream>

/************************************************************************/
/* Internal Class Implementation                                        */
/************************************************************************/

/************************************************************************/
/* External Class Implementation                                        */
/************************************************************************/

Saver::Saver() : pImpl(new Impl(this))
{
}


Saver::~Saver()
{
    delete(pImpl);
}


Result Saver::save(std::unique_ptr<Paint> paint, const std::string& path) noexcept
{
    if (!paint || path.empty()) return Result::InvalidArguments;

    auto saver = unique_ptr<Saver>(new Saver());
    if (!saver) return Result::FailedAllocation;

    if (saver->pImpl->save(paint.release(), path)) return Result::Success;

    return Result::Unknown;
}


void Saver::saveMemberIndicator(TvgIndicator ind)
{
/*
    if (pImpl->size + TVG_INDICATOR_SIZE > pImpl->reserved) resizeBuffer(pImpl->size + TVG_INDICATOR_SIZE);

    memcpy(pImpl->bufferPosition, &ind, TVG_INDICATOR_SIZE);
    pImpl->bufferPosition += TVG_INDICATOR_SIZE;
    pImpl->size += TVG_INDICATOR_SIZE;
*/
}


void Saver::saveMemberDataSize(ByteCounter byteCnt)
{
/*
    if (pImpl->size + BYTE_COUNTER_SIZE > pImpl->reserved) resizeBuffer(pImpl->size + BYTE_COUNTER_SIZE);

    memcpy(pImpl->bufferPosition, &byteCnt, BYTE_COUNTER_SIZE);
    pImpl->bufferPosition += BYTE_COUNTER_SIZE;
    pImpl->size += BYTE_COUNTER_SIZE;
*/
}

