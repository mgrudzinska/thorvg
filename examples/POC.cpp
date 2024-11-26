/*
 * Copyright (c) 2020 - 2024 the ThorVG project. All rights reserved.

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
    Scene* doc = nullptr;
    unique_ptr<tvg::Animation> page2 = nullptr;
    unique_ptr<tvg::Animation> page6 = nullptr;
    unique_ptr<tvg::Animation> page7 = nullptr;
    unique_ptr<tvg::Animation> page8 = nullptr;
    Text* page9 = nullptr;

    bool content(Canvas* canvas, uint32_t w, uint32_t h) override
    {
        if (!canvas) return false;

        if (!tvgexam::verify(Text::load(EXAMPLE_DIR"/font/Arial.ttf"))) return false;

        //Background
        auto bg = Shape::gen();
        bg->appendRect(0, 0, w, h);
        bg->fill(255, 255, 255);
        canvas->push(std::move(bg));

        //document
        doc = Scene::gen().release();

        //page 1
        {
            auto page = Scene::gen();

            //svg
            char buf[PATH_MAX];
            snprintf(buf, sizeof(buf), EXAMPLE_DIR"/poc/page1.svg");
            auto picture = Picture::gen();
            picture->load(buf);
            picture->scale(0.409091f);
            page->push(std::move(picture));

            doc->push(std::move(page));
        }

        //page 2
        {
            auto page = Scene::gen();
            page->translate(0, 1700);

            //svg
            {
                auto picture = Picture::gen();
                char buf[PATH_MAX];
                snprintf(buf, sizeof(buf), EXAMPLE_DIR"/poc/page2.svg");
                picture->load(buf);
                picture->translate(100, 0);
                picture->scale(2.0f);
                page->push(std::move(picture));
            }

            //lottie
            {
                page2 = unique_ptr<tvg::Animation>(tvg::Animation::gen());
                auto picture = page2->picture();
                picture->load(EXAMPLE_DIR"/poc/page2.json");
                picture->scale(1.3f);
                picture->translate(300, 1550);
                page->push(tvg::cast(picture));
            }

            doc->push(std::move(page));
        }

        //page 3
        {
            auto page = Scene::gen();
            page->translate(0, 3650);

            //svg
            auto picture = Picture::gen();
            char buf[PATH_MAX];
            snprintf(buf, sizeof(buf), EXAMPLE_DIR"/poc/page3.svg");
            picture->load(buf);
            picture->translate(100, 0);
            picture->scale(1.5f);
            page->push(std::move(picture));

            doc->push(std::move(page));
        }

        //page 4
        {
            auto page = Scene::gen();
            page->translate(0, 5100);

            //svg
            auto picture = Picture::gen();
            char buf[PATH_MAX];
            snprintf(buf, sizeof(buf), EXAMPLE_DIR"/poc/page4.svg");
            picture->load(buf);
            picture->scale(1.3f);
            page->push(std::move(picture));

            doc->push(std::move(page));
        }

        //page 5
        {
            auto page = Scene::gen();
            page->translate(0, 6100);

            //svg
            auto picture = Picture::gen();
            char buf[PATH_MAX];
            snprintf(buf, sizeof(buf), EXAMPLE_DIR"/poc/page5.svg");
            picture->load(buf);
            picture->scale(1.3f);
            page->push(std::move(picture));

            doc->push(std::move(page));
        }


        //page 6
        {
            auto page = Scene::gen();
            page->translate(0, 7100);

            //svg
            auto picture = Picture::gen();
            char buf[PATH_MAX];
            snprintf(buf, sizeof(buf), EXAMPLE_DIR"/poc/page6.svg");
            picture->load(buf);
            picture->scale(1.3f);
            page->push(std::move(picture));

            //lottie
            {
                page6 = unique_ptr<tvg::Animation>(tvg::Animation::gen());
                auto picture = page6->picture();
                picture->load(EXAMPLE_DIR"/poc/page6.json");
                picture->scale(0.68f);
                picture->translate(730, 175);
                page->push(tvg::cast(picture));
            }

            doc->push(std::move(page));
        }


        //page 7
        {
            auto page = Scene::gen();
            page->translate(0, 8200);

            //lottie
            page7 = tvg::Animation::gen();
            auto picture = page7->picture();
            picture->load(EXAMPLE_DIR"/poc/page6.json");
            picture->scale(1.5);
            page->push(tvg::cast(picture));

            //lottie
            auto text = tvg::Text::gen();
            text->font("Arial", 60);
            text->text("Long, long ago,");
            text->translate(250, 100);
            text->fill(255, 255, 255);
            page->push(std::move(text));

            auto text2 = tvg::Text::gen();
            text2->font("Arial", 60);
            text2->text("a kind and humble ghost");
            text2->translate(250, 200);
            text2->fill(255, 255, 255);
            page->push(std::move(text2));

            auto text3 = tvg::Text::gen();
            text3->font("Arial", 60);
            text3->text("lived near our village.");
            text3->translate(250, 300);
            text3->fill(255, 255, 255);
            page->push(std::move(text3));

            auto text4 = tvg::Text::gen();
            text4->font("Arial", 60);
            text4->text("Its name is \"Jinny\"...");
            text4->translate(250, 400);
            text4->fill(255, 255, 255);
            page->push(std::move(text4));

            doc->push(std::move(page));
        }


        //page 8
        {
            auto page = Scene::gen();
            page->translate(0, 10000);

            //lottie
            page8 = tvg::Animation::gen();
            auto picture = page8->picture();
            picture->load(EXAMPLE_DIR"/poc/video.json");
            picture->scale(0.75);
            page->push(tvg::cast(picture));

            //text
            auto text = tvg::Text::gen();
            text->font("Arial", 50);
            text->text("This is a Shorts:");
            text->translate(50, -100);
            text->fill(0, 0, 0);
            page->push(std::move(text));

            doc->push(std::move(page));
        }

        //page 9
        {
            auto page = Scene::gen();
            page->translate(0, 11000);

            //text
            page9 = Text::gen().release();
            page9->font("Arial", 100, "bold");
            page9->text("Thank You. :)");
            page9->translate(300, 200);
            page9->fill(0, 0, 0);
            page->push(tvg::cast(page9));

            doc->push(std::move(page));
        }

        canvas->push(tvg::cast(doc));

        return true;
    }

    bool scrolled(Canvas* canvas, int32_t x, int32_t y) override
    {
        #define SCROLL_SPEED 80

        dy += y * SCROLL_SPEED;

        if (dy < 0) dy = 0;
        if (dy > 10000) dy = 10000;

        doc->translate((float)dx, -(float)dy);

        printf("%d %d\n", dx, dy);

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