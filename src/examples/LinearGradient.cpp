#include "Common.h"

/************************************************************************/
/* Drawing Commands                                                     */
/************************************************************************/

void tvgDrawCmds(tvg::Canvas* canvas)
{
    if (!canvas) return;

    //Gradient Color Stops
    tvg::Fill::ColorStop colorStops[2];
    colorStops[0] = {0, 0, 255, 255, 255};
    colorStops[1] = {1, 255, 0, 0, 255};

    tvg::Fill::ColorStop colorStopsRev[2];
    colorStopsRev[0] = {0, 255, 0, 0, 255};
    colorStopsRev[1] = {1, 0, 255, 255, 255};

    //Rect
    auto shape1 = tvg::Shape::gen();
    shape1->appendRect(0, 0, 400, 400, 0, 0);

    //LinearGradient
    auto fill1 = tvg::LinearGradient::gen();
    fill1->linear(0, 0, 400, 400);
    fill1->colorStops(colorStops, 2);
    shape1->fill(move(fill1));
    if (canvas->push(move(shape1)) != tvg::Result::Success) return;

    //Scaled rect - scaled to be as shape 1 but with rounded corners
    auto shape2 = tvg::Shape::gen();
    shape2->appendRect(0, 0, 10, 10, 4, 4);
    shape2->scale(40);

    //LinearGradient
    auto fill2 = tvg::LinearGradient::gen();
    fill2->linear(0, 0, 10, 10);
    fill2->colorStops(colorStops, 2);
    shape2->fill(move(fill2));
    if (canvas->push(move(shape2)) != tvg::Result::Success) return;

    //small rect
    auto shape3 = tvg::Shape::gen();
    shape3->appendRect(0, 0, 200, 200, 0, 0);
    shape3->rotate(180);
    shape3->translate(300, 300);

    //LinearGradient
    auto fill3 = tvg::LinearGradient::gen();
    fill3->linear(0, 0, 200, 200);
    fill3->colorStops(colorStopsRev, 2);
    shape3->fill(move(fill3));
    if (canvas->push(move(shape3)) != tvg::Result::Success) return;
}


/************************************************************************/
/* Sw Engine Test Code                                                  */
/************************************************************************/

static unique_ptr<tvg::SwCanvas> swCanvas;

void tvgSwTest(uint32_t* buffer)
{
    //Create a Canvas
    swCanvas = tvg::SwCanvas::gen();
    swCanvas->target(buffer, WIDTH, WIDTH, HEIGHT, tvg::SwCanvas::ARGB8888);

    /* Push the shape into the Canvas drawing list
       When this shape is into the canvas list, the shape could update & prepare
       internal data asynchronously for coming rendering.
       Canvas keeps this shape node unless user call canvas->clear() */
    tvgDrawCmds(swCanvas.get());
}

void drawSwView(void* data, Eo* obj)
{
    if (swCanvas->draw() == tvg::Result::Success) {
        swCanvas->sync();
    }
}


/************************************************************************/
/* GL Engine Test Code                                                  */
/************************************************************************/

static unique_ptr<tvg::GlCanvas> glCanvas;

void initGLview(Evas_Object *obj)
{
    static constexpr auto BPP = 4;

    //Create a Canvas
    glCanvas = tvg::GlCanvas::gen();
    glCanvas->target(nullptr, WIDTH * BPP, WIDTH, HEIGHT);

    /* Push the shape into the Canvas drawing list
       When this shape is into the canvas list, the shape could update & prepare
       internal data asynchronously for coming rendering.
       Canvas keeps this shape node unless user call canvas->clear() */
    tvgDrawCmds(glCanvas.get());
}

void drawGLview(Evas_Object *obj)
{
    auto gl = elm_glview_gl_api_get(obj);
    gl->glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    gl->glClear(GL_COLOR_BUFFER_BIT);

    if (glCanvas->draw() == tvg::Result::Success) {
        glCanvas->sync();
    }
}


/************************************************************************/
/* Main Code                                                            */
/************************************************************************/

int main(int argc, char **argv)
{
    tvg::CanvasEngine tvgEngine = tvg::CanvasEngine::Sw;

    if (argc > 1) {
        if (!strcmp(argv[1], "gl")) tvgEngine = tvg::CanvasEngine::Gl;
    }

    //Initialize ThorVG Engine
    if (tvgEngine == tvg::CanvasEngine::Sw) {
        cout << "tvg engine: software" << endl;
    } else {
        cout << "tvg engine: opengl" << endl;
    }

    //Threads Count
    auto threads = 0;//std::thread::hardware_concurrency();

    //Initialize ThorVG Engine
    if (tvg::Initializer::init(tvgEngine, threads) == tvg::Result::Success) {

        elm_init(argc, argv);

        if (tvgEngine == tvg::CanvasEngine::Sw) {
            createSwView();
        } else {
            createGlView();
        }

        elm_run();
        elm_shutdown();

        //Terminate ThorVG Engine
        tvg::Initializer::term(tvgEngine);

    } else {
        cout << "engine is not supported" << endl;
    }
    return 0;
}
