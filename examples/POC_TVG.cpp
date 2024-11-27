/*
 * Copyright (c) 2024 the ThorVG project. All rights reserved.

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

#include "Example.h"


/************************************************************************/
/* ThorVG Drawing Contents                                              */
/************************************************************************/

using namespace tvg;

struct UserExample : tvgexam::Example
{
    int dx = 0, dy = 0;
    Picture* picture = nullptr;
    bool initialized = false;
    Picture* page2 = nullptr;
    Picture* page6 = nullptr;
    Picture* page7 = nullptr;
    Picture* page8 = nullptr;
    Shape* page9 = nullptr;
    uint32_t maxY = 9560;

    bool content(Canvas* canvas, uint32_t w, uint32_t h) override
    {
        if (!canvas) return false;

        if (!tvgexam::verify(Text::load(EXAMPLE_DIR"/font/Arial.ttf"))) return false;

        picture = Picture::gen().release();
        if (!tvgexam::verify(picture->load(EXAMPLE_DIR"/tvg/poc.tvg"), "Error while loading \"poc.tvg\".")) {
            return false;
        }

        if (!initialized) {
            initialized = true;

            if (auto paint = picture->paint(99910)) {
                page2 = (Picture*)paint;
            }
            if (auto paint = picture->paint(99911)) {
                page6 = (Picture*)paint;
            }
            if (auto paint = picture->paint(99912)) {
                page7 = (Picture*)paint;
            }
            if (auto paint = picture->paint(99913)) {
                page8 = (Picture*)paint;
            }
            if (auto paint = picture->paint(99914)) {
                page9 = (Shape*)paint;
            }
        }

        canvas->push(tvg::cast(picture));

        return true;
    }

    bool scrolled(Canvas* canvas, int32_t x, int32_t y) override
    {
#define SCROLL_SPEED 80

        dy -= y * SCROLL_SPEED;

        if (dy < 0) dy = 0;
        if (dy > maxY) dy = maxY;

        picture->translate((float)dx, (float)-dy);

        return true;
    }


    bool page(Canvas* canvas, int direction) override
    {
#define PAGE_SIZE 1000

        dy -= direction * PAGE_SIZE;

        if (dy < 0) dy = 0;
        if (dy > maxY) dy = maxY;

        picture->translate((float)dx, (float)-dy);

        return true;
    }

    bool update(tvg::Canvas* canvas, uint32_t elapsed) override
    {
        {
            auto progress = tvgexam::progress(elapsed, page2->duration());
            page2->frame(page2->totalFrame() * progress);
        }

        {
            auto progress = tvgexam::progress(elapsed, page6->duration());
            page6->frame(page6->totalFrame() * progress);
        }

        {
            auto progress = tvgexam::progress(elapsed, page7->duration());
            page7->frame(page7->totalFrame() * progress);
        }

        {
            auto progress = tvgexam::progress(elapsed, page8->duration());
            page8->frame(page8->totalFrame() * progress);
        }

        {
            page9->fill(rand() % 255, rand() % 255, rand() % 255);
        }

        canvas->update();

        return true;
    }

};


/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    return tvgexam::main(new UserExample, argc, argv, 1400, 1800);
}