#include "Common.h"
#include <fstream>

/************************************************************************/
/* Drawing Commands                                                     */
/************************************************************************/

uint32_t *data = nullptr;

void tvgDrawCmds(tvg::Canvas* canvas)
{
    if (!canvas) return;

    int w = 200, h = 300;
    string path(EXAMPLE_DIR"/rawimage_200x300.raw");
    ifstream file(path);
    if (!file.is_open()) return ;
    data = (uint32_t*)malloc(sizeof(uint32_t) * (w*h));
    file.read(reinterpret_cast<char*>(data), sizeof(data) * (w*h));
    file.close();


/*
////////////////////////////////////////////////////////////////////////////////////////////////////
// 1 _rasterTranslucentImageRle - identity
    auto picture1 = tvg::Picture::gen();
    if (picture1->load(data, 200, 300, true) != tvg::Result::Success) return;
    picture1->opacity(128);

    auto circle1 = tvg::Shape::gen();
    circle1->appendCircle(0, 0, 100, 150);
    circle1->fill(255, 0, 0, 255);
    picture1->composite(move(circle1), tvg::CompositeMethod::ClipPath);

    canvas->push(move(picture1));


// 2 _rasterTranslucentImageRle - translated
    auto picture2 = tvg::Picture::gen();
    if (picture2->load(data, 200, 300, true) != tvg::Result::Success) return;
    picture2->opacity(128);
    picture2->translate(300, 0);

    auto circle2 = tvg::Shape::gen();
    circle2->appendCircle(300, 0, 100, 150);
    circle2->fill(255, 0, 0, 255);
    picture2->composite(move(circle2), tvg::CompositeMethod::ClipPath);

    canvas->push(move(picture2));


// 3 _rasterTranslucentImageRle - transformed
    auto picture3 = tvg::Picture::gen();
    if (picture3->load(data, 200, 300, true) != tvg::Result::Success) return;
    picture3->opacity(128);
    picture3->translate(600, 0);
    picture3->rotate(1);

    auto circle3 = tvg::Shape::gen();
    circle3->appendCircle(600, 0, 100, 150);
    circle3->fill(255, 0, 0, 255);
    picture3->composite(move(circle3), tvg::CompositeMethod::ClipPath);

    canvas->push(move(picture3));


////////////////////////////////////////////////////////////////////////////////////////////////////
// 4 _rasterImageRle - identity
    auto picture4 = tvg::Picture::gen();
    if (picture4->load(data, 200, 300, true) != tvg::Result::Success) return;

    auto circle4 = tvg::Shape::gen();
    circle4->appendCircle(0, 300, 100, 150);
    circle4->fill(255, 0, 0, 255);
    picture4->composite(move(circle4), tvg::CompositeMethod::ClipPath);

    canvas->push(move(picture4));


// 5 _rasterImageRle - translated
    auto picture5 = tvg::Picture::gen();
    if (picture5->load(data, 200, 300, true) != tvg::Result::Success) return;
    picture5->translate(300, 0);

    auto circle5 = tvg::Shape::gen();
    circle5->appendCircle(300, 300, 100, 150);
    circle5->fill(255, 0, 0, 255);
    picture5->composite(move(circle5), tvg::CompositeMethod::ClipPath);

    canvas->push(move(picture5));


// 6 _rasterImageRle - transformed
    auto picture6 = tvg::Picture::gen();
    if (picture6->load(data, 200, 300, true) != tvg::Result::Success) return;
    picture6->translate(600, 0);
    picture6->rotate(1);

    auto circle6 = tvg::Shape::gen();
    circle6->appendCircle(600, 300, 100, 150);
    circle6->fill(255, 0, 0, 255);
    picture6->composite(move(circle6), tvg::CompositeMethod::ClipPath);

    canvas->push(move(picture6));


////////////////////////////////////////////////////////////////////////////////////////////////////
// 7 _translucentImage - identity
    auto picture7 = tvg::Picture::gen();
    if (picture7->load(data, 200, 300, true) != tvg::Result::Success) return;
    picture7->opacity(200);

    canvas->push(move(picture7));


// 11 _translucentImage - translated
    auto picture11 = tvg::Picture::gen();
    if (picture11->load(data, 200, 300, true) != tvg::Result::Success) return;
    picture11->opacity(200);
    picture11->translate(300, 0);

    canvas->push(move(picture11));


// 15 _translucentImage - transformed
    auto picture15 = tvg::Picture::gen();
    if (picture15->load(data, 200, 300, true) != tvg::Result::Success) return;
    picture15->opacity(200);
    picture15->translate(600, 0);
    picture15->rotate(1);

    canvas->push(move(picture15));


////////////////////////////////////////////////////////////////////////////////////////////////////
// 8 _translucentImageAlphaMask - identity
    auto picture8 = tvg::Picture::gen();
    if (picture8->load(data, 200, 300, true) != tvg::Result::Success) return;
    picture8->opacity(200);

    auto circle8 = tvg::Shape::gen();
    circle8->appendCircle(100, 100, 100, 100);
    circle8->fill(255, 0, 0, 28);
    picture8->composite(move(circle8), tvg::CompositeMethod::AlphaMask);

    canvas->push(move(picture8));


// 12 _translucentImageAlphaMask - translated
    auto picture12 = tvg::Picture::gen();
    if (picture12->load(data, 200, 300, true) != tvg::Result::Success) return;
    picture12->opacity(200);
    picture12->translate(300, 0);

    auto circle12 = tvg::Shape::gen();
    circle12->appendCircle(400, 100, 100, 100);
    circle12->fill(255, 0, 0, 28);
    picture12->composite(move(circle12), tvg::CompositeMethod::AlphaMask);

    canvas->push(move(picture12));


// 16 _translucentImageAlphaMask - transformed
    auto picture16 = tvg::Picture::gen();
    if (picture16->load(data, 200, 300, true) != tvg::Result::Success) return;
    picture16->opacity(200);
    picture16->translate(600, 0);
    picture16->rotate(1);

    auto circle16 = tvg::Shape::gen();
    circle16->appendCircle(700, 100, 100, 100);
    circle16->fill(255, 0, 0, 28);
    picture16->composite(move(circle16), tvg::CompositeMethod::AlphaMask);

    canvas->push(move(picture16));


////////////////////////////////////////////////////////////////////////////////////////////////////
// 9 _translucentImageInvAlphaMask - identity
    auto picture9 = tvg::Picture::gen();
    if (picture9->load(data, 200, 300, true) != tvg::Result::Success) return;
    picture9->opacity(200);

    auto circle9 = tvg::Shape::gen();
    circle9->appendCircle(100, 100, 100, 100);
    circle9->fill(255, 0, 0, 25);
    picture9->composite(move(circle9), tvg::CompositeMethod::InvAlphaMask);

    canvas->push(move(picture9));


// 13 _translucentImageInvAlphaMask - identity
    auto picture13 = tvg::Picture::gen();
    if (picture13->load(data, 200, 300, true) != tvg::Result::Success) return;
    picture13->opacity(200);
    picture13->translate(300, 0);

    auto circle13 = tvg::Shape::gen();
    circle13->appendCircle(400, 100, 100, 100);
    circle13->fill(255, 0, 0, 25);
    picture13->composite(move(circle13), tvg::CompositeMethod::InvAlphaMask);

    canvas->push(move(picture13));


// 17  _translucentImageInvAlphaMask - identity
    auto picture17 = tvg::Picture::gen();
    if (picture17->load(data, 200, 300, true) != tvg::Result::Success) return;
    picture17->opacity(200);
    picture17->translate(600, 0);
    picture17->rotate(1);

    auto circle17 = tvg::Shape::gen();
    circle17->appendCircle(700, 100, 100, 100);
    circle17->fill(255, 0, 0, 25);
    picture17->composite(move(circle17), tvg::CompositeMethod::InvAlphaMask);

    canvas->push(move(picture17));
*/

////////////////////////////////////////////////////////////////////////////////////////////////////
// 19 _rasterImage - identity
    auto picture19 = tvg::Picture::gen();
    if (picture19->load(data, 200, 300, true) != tvg::Result::Success) return;

    canvas->push(move(picture19));


// 20 _rasterImage - translated
    auto picture20 = tvg::Picture::gen();
    if (picture20->load(data, 200, 300, true) != tvg::Result::Success) return;
    picture20->translate(300, 0);

    canvas->push(move(picture20));


// 21 _rasterImage - transformed
    auto picture21 = tvg::Picture::gen();
    if (picture21->load(data, 200, 300, true) != tvg::Result::Success) return;
    picture21->translate(600, 0);
    picture21->rotate(1);

    canvas->push(move(picture21));


/*
////////////////////////////////////////////////////////////////////////////////////////////////////
// user's matrix
    auto picture22 = tvg::Picture::gen();
    if (picture22->load(data, 200, 300, true) != tvg::Result::Success) return;

    tvg::Matrix m = {1,0,0, 0,0,0, 0,0,1};
    picture22->transform(m);

    canvas->push(move(picture22));
*/
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
    auto threads = std::thread::hardware_concurrency();

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
        tvg::Initializer::term(tvg::CanvasEngine::Sw);

        if (data) free(data);

    } else {
        cout << "engine is not supported" << endl;
    }
    return 0;
}
