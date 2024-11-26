/*
 * Copyright (c) 2023 - 2024 the ThorVG project. All rights reserved.

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

struct UserExample : tvgexam::Example
{
    bool content(tvg::Canvas* canvas, uint32_t w, uint32_t h) override
    {
        if (!canvas) return false;

        //Background
        auto shape = tvg::Shape::gen();
        shape->appendRect(0, 0, w, h);
        shape->fill(75, 75, 75);
        canvas->push(std::move(shape));

         shape = tvg::Shape::gen();
        shape->appendRect(0, 0, 100, 100);
        shape->fill(255, 0, 0);
        shape->scale(0.8);
        shape->translate(50,200);
        canvas->push(std::move(shape));

        //Load a necessary font data.
        //The loaded font will be released when the Initializer::term() is called.
        //Otherwise, you can immediately unload the font data.
        //Please check Text::unload() APIs.
        if (!tvgexam::verify(tvg::Text::load(EXAMPLE_DIR"/font/Arial.ttf"))) return false;


        auto text = tvg::Text::gen();
        text->font("Arial", 81.3);
        text->text("THORVG Text");
        text->fill(255, 0, 0);
        // text->scale(0.8);
        // text->translate(50,200);
        canvas->push(std::move(text));



        return true;
    }
};


/************************************************************************/
/* Entry Point                                                          */
/************************************************************************/

int main(int argc, char **argv)
{
    return tvgexam::main(new UserExample, argc, argv, 800,800,0, false);
}