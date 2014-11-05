#include "RawImages.h"

using namespace ci;
using namespace ci::app;

class RawImagesApp : public AppNative {
  public:
    gl::Texture tex;
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    void prepareSettings( ci::app::AppBasic::Settings* settings );
};

void RawImagesApp::setup()
{
    controller.setPolicyFlags(controller.POLICY_IMAGES);
}

void RawImagesApp::mouseDown( MouseEvent event )
{
}

void RawImagesApp::update()
{
    Frame frame = controller.frame();
    ImageList images = frame.images();
    for(int i = 0; i < 2; i++){
        Image image = images[i];
        const unsigned char* image_buffer = image.data();
        Surface surface(image.width(), image.height(), true, SurfaceChannelOrder::RGBA);
        int cursor = 0;
        Surface::Iter iter = surface.getIter();
        while( iter.line() ) {
            while( iter.pixel() ) {
                iter.r() = image_buffer[cursor];
                iter.g() = 0;
                iter.b() = 0;
                iter.a() = 255;
                cursor++;
            }
        }
        glPushMatrix();
        tex = gl::Texture(surface);
        glScalef(2.0, 3.0, 0.0);
        gl::draw(tex);
        glPopMatrix();
    }

}

void RawImagesApp::draw()
{
}

void RawImagesApp::prepareSettings( Settings* settings )
{
	settings->setWindowSize( 1024, 768 );
	settings->setFrameRate( 60.0f );
}

CINDER_APP_NATIVE( RawImagesApp, RendererGl )
